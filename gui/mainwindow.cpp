#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include "QMessageBox"
#include "process_handler/keyboard_callback.h"
#include <thread>
#include <regex>
#include <QStyledItemDelegate>
#include <QFileDialog>
#include <QTextStream>
#include <QBrush>

#include "delegates/non_editable_delegate.hpp"
#include "delegates/numeric_delegate.hpp"
#include "delegates/action_delegate.hpp"
#include "dialogs/clicker_data_dialog.h"
#include "config.hpp"

void setupTable(QTableWidget* table) {
    table->setColumnCount(4);
    QStringList headers = {"Name", "Key", "Delay ms", "Action"};
    table->setHorizontalHeaderLabels(headers);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    auto* nonEditableDelegate = new GUI::Delegates::NonEditableDelegate(table);
    table->setItemDelegateForColumn(0, nonEditableDelegate);
    table->setItemDelegateForColumn(1, nonEditableDelegate);

    auto* numericDelegate = new GUI::Delegates::NumericDelegate(table);
    table->setItemDelegateForColumn(2, numericDelegate);

    auto* actionDelegate = new GUI::Delegates::ActionDelegate(table);
    table->setItemDelegateForColumn(3, actionDelegate);
    table->verticalHeader()->hide();
    QObject::connect(table, &QTableWidget::itemChanged, [table](QTableWidgetItem* item) {
        int row = item->row();
        int column = item->column();

        QTableWidgetItem* firstColumnItem = table->item(row, 0);
        if (!firstColumnItem) return;

        QVariant userData = firstColumnItem->data(Qt::UserRole);

        if (userData.canConvert<std::shared_ptr<ClickerData>>()) {
            auto clickerData = qvariant_cast<std::shared_ptr<ClickerData>>(userData);
            if (column == 3) {
                clickerData->event = (clickerData->event == ClickerData::Event::Pressed) ?
                                     ClickerData::Event::Released : ClickerData::Event::Pressed;
            }
        } else if (userData.canConvert<std::shared_ptr<Delay>>()) {
            auto delayData = qvariant_cast<std::shared_ptr<Delay>>(userData);
            if (column == 2) {
                delayData->delay = item->text().toInt();
            }
        }
    });
}



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    QString title = QString("%1 %2").arg(InjectionClicker::cmake::project_name.data(),
                                         InjectionClicker::cmake::project_version.data());
    setWindowTitle(title);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setupTable(ui->tableWidget);
    ui->label->setOpenExternalLinks(true);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveRoutineData);
    connect(ui->actionLoad, &QAction::triggered, this, &MainWindow::loadRoutineData);
    qRegisterMetaType<std::shared_ptr<ClickerData>>("std::shared_ptr<ClickerData>");
    qRegisterMetaType<std::shared_ptr<Delay>>("std::shared_ptr<Delay>");
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

    if(ui->tableWidget->rowCount() == 0){
        createErrorBox("Please add some keys to be clicked");
        return;
    }

    if (!clicker->getClickerStatus()) enableClicker();
    else disableClicker();
}

