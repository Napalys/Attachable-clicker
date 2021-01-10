#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../clicker/clickerData.h"
#include "../clicker/clicker.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_pushButton_Start_clicked();

    void on_pushButton_PID_clicked();

    std::vector<clickerData> retreiveClickToInvoke();

    HWND receiveHWND(const DWORD& dwProcessID, const std::string& processName);

private:
    Ui::MainWindow *ui;
    Clicker clicker;
};
#endif // MAINWINDOW_H
