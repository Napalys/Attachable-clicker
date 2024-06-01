#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include "QMessageBox"
#include "process_handler/keyboard_callback.h"
#include <thread>
#include <QFileDialog>
#include <QTextStream>
#include "dialogs/clicker_data_dialog.h"
#include "config.hpp"
#include "discord_bot.h"
#include "dialogs/loading_dialog.h"
#include "dialogs/add_anomaly_dialog.h"
#include <QtConcurrent/QtConcurrent>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    initializeUI();
    connectSignals();
}

void MainWindow::initializeUI() {
    table_manager = std::make_unique<GUI::TableManager>(ui->tableWidget);
    anomaly_manager = std::make_unique<GUI::AnomalyManager>(ui->tableWidget_anomaly);
    table_manager->setupTable();
    anomaly_manager->setupTable();
    setWindowTitle(QString("%1 %2").arg(InjectionClicker::cmake::project_name.data(), InjectionClicker::cmake::project_version.data()));
    ui->label->setEnabled(true);
    ui->label->setOpenExternalLinks(true);
    loadSettings();
}

void MainWindow::loadSettings() {
    QSettings settings("NoName", "InjectionClicker");
    QString token = settings.value("bot_token", "").toString();
    QString chan_id = settings.value("channel_id", "").toString();
    ui->lineEdit_bot_token->setText(token);
    ui->lineEdit_channel_id->setText(chan_id);
}

void MainWindow::saveSettings() {
    QSettings settings("NoName", "InjectionClicker");
    settings.setValue("bot_token", ui->lineEdit_bot_token->text());
    settings.setValue("channel_id", ui->lineEdit_channel_id->text());
}


void MainWindow::connectSignals() {
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveRoutineData);
    connect(ui->actionLoad, &QAction::triggered, this, &MainWindow::loadRoutineData);
    connect(ui->lineEdit_bot_token, &QLineEdit::textChanged, this, &MainWindow::saveSettings);
    connect(ui->lineEdit_channel_id, &QLineEdit::textChanged, this, &MainWindow::saveSettings);

    qRegisterMetaType<QItemSelection>();
}