void MainWindow::enableClicker(){
    auto keyEvents = extractAllDataFromTable();
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

std::vector<std::variant<ClickerData, Delay>> MainWindow::extractAllDataFromTable() {
    std::vector<std::variant<ClickerData, Delay>> allData;
    int rowCount = ui->tableWidget->rowCount();

    for (int i = 0; i < rowCount; ++i) {
        QVariant userData = ui->tableWidget->item(i, 0)->data(Qt::UserRole);
        if (userData.canConvert<std::shared_ptr<ClickerData>>()) {
            auto clickerData = qvariant_cast<std::shared_ptr<ClickerData>>(userData);
            allData.emplace_back(*clickerData);
        } else if (userData.canConvert<std::shared_ptr<Delay>>()) {
            auto delayData = qvariant_cast<std::shared_ptr<Delay>>(userData);
            allData.emplace_back(*delayData);
        }
    }

    return allData;
}

struct PtrCreatorVisitor {
    QVariant operator()(const ClickerData& data) const {
        return QVariant::fromValue(std::make_shared<ClickerData>(data));
    }

    QVariant operator()(const Delay& data) const {
        return QVariant::fromValue(std::make_shared<Delay>(data));
    }
};



void MainWindow::addRowToTable(const std::variant<ClickerData, Delay>& data) {
    int row = ui->tableWidget->currentRow() + 1;
    if (row == 0) {
        row = ui->tableWidget->rowCount();
    }

    ui->tableWidget->insertRow(row);

    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, ClickerData>) {
            // Handling ClickerData
            auto *name = new QTableWidgetItem(QString::fromStdString(arg.key_name));
            auto *keyItem = new QTableWidgetItem(QString::number(arg.key_code));
            auto *action = new QTableWidgetItem(arg.event == ClickerData::Event::Pressed ? "Pressed" : "Released");
            auto *placeholder = new QTableWidgetItem();  // Placeholder for the fourth column

            // Set items to the table
            ui->tableWidget->setItem(row, 0, name);
            ui->tableWidget->setItem(row, 1, keyItem);
            ui->tableWidget->setItem(row, 2, placeholder);
            ui->tableWidget->setItem(row, 3, action);

            // Gray out the unused fourth column
            placeholder->setFlags(placeholder->flags() ^ Qt::ItemIsEditable);
            placeholder->setBackground(QBrush(Qt::darkGray));
        } else if constexpr (std::is_same_v<T, Delay>) {
            // Handling Delay
            auto *name = new QTableWidgetItem("Delay");
            auto *delayTime = new QTableWidgetItem(QString::number(arg.delay));
            auto *placeholder1 = new QTableWidgetItem();
            auto *placeholder2 = new QTableWidgetItem();

            ui->tableWidget->setItem(row, 0, name);
            ui->tableWidget->setItem(row, 1, placeholder1);
            ui->tableWidget->setItem(row, 2, delayTime);
            ui->tableWidget->setItem(row, 3, placeholder2);

            placeholder1->setFlags(placeholder1->flags() ^ Qt::ItemIsEditable);
            placeholder1->setBackground(QBrush(Qt::darkGray));
            placeholder2->setFlags(placeholder2->flags() ^ Qt::ItemIsEditable);
            placeholder2->setBackground(QBrush(Qt::darkGray));
        }
    }, data);

    ui->tableWidget->item(row, 0)->setData(Qt::UserRole, std::visit(PtrCreatorVisitor{}, data));
    ui->tableWidget->setCurrentCell(row, 0);
    ui->tableWidget->scrollToItem(ui->tableWidget->item(row, 0));
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
            addRowToTable(data);
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
    int selectedRow = ui->tableWidget->selectionModel()->currentIndex().row();
    if (selectedRow < 0) {
        createErrorBox(std::string("First select row to be removed"));
        return;
    }
    ui->tableWidget->removeRow(selectedRow);
}

void MainWindow::on_pushButton_insert_key_clicked() {
    GUI::Dialogs::ClickerDataDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        ClickerData data = dialog.getClickerData();
        addRowToTable(data);
    }
}

void MainWindow::saveRoutineData() {
    auto allData = extractAllDataFromTable();
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

    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    setupTable(ui->tableWidget);
    bool error = false;
    for (const auto& item : jsonData["routine"]) {
        if (!item.contains("type") || !item["type"].is_string()) {
            error = true;
            continue;
        }

        auto type = item["type"].get<std::string>();
        if (type == "ClickerData") {
            if (item.contains("key_name") && item.contains("key_code") && item.contains("event")) {
                addRowToTable(item.get<ClickerData>());
            } else {
                createErrorBox("Missing fields in ClickerData object.");
            }
        } else if (type == "Delay") {
            if (item.contains("delay")) {
                addRowToTable(item.get<Delay>());
            } else {
                createErrorBox("Missing 'delay' field in Delay object.");
            }
        } else {
            createErrorBox("Unknown type in JSON data: " + type);
        }
    }
    if(error)createErrorBox("Error in JSON data: Missing or invalid 'type' key.");
}
