#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "clicker_data.h"
#include "clicker.h"
#include "managers/table_manager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#include <QSpinBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QFileDialog>


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

private:
    void saveRoutineData();
    void loadRoutineData();
    void setPIDFoundSuccessful();
    void enableKeyStrokeRecording();
    void disableKeyStrokeRecording();
    void enableClicker();
    void disableClicker();
    void initializeUI();
    void connectSignals();
    static void createErrorBox(const std::string &errorMsg);

private:
    Ui::MainWindow *ui;
    std::unique_ptr<GUI::TableManager> tableManager;
    std::unique_ptr<Clicker> clicker = nullptr;
    bool isRecording = false;
};

#endif // MAINWINDOW_H