void MainWindow::createErrorBox(const std::string &errorMsg) {
    QMessageBox::warning(nullptr, "Error", errorMsg.c_str());
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_pushButton_Start_clicked() {
    if (!clicker) {
        createErrorBox("Please set up PID first");
        return;
    }

    if (isRecording) {
        createErrorBox("Please stop recording before starting clicker");
        return;
    }

    if(table_manager->isEmpty()){
        createErrorBox("Please add some keys to be clicked");
        return;
    }

    if (!clicker->getClickerStatus()) enableClicker();
    else disableClicker();
}

void MainWindow::enableClicker(){
    auto keyEvents = table_manager->extractAllData();
    clicker->setClickerStatus(true);
    ui->pushButton_Start->setText("Stop");
    clicker->addRoutine(keyEvents);
    clicker->startRoutines();
}

void MainWindow::disableClicker(){
    clicker->setClickerStatus(false);
    ui->pushButton_Start->setText("Start");
    createErrorBox("Waiting for clicker to finish task, This may take up to max declared ms");
    clicker->stopRoutines();
}



void MainWindow::on_pushButton_select_window_clicked() {
    ProcessHandler::callBackOnPIDExtracted([&](int pid, const std::string& title){
        try {
            clicker = std::make_unique<Clicker>(pid, title);
        }
        catch (const std::exception &e) {
            createErrorBox(std::string("PID with such name not found. Did you mean? ") + e.what());
            return;
        }
        ui->lineEdit_PID->setText(QString::number(pid));
        ui->lineEdit_tittle->setText(QString(title.data()));
        setPIDFoundSuccessful();
    });
}

void MainWindow::on_pushButton_record_clicked() {
    if (isRecording) disableKeyStrokeRecording();
    else enableKeyStrokeRecording();
}

void MainWindow::enableKeyStrokeRecording() {
    try {
        ProcessHandler::registerCallBack([&](const std::variant<ClickerData, Delay>& action) {
            table_manager->addRow(action);
        });
    }
    catch (const std::exception &e) {
        createErrorBox(e.what());
        return;
    }
    ui->pushButton_record->setStyleSheet("background-color: rgb(220, 20, 60); color: rgb(255, 255, 255)");
    ui->pushButton_record->setText("Stop recording");
    isRecording = !isRecording;
}

void MainWindow::disableKeyStrokeRecording() {
    ProcessHandler::removeCallBack();
    ui->pushButton_record->setStyleSheet("");
    ui->pushButton_record->setText("Record Key strokes");
    isRecording = !isRecording;
}

void MainWindow::on_pushButton_PID_clicked() {
    uint32_t process_id = ui->lineEdit_PID->text().toInt();
    std::string process_name = ui->lineEdit_tittle->text().toStdString();

    if (process_id == 0 || process_name.empty()) {
        createErrorBox("Incorrect PID, it can contain only numbers! Tittle cannot be empty");
        return;
    }
    try {
        clicker = std::make_unique<Clicker>(process_id, process_name);
    }
    catch (const std::exception &e) {
        createErrorBox(std::string("PID with such name not found. Did you mean? ") + e.what());
        return;
    }
    setPIDFoundSuccessful();
}


void MainWindow::setPIDFoundSuccessful() {
    ui->lineEdit_PID->setReadOnly(true);
    ui->lineEdit_tittle->setReadOnly(true);
    auto *palette = new QPalette();
    palette->setColor(QPalette::Base, Qt::gray);
    palette->setColor(QPalette::Text, Qt::black);
    ui->lineEdit_PID->setPalette(*palette);
    ui->lineEdit_tittle->setPalette(*palette);
    ui->pushButton_PID->setEnabled(false);
    ui->pushButton_PID->setAutoFillBackground(true);
    ui->pushButton_PID->setStyleSheet("background-color: rgb(50, 165, 89); color: rgb(255, 255, 255)");
    ui->pushButton_PID->setText("Success!");
}



void MainWindow::on_pushButton_delete_key_clicked() {
    table_manager->deleteSelectedRow();
}

void MainWindow::on_pushButton_insert_key_clicked() {
    GUI::Dialogs::ClickerDataDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted) return;
    ClickerData clicker_data = dialog.getClickerData();
    table_manager->addRow(clicker_data);
}

void MainWindow::saveRoutineData() {
    auto extracted_actions = table_manager->extractAllData();
    nlohmann::json jsonData;

    jsonData["version"] = InjectionClicker::cmake::project_version.data();

    for (auto &action: extracted_actions) {
        jsonData["routine"].push_back(std::visit([](auto &&arg) -> nlohmann::json {
            return arg;
        }, action));
    }

    const auto initialDir = QDir::currentPath() + "/Routines";
    QDir().mkpath(initialDir);
    QString defaultFileName = initialDir + "/Untitled.json";
    QString fileName = QFileDialog::getSaveFileName(this, "Save File", defaultFileName,
                                                    "JSON Files (*.json);;All Files (*)");

    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        createErrorBox("Could not open file for writing.");
        return;
    }

    try {
        QTextStream out(&file);
        out << QString::fromStdString(jsonData.dump(4));
        file.close();
    } catch (const std::exception& e) {
        createErrorBox(std::string("Failed to save data: ") + e.what());
        file.close();
        return;
    }

}

