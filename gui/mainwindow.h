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

Q_DECLARE_METATYPE(std::shared_ptr<ClickerData>)
Q_DECLARE_METATYPE(std::shared_ptr<Delay>)

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
    void setPIDFoundSuccessful();
    void enableKeyStrokeRecording();
    void disableKeyStrokeRecording();
    void enableClicker();
    void disableClicker();
    void addRowToTable(const std::variant<ClickerData, Delay>& data);
    std::vector<std::variant<ClickerData, Delay>> extractAllDataFromTable();

private:
    Ui::MainWindow *ui;
    std::unique_ptr<Clicker> clicker = nullptr;
    bool isRecording = false;
};

#endif // MAINWINDOW_H
