#ifndef MYSERVER_H
#define MYSERVER_H

#include<QObject>
#include<QDebug>
#include<QTcpServer>
#include<QTcpSocket>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#define def20second     ( 20000 / 50 )      //her adim 50 ms
#define def5second      ( 5000 / 50 )       //her adim 50 ms

class MyServer : public QObject
{
    Q_OBJECT
public:
    explicit MyServer(QObject *parent = nullptr);

signals:
    void dataReceived(QByteArray);

public slots:
    void newConnection();
    void disconnected();
    void readyRead();
private:
    QTcpServer *server;
    QHash<QTcpSocket*, QByteArray*> buffers;        //We need a buffer to store data until block has completely received
    QHash<QTcpSocket*, qint32*> sizes;              //We need to store the size to verify if a block has received completely
};

#endif // MYSERVER_H
