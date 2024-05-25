#include "gui/mainwindow.h"
#include <QApplication>

#ifdef Q_OS_WIN
#include "windows.h"
#endif

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

#ifdef Q_OS_WIN
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
#endif

    return QApplication::exec();
}