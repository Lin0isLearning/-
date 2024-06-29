#include <QApplication>
#include "mainwindow.h"
#include "client.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // 如果你有多个窗口并且只是用来测试，可以展示所有窗口
    MainWindow w;
    w.show();

    Client c;
    c.show();

    return a.exec();
}
