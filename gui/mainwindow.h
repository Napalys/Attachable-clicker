#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "clicker_data.h"
#include "clicker.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

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
    void on_select_PID_clicked();
    void on_button_record_clicked();
    void addRowToTable(const ClickerData& data);
    std::vector<ClickerData> extractAllDataFromTable();

    std::vector<ClickerData> retrieveClickToInvoke();

private:
    Ui::MainWindow *ui;
    std::unique_ptr<Clicker> clicker = nullptr;
    bool isRecording = false;
};

#endif // MAINWINDOW_H
