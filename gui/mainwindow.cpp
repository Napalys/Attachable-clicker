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


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    tableManager = new GUI::TableManager(ui->tableWidget);
    tableManager->setupTable();
    QString title = QString("%1 %2").arg(InjectionClicker::cmake::project_name.data(),InjectionClicker::cmake::project_version.data());
    setWindowTitle(title);
    ui->label->setOpenExternalLinks(true);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveRoutineData);
    connect(ui->actionLoad, &QAction::triggered, this, &MainWindow::loadRoutineData);
    qRegisterMetaType<QItemSelection>();
}

MainWindow::~MainWindow() {
    delete ui;
}

void createErrorBox(const std::string &errorMsg) {
    QMessageBox::warning(nullptr, "Error", errorMsg.c_str());
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

    if(tableManager->isEmpty()){
        createErrorBox("Please add some keys to be clicked");
        return;
    }

    if (!clicker->getClickerStatus()) enableClicker();
    else disableClicker();
}

void MainWindow::enableClicker(){
    auto keyEvents = tableManager->extractAllData();
    clicker->setClickerStatus(true);
    ui->pushButton_Start->setText("Stop");
    clicker->addRoutine(keyEvents);
    clicker->startRoutines();
}

void MainWindow::disableClicker(){
    clicker->setClickerStatus(false);
    ui->pushButton_Start->setText("Start");
    QMessageBox::warning(nullptr, "Warning",
                         "Waiting for clicker to finish task, This may take up to max declared ms");
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
        ProcessHandler::registerCallBack([&](const std::variant<ClickerData, Delay>& data) {
            tableManager->addRow(data);
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
    tableManager->deleteSelectedRow();
}

void MainWindow::on_pushButton_insert_key_clicked() {
    GUI::Dialogs::ClickerDataDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted) return;
    ClickerData data = dialog.getClickerData();
    tableManager->addRow(data);
}

void MainWindow::saveRoutineData() {
    auto allData = tableManager->extractAllData();
    nlohmann::json jsonData;

    jsonData["version"] = InjectionClicker::cmake::project_version.data();

    for (auto &data: allData) {
        jsonData["routine"].push_back(std::visit([](auto &&arg) -> nlohmann::json {
            return arg;
        }, data));
    }

    const auto initialDir = QDir::currentPath() + "/Routines";
    QDir().mkpath(initialDir);
    QString defaultFileName = initialDir + "/Untitled.json";
    QString fileName = QFileDialog::getSaveFileName(this, "Save File", defaultFileName,
                                                    "JSON Files (*.json);;All Files (*)");

    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not open file for writing.");
        return;
    }

    try {
        QTextStream out(&file);
        out << QString::fromStdString(jsonData.dump(4));
        file.close();
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString("Failed to save data: %1").arg(e.what()));
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
        QMessageBox::warning(this, "Error", "Could not open file for reading.");
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


    tableManager->setupTable();
    bool error = false;
    for (const auto& item : jsonData["routine"]) {
        if (!item.contains("type") || !item["type"].is_string()) {
            error = true;
            continue;
        }

        auto type = item["type"].get<std::string>();
        if (type == "ClickerData") {
            if (item.contains("key_name") && item.contains("key_code") && item.contains("event")) {
                tableManager->addRow(item.get<ClickerData>());
            } else {
                createErrorBox("Missing fields in ClickerData object.");
            }
        } else if (type == "Delay") {
            if (item.contains("delay")) {
                tableManager->addRow(item.get<Delay>());
            } else {
                createErrorBox("Missing 'delay' field in Delay object.");
            }
        } else {
            createErrorBox("Unknown type in JSON data: " + type);
        }
    }
    if(error)createErrorBox("Error in JSON data: Missing or invalid 'type' key.");
}
