#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <windows.h>
#include <iostream>
#include "QMessageBox"
#include <thread>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    clicker = Clicker();
    clicker.setClickerStatus(false);
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void createErrorBox(const std::string& errorMsg){
    QMessageBox::warning(0,"Error",errorMsg.c_str());
}

std::vector<clickerData> MainWindow::retreiveClickToInvoke(){
    // Retreive check boxes from the GUI
    QList<QCheckBox *> l_checkboxes = this->findChildren<QCheckBox *>();
    // Retreive text boxes from GUI
    QList<QLineEdit *> txtBoxes = this->findChildren<QLineEdit *>();
    std::vector<clickerData> keyEvents{};
    //Configure out clicker
    uint8_t currentKeyCode = 0x70;
    for(auto& cBox : l_checkboxes){
        if (cBox->isChecked()){
            for(auto& tBox : txtBoxes){
                // If check box with text edit line allign we && check box is enabled
                // we create new data clicker obj
                if( cBox->y() == tBox->y()){
                    keyEvents.emplace_back(currentKeyCode, tBox->text().toUInt());
                    break;
                }
            }
        }
        currentKeyCode++;
    }
    return keyEvents;
}

void MainWindow::on_pushButton_Start_clicked()
{
    if(clicker.getHWND() == nullptr){
        createErrorBox("Please set up PID first");
        return;
    }

    if(!clicker.getClickerStatus()){
        std::vector<clickerData> keyEvents = retreiveClickToInvoke();
        for(auto& ev : keyEvents){
            std::cout << ev.key_code <<std::endl;
        }
        std::cout << "----------" <<std::endl;
        clicker.setClickerStatus(true);
        ui->pushButton_Start->setText("Stop");
        clicker.initClickerThreads(keyEvents);
    }
    else{
        clicker.setClickerStatus(false);
        ui->pushButton_Start->setText("Start");
        QMessageBox warning;
        warning.warning(0,"Warning","Waiting for clicker to finish task, This may take up to max declared ms");
        clicker.destroyClickerThreads();
    }
}

HWND MainWindow::receiveHWND(const DWORD& dwProcessID, const std::string& processName){
    std::vector <HWND> vhWnds;
    HWND targetWindow = nullptr;
    // find all hWnds (vhWnds) associated with a process id (dwProcessID)
       HWND hCurWnd = nullptr;
       do
       {
           hCurWnd = FindWindowEx(nullptr, hCurWnd, nullptr, nullptr);
           DWORD checkProcessID = 0;
           GetWindowThreadProcessId(hCurWnd, &checkProcessID);
           if (checkProcessID == dwProcessID)
           {
               vhWnds.emplace_back(hCurWnd);  // add the found hCurWnd to the vector

               std::wstring title(GetWindowTextLength(hCurWnd) + 1, L'\0');
               GetWindowTextW(hCurWnd, &title[0], title.size());
               std::string str( title.begin(), title.end() );
               if(str.find(processName) != std::string::npos)
                   targetWindow = hCurWnd;

           }
       }
       while (hCurWnd != nullptr);

       //Throw error is no processes found with such PID
       if(vhWnds.empty()){
            createErrorBox("Wrong PID!");
            return nullptr;
       }
        std::string errorMessage = "";
       //Throw error if proccess with such PID and name not found
       if(targetWindow == nullptr){

           for(auto& hwnd : vhWnds){
               std::wstring title(GetWindowTextLength(hwnd) + 1, L'\0');
               GetWindowTextW(hwnd, &title[0], title.size());
               std::cout << title.size() <<std::endl;
                std::string str( title.begin(), title.end() );
                //remove string terminator, thus after converting to char we do not lose data
                str.erase(std::find(str.begin(), str.end(), '\0'), str.end());
                if(title.size() > 1)
                    errorMessage = errorMessage + '\n' + str;
           }
           std::cout << errorMessage <<std::endl;
           createErrorBox("PID with such name not found. Did you mean? " + errorMessage);
       }
       else{
           return targetWindow;

       }
       return nullptr;
}


void MainWindow::on_pushButton_PID_clicked()
{
    //Retreive PID from the text box
    DWORD dwProcessID = ui->lineEdit_PID->text().toInt();

    //Retreive name of PID
    std::string processName = ui->lineEdit_PID_2->text().toStdString();

    //If PID or name of the process not provided, throw error msg
    if(dwProcessID == 0 || processName.empty()){
        createErrorBox("Incorrect PID, it can contain only numbers! Tittle cannot be empty");
        return;
    }
    HWND hwnd = receiveHWND(dwProcessID, processName);
    std::cout << hwnd <<std::endl;
    if(hwnd != nullptr){
        clicker.setHWND(hwnd);
        ui->lineEdit_PID->setReadOnly(true);
         ui->lineEdit_PID_2->setReadOnly(true);
        QPalette *palette = new QPalette();
        palette->setColor(QPalette::Base,Qt::gray);
        palette->setColor(QPalette::Text,Qt::black);
        ui->lineEdit_PID->setPalette(*palette);
         ui->lineEdit_PID_2->setPalette(*palette);
         ui->pushButton_PID->setEnabled(false);
         ui->pushButton_PID->setAutoFillBackground(true);
         ui->pushButton_PID->setStyleSheet("background-color: rgb(50, 165, 89); color: rgb(255, 255, 255)");
         ui->pushButton_PID->setText("Success!");
    }
}
