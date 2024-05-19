#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include "QMessageBox"
#include <thread>
#include <regex>


MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void createErrorBox(const std::string &errorMsg) {
    QMessageBox::warning(nullptr, "Error", errorMsg.c_str());
}

std::vector<ClickerData> MainWindow::retrieveClickToInvoke() {
    /// Retrieve check boxes from the GUI
    QList<QCheckBox *> l_checkboxes = this->findChildren<QCheckBox *>();
    /// Retrieve text boxes from GUI
    QList<QLineEdit *> txtBoxes = this->findChildren<QLineEdit *>();
    std::vector<ClickerData> keyEvents{};
    /// Configure out clicker
    uint8_t currentKeyCode = 0x70;
    for (auto &cBox: l_checkboxes) {
        if (cBox->isChecked()) {
            for (auto &tBox: txtBoxes) {
                /// If check box with text edit line align we && check box is enabled
                /// we create new data clicker obj
                if (cBox->y() == tBox->y()) {
                    std::string keyNumber = std::regex_replace(tBox->accessibleName().toStdString(),
                                                               std::regex(R"([\D])"), "");
                    keyEvents.emplace_back(currentKeyCode, tBox->text().toUInt(), std::stoi(keyNumber) > 8);
                    break;
                }
            }
        }
        currentKeyCode++;
    }
#ifdef DEBUG
    std::cout << "Number of keys to inject:  " << keyEvents.size() << std::endl;
#endif
    return keyEvents;
}

void MainWindow::on_pushButton_Start_clicked() {
    if (!clicker) {
        createErrorBox("Please set up PID first");
        return;
    }

    if (!clicker->getClickerStatus()) {
        std::vector<ClickerData> keyEvents = retrieveClickToInvoke();
#ifdef DEBUG
        for (auto &ev: keyEvents) {
            std::cout << ev.key_code << (ev.longClick ? " Long" : " Short") << std::endl;
        }
        std::cout << "----------" << std::endl;
#endif
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

void MainWindow::on_pushButton_PID_clicked() {
    ///Retrieve PID from the text box
    uint32_t dwProcessID = ui->lineEdit_PID->text().toInt();

    ///Retrieve name of PID
    std::string processName = ui->lineEdit_PID_2->text().toStdString();

    ///If PID or name of the process not provided, throw error msg
    if (dwProcessID == 0 || processName.empty()) {
        createErrorBox("Incorrect PID, it can contain only numbers! Tittle cannot be empty");
        return;
    }
    try {
        clicker = std::make_unique<Clicker>(dwProcessID, processName);
    }
    catch (const std::exception &e) {
        createErrorBox(std::string("PID with such name not found. Did you mean? ") + e.what());
        return;
    }
    ui->lineEdit_PID->setReadOnly(true);
    ui->lineEdit_PID_2->setReadOnly(true);
    auto *palette = new QPalette();
    palette->setColor(QPalette::Base, Qt::gray);
    palette->setColor(QPalette::Text, Qt::black);
    ui->lineEdit_PID->setPalette(*palette);
    ui->lineEdit_PID_2->setPalette(*palette);
    ui->pushButton_PID->setEnabled(false);
    ui->pushButton_PID->setAutoFillBackground(true);
    ui->pushButton_PID->setStyleSheet("background-color: rgb(50, 165, 89); color: rgb(255, 255, 255)");
    ui->pushButton_PID->setText("Success!");
}
