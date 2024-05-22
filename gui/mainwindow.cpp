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

#include "delegates/non_editable_delegate.hpp"
#include "delegates/numeric_delegate.hpp"
#include "delegates/action_delegate.hpp"

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
}



MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setupTable(ui->tableWidget);
    ui->label->setOpenExternalLinks(true);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveRoutineData);
    connect(ui->actionLoad, &QAction::triggered, this, &MainWindow::loadRoutineData);
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

    if (!clicker->getClickerStatus()) {
        std::vector<ClickerData> keyEvents = extractAllDataFromTable();
        clicker->setClickerStatus(true);
        ui->pushButton_Start->setText("Stop");
        clicker->initClickerThreads(keyEvents);
    } else {
        clicker->setClickerStatus(false);
        ui->pushButton_Start->setText("Start");
        QMessageBox::warning(nullptr, "Warning",
                             "Waiting for clicker to finish task, This may take up to max declared ms");
        clicker->destroyClickerThreads();
    }
}

std::vector<ClickerData> MainWindow::extractAllDataFromTable() {
    std::vector<ClickerData> allData;
    int rowCount = ui->tableWidget->rowCount();

    for (int i = 0; i < rowCount; ++i) {
        ClickerData data;
        QTableWidgetItem* nameItem = ui->tableWidget->item(i, 0);
        QTableWidgetItem* keyItem = ui->tableWidget->item(i, 1);
        QTableWidgetItem* delayItem = ui->tableWidget->item(i, 2);
        QTableWidgetItem* actionItem = ui->tableWidget->item(i, 3);

        if (nameItem) data.key_name = nameItem->text().toStdString();
        if (keyItem) data.key_code = keyItem->text().toInt();
        if (delayItem) data.delay = delayItem->text().toInt();
        if (actionItem) {
            std::string action = actionItem->text().toStdString();
            data.event = (action == "Pressed") ? ClickerData::Event::Pressed : ClickerData::Event::Released;
        }
        allData.push_back(data);
    }

    return allData;
}

void MainWindow::addRowToTable(const ClickerData& data) {
    int row = ui->tableWidget->currentRow() + 1;
    if (row == 0) {
        row = ui->tableWidget->rowCount();
    }

    ui->tableWidget->insertRow(row);

    auto *name = new QTableWidgetItem(QString(data.key_name.data()));
    ui->tableWidget->setItem(row, 0, name);

    auto *keyItem = new QTableWidgetItem(QString::number(data.key_code));
    ui->tableWidget->setItem(row, 1, keyItem);

    auto *delayItem = new QTableWidgetItem(QString::number(data.delay));
    ui->tableWidget->setItem(row, 2, delayItem);

    auto *action = new QTableWidgetItem(data.event == ClickerData::Event::Pressed ? "Pressed" : "Released");
    ui->tableWidget->setItem(row, 3, action);
}


void MainWindow::on_pushButton_select_window_clicked() {
    extractAllDataFromTable();
    std::cout << "on_select_PID_clicked select" << std::endl;
}

void MainWindow::on_pushButton_record_clicked() {
    if (isRecording) {
        ProcessHandler::removeCallBack();
        ui->pushButton_record->setStyleSheet("");
        ui->pushButton_record->setText("Record Key strokes");
        std::cout << "on_pushButton_record_clicked unselect" << std::endl;
    } else {
        try {
            ProcessHandler::registerCallBack([&](const ClickerData& data){
                addRowToTable(data);
            });
        }
        catch (const std::exception &e) {
            createErrorBox(e.what());
            return;
        }
        ui->pushButton_record->setStyleSheet("background-color: rgb(220, 20, 60); color: rgb(255, 255, 255)");
        ui->pushButton_record->setText("Stop recording");
        std::cout << "on_pushButton_record_clicked select" << std::endl;
    }
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
    if (selectedRow >= 0) {
        ui->tableWidget->removeRow(selectedRow);
    }else{
        createErrorBox(std::string("First select row to be removed"));
    }
}

void MainWindow::on_pushButton_insert_key_clicked() {
    ClickerDataDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        ClickerData data = dialog.getClickerData();
        addRowToTable(data);
    }
}

void MainWindow::saveRoutineData() {
    nlohmann::json jsonData = extractAllDataFromTable();
    const auto initialDir = QDir::currentPath() + "/Routines";
    QDir().mkpath(initialDir);
    QString defaultFileName = initialDir + "/Untitled.json";
    QString fileName = QFileDialog::getSaveFileName(
            this,
            "Save File",
            defaultFileName,
            "JSON Files (*.json);;All Files (*)"
    );

    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not open file for writing.");
        return;
    }

    QTextStream out(&file);
    out << QString(jsonData.dump(4).data());
    file.close();
}

void MainWindow::loadRoutineData() {
    const QString initialDir = QDir::currentPath() + "/Routines";
    QDir().mkpath(initialDir);
    QString defaultFileName = initialDir + "/Untitled.json";
    QString fileName = QFileDialog::getOpenFileName(
            this,
            "Open File",
            defaultFileName,
            "JSON Files (*.json);;All Files (*)"
    );
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not open file for reading.");
        return;
    }

    QTextStream in(&file);
    std::string rawData = in.readAll().toStdString();
    file.close();

    nlohmann::json jsonData;
    try{
        jsonData = nlohmann::json::parse(rawData, nullptr, true, true);
    }catch (const nlohmann::json::parse_error &e){
        createErrorBox(std::string("Error while loading routine: ") + e.what());
        return;
    }
    const auto dataVec = jsonData.get<std::vector<ClickerData>>();

    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);

    for(const auto& data : dataVec) {
        addRowToTable(data);
    }
}
