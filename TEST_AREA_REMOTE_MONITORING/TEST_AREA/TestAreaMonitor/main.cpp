#include "mainwindow.h"
#include <QApplication>
#include <QTcpServer>
#include <QTcpSocket>

#include "TcpServer/myserver.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MyServer mServer;
    MainWindow w;
    w.show();

    return a.exec();
}
