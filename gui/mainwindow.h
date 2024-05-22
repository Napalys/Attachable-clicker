#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "clicker_data.h"
#include "clicker.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#include <QSpinBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QFileDialog>


class ClickerDataDialog : public QDialog {
Q_OBJECT

public:
    ClickerDataDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setupUi();
        connectSignals();
    }

    [[nodiscard]] ClickerData getClickerData() const {
        return {static_cast<uint8_t>(keyCodeEdit->text().toInt()),
                static_cast<uint32_t>(delayEdit->value()),
                static_cast<ClickerData::Event>(eventComboBox->currentIndex()),
                keyNameEdit->text().toStdString()};
    }

private:
    QLineEdit *keyNameEdit{};
    QSpinBox *keyCodeEdit{};
    QSpinBox *delayEdit{};
    QComboBox *eventComboBox{};
    QDialogButtonBox *buttonBox{};

    void setupUi() {
        keyCodeEdit = new QSpinBox(this);
        keyCodeEdit->setRange(0, 255);

        delayEdit = new QSpinBox(this);
        delayEdit->setRange(0, 100000);

        keyNameEdit = new QLineEdit(this);

        eventComboBox = new QComboBox(this);
        eventComboBox->addItem("Pressed", QVariant(static_cast<int>(ClickerData::Event::Pressed)));
        eventComboBox->addItem("Released", QVariant(static_cast<int>(ClickerData::Event::Released)));
        eventComboBox->addItem("Unknown", QVariant(static_cast<int>(ClickerData::Event::Unknown)));

        buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

        auto *layout = new QFormLayout(this);
        layout->addRow("Key Code:", keyCodeEdit);
        layout->addRow("Delay (ms):", delayEdit);
        layout->addRow("Event:", eventComboBox);
        layout->addRow("Key Name:", keyNameEdit);
        layout->addWidget(buttonBox);

        setLayout(layout);
        setWindowTitle("Add Clicker Data");
    }

    void connectSignals() {
        connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    }
};

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

private slots:

    void on_pushButton_Start_clicked();
    void on_pushButton_PID_clicked();
    void on_pushButton_delete_key_clicked();
    void on_pushButton_insert_key_clicked();
    void on_pushButton_record_clicked();
    void on_pushButton_select_window_clicked();

    void saveRoutineData();
    void loadRoutineData();
    void addRowToTable(const ClickerData& data);
    std::vector<ClickerData> extractAllDataFromTable();

private:
    Ui::MainWindow *ui;
    std::unique_ptr<Clicker> clicker = nullptr;
    bool isRecording = false;
};

#endif // MAINWINDOW_H
