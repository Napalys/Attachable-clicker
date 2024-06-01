#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "clicker_data.h"
#include "clicker.h"
#include "managers/table_manager.h"
#include "discord_bot.h"
#include "managers/anomaly_manager.h"

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
    void on_pushButton_Register_Bot_clicked();
    void on_pushButton_add_anomaly_clicked();
    void on_pushButton_remove_anomaly_clicked();
    void createErrorBoxQStr(const QString &errorMsg);

private:
    void saveRoutineData();
    void loadRoutineData();
    void setPIDFoundSuccessful();
    void setNotificationConnected();
    void setNotificationDisconnected();
    void enableKeyStrokeRecording();
    void disableKeyStrokeRecording();
    void enableClicker();
    void disableClicker();
    void initializeUI();
    void connectSignals();
    void loadSettings();
    void saveSettings();
    static void createErrorBox(const std::string &errorMsg);

private:
    Ui::MainWindow *ui;
    std::unique_ptr<GUI::TableManager> table_manager;
    std::unique_ptr<GUI::AnomalyManager> anomaly_manager;
    std::unique_ptr<Clicker> clicker = nullptr;
    std::unique_ptr<Notification::DiscordBot> bot = nullptr;
    bool isRecording = false;
};

#endif // MAINWINDOW_H
