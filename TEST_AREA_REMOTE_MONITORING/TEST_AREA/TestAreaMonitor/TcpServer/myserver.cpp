#include "myserver.h"
#include "QDebug"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "variable.h"

#include <QTime>

MyServer::MyServer(QObject *parent) : QObject(parent)
{
    server = new QTcpServer(this);

    connect(server,SIGNAL(newConnection()),this,SLOT(newConnection()));

    if(!server->listen(QHostAddress::Any,10009))
    {
        qDebug() << "Server could not start!";
    }
    else
    {
        qDebug() << "Server started!";
    }
}


void MyServer::newConnection()
{
    while (server->hasPendingConnections())
    {
        QTcpSocket *socket = server->nextPendingConnection();
        connect(socket, SIGNAL(readyRead()), SLOT(readyRead()));
        connect(socket, SIGNAL(disconnected()), SLOT(disconnected()));
        QByteArray *buffer = new QByteArray();
        qint32 *s = new qint32(0);
        buffers.insert(socket, buffer);
        sizes.insert(socket, s);

        ConnectedCounter++;

        if( ConnectedCounter > defOneConnection )
        {
            CommunicationStatus = defMultiConnected;
        }
        else if( ConnectedCounter == defOneConnection )
        {
            CommunicationStatus = defConnected;
            DataReceiveTimer = def5second;
        }
        if( CommunicationStatus == defMultiConnected )
        {
            ConnectionTimer = def5second;
            DataReceiveTimer = 0;
        }
        oldCommunicationStatus = CommunicationStatus;

    }
}


void MyServer::disconnected()
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    QByteArray *buffer = buffers.value(socket);
    qint32 *s = sizes.value(socket);
    socket->deleteLater();
    delete buffer;
    delete s;

    ConnectedCounter--;

    if( ConnectedCounter == defNoConnection )
    {
        ConnectionTimer = def20second;                  //Hemen disconnection yazmasin ekrana, belki baglanti kopup yeniden gerceklesir.
        CommunicationStatus = defDisconnected;
        DataReceiveTimer = 0;
    }
    else if( ConnectedCounter == defOneConnection )
    {
        CommunicationStatus = defConnected;
        DataReceiveTimer = def5second;
    }
    else
    {
        ConnectionTimer = def5second;
        CommunicationStatus = defMultiConnected;
        DataReceiveTimer = 0;
    }
    oldCommunicationStatus = CommunicationStatus;

}
extern MainWindow *Frame;
MainWindow *Frame = nullptr;
void MyServer::readyRead()
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    QByteArray *buffer = buffers.value(socket);
    QByteArray senddata;
    ushort ans = 0;

    if ( socket->bytesAvailable() > 0 )
    {
        //qDebug() << "SLOT : " << QTime::currentTime().toString() << "-"  << QTime::currentTime().msec();

        DataReceiveTimer = def20second;

        buffer->append(socket->readAll());

        QString ReceivedData(QString::fromLatin1(*buffer));
        buffer->clear();

        if( CommunicationStatus == defMultiConnected )
            buffer->append("INIT");
        else
        {
            ans = Frame->ParseStr(ReceivedData);
            if( ans == defOK )
                buffer->append("OK");
            else if( ans == defINIT )
                buffer->append("INIT");
            else
                buffer->append("ERROR");
        }

        socket->write(*buffer);
        buffer->clear();
        socket->flush();
    }
    else
    {
        buffer->append("ERROR");
    }

}