void MainWindow::loadRoutineData() {
    const QString initialDir = QDir::currentPath() + "/Routines";
    QDir().mkpath(initialDir);
    QString defaultFileName = initialDir + "/Untitled.json";
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", defaultFileName, "JSON Files (*.json);;All Files (*)");

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        createErrorBox("Could not open file for reading.");
        return;
    }

    QTextStream in(&file);
    std::string rawData = in.readAll().toStdString();
    file.close();

    nlohmann::json jsonData;
    try {
        jsonData = nlohmann::json::parse(rawData);
    } catch (const nlohmann::json::parse_error& e) {
        createErrorBox("Error while loading routine: JSON parsing error - " + std::string(e.what()));
        return;
    }

    if (!jsonData.contains("version") || jsonData["version"] != InjectionClicker::cmake::project_version.data()) {
        createErrorBox("Warning: JSON file version mismatch or missing version.");
    }

    if (!jsonData.contains("routine") || !jsonData["routine"].is_array()) {
        createErrorBox("Error in JSON structure: Missing or invalid 'routine' key.");
        return;
    }


    table_manager->setupTable();
    bool error = false;
    for (const auto& item : jsonData["routine"]) {
        if (!item.contains("type") || !item["type"].is_string()) {
            error = true;
            continue;
        }

        auto type = item["type"].get<std::string>();
        if (type == "ClickerData") {
            if (item.contains("key_name") && item.contains("key_code") && item.contains("event")) {
                table_manager->addRow(item.get<ClickerData>());
            } else {
                createErrorBox("Missing fields in ClickerData object.");
            }
        } else if (type == "Delay") {
            if (item.contains("delay")) {
                table_manager->addRow(item.get<Delay>());
            } else {
                createErrorBox("Missing 'delay' field in Delay object.");
            }
        } else {
            createErrorBox("Unknown type in JSON data: " + type);
        }
    }
    if(error)createErrorBox("Error in JSON data: Missing or invalid 'type' key.");
}

void MainWindow::setNotificationConnected() {
    ui->pushButton_Register_Bot->setEnabled(false);
    ui->pushButton_Register_Bot->setAutoFillBackground(true);
    ui->pushButton_Register_Bot->setStyleSheet("background-color: rgb(50, 165, 89); color: rgb(255, 255, 255)");
    ui->pushButton_Register_Bot->setText("Bot connected");
    ui->lineEdit_bot_token->setDisabled(true);
    ui->lineEdit_channel_id->setDisabled(true);
}

void MainWindow::on_pushButton_Register_Bot_clicked() {
    if (ui->lineEdit_channel_id->text().isEmpty()) {
        createErrorBox("Please add channel id");
        return;
    }

    if (bot) return;
    auto *loadingDialog = new GUI::Dialogs::LoadingDialog(this);
    loadingDialog->setAttribute(Qt::WA_DeleteOnClose);
    loadingDialog->show();

    QtConcurrent::run([this, loadingDialog]() {
        try {
            const auto token = ui->lineEdit_bot_token->text().toStdString();
            const auto chan_id = ui->lineEdit_channel_id->text().toStdString();

            bot = std::make_unique<Notification::DiscordBot>(token, [](const std::string &s) {
                std::cout << s << std::endl;
            });
            bot->run();
            bot->send_message(chan_id, "Successfully connected",
                              [this, loadingDialog](bool success, const std::string &err) {
                                  QMetaObject::invokeMethod(this, [this, success, err, loadingDialog]() {
                                      if (!success) {
                                          createErrorBox(err);
                                          bot = nullptr;
                                      } else {
                                          setNotificationConnected();
                                      }
                                      loadingDialog->accept();
                                      loadingDialog->deleteLater();

                                  });
                              });
        } catch (const std::exception &e) {
            QMetaObject::invokeMethod(this, "createErrorBoxQStr", Qt::QueuedConnection,
                                      Q_ARG(QString, QString::fromStdString(e.what())));
            bot = nullptr;
            loadingDialog->accept();
            loadingDialog->deleteLater();
        }
    });
}

void MainWindow::createErrorBoxQStr(const QString &errorMsg) {
    QMessageBox::warning(this, "Error", errorMsg);
}

void MainWindow::on_pushButton_add_anomaly_clicked() {
    GUI::Dialogs::AddAnomalyDialog dialog;
    if (dialog.exec() == QDialog::Accepted) {
        anomaly_manager->addRow(dialog.getImagePath().toStdString(),
               dialog.getMessage().toStdString(),
               dialog.getPercentage());
    }
}

void MainWindow::on_pushButton_remove_anomaly_clicked() {

}
