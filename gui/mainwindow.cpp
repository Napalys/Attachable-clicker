#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include "QMessageBox"
#include "process_handler/keyboard_callback.h"
#include <thread>
#include <regex>
#include <QItemDelegate>
#include <QStyledItemDelegate>
#include <QComboBox>

class NumericDelegate : public QItemDelegate {
public:
    explicit NumericDelegate(QObject *parent = nullptr) : QItemDelegate(parent) {}

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem&,
                          const QModelIndex&) const override {
        auto* editor = new QLineEdit(parent);
        editor->setValidator(new QIntValidator(0, 100000, editor));
        return editor;
    }
};

class NonEditableDelegate : public QItemDelegate {
public:
    explicit NonEditableDelegate(QObject *parent = nullptr) : QItemDelegate(parent) {}

    QWidget* createEditor(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const override {
        return nullptr;
    }
};

class ActionDelegate : public QStyledItemDelegate {
public:
    explicit ActionDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override {
        Q_UNUSED(option);
        Q_UNUSED(index);
        auto *editor = new QComboBox(parent);
        editor->addItem("Pressed");
        editor->addItem("Released");
        return editor;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const override {
        QString value = index.model()->data(index, Qt::EditRole).toString();
        auto *comboBox = dynamic_cast<QComboBox*>(editor);
        comboBox->setCurrentIndex(comboBox->findText(value));
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override {
        auto *comboBox = dynamic_cast<QComboBox*>(editor);
        model->setData(index, comboBox->currentText(), Qt::EditRole);
    }

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        Q_UNUSED(index);
        editor->setGeometry(option.rect);
    }
};

void setupTable(QTableWidget* table) {
    table->setColumnCount(4);
    QStringList headers = {"Name", "Key", "Delay ms", "Action"};
    table->setHorizontalHeaderLabels(headers);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    auto* nonEditableDelegate = new NonEditableDelegate(table);
    table->setItemDelegateForColumn(0, nonEditableDelegate);
    table->setItemDelegateForColumn(1, nonEditableDelegate);

    auto* numericDelegate = new NumericDelegate(table);
    table->setItemDelegateForColumn(2, numericDelegate);

    auto* actionDelegate = new ActionDelegate(table);
    table->setItemDelegateForColumn(3, actionDelegate);
    table->verticalHeader()->hide();
}



MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setupTable(ui->tableWidget);
     ui->label->setOpenExternalLinks(true);
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
                    keyEvents.emplace_back(currentKeyCode, tBox->text().toUInt(), ClickerData::Event::Pressed, "");
                    keyEvents.emplace_back(currentKeyCode, 100, ClickerData::Event::Released, " ");
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
        std::vector<ClickerData> keyEvents = extractAllDataFromTable();
#ifdef DEBUG
        for (auto &ev: keyEvents) {
            std::cout << ev.key_code << "Short" << std::endl;
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


void MainWindow::on_select_PID_clicked() {
    extractAllDataFromTable();
    std::cout << "on_select_PID_clicked select" << std::endl;
}

void MainWindow::on_button_record_clicked() {
    if (isRecording) {
        ProcessHandler::removeCallBack();
        ui->button_record->setStyleSheet("");
        ui->button_record->setText("Record Key strokes");
        std::cout << "on_button_record_clicked unselect" << std::endl;
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
        ui->button_record->setStyleSheet("background-color: rgb(220, 20, 60); color: rgb(255, 255, 255)");
        ui->button_record->setText("Stop recording");
        std::cout << "on_button_record_clicked select" << std::endl;
    }
    isRecording = !isRecording;
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

void MainWindow::on_pushButton_delete_key_clicked() {
    int selectedRow = ui->tableWidget->selectionModel()->currentIndex().row();
    if (selectedRow >= 0) {
        ui->tableWidget->removeRow(selectedRow);
    }else{
        createErrorBox(std::string("First select row to be removed"));

    }
}

void MainWindow::on_pushButton_insert_key_clicked() {
    addRowToTable(ClickerData{});
}
