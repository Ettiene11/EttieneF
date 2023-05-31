#include "client.h"
#include "iostream"
#include "mainwindow.h"

static inline QByteArray IntToArray(qint32 source);
static inline qint32 ArrayToInt(QByteArray source);


Client::Client(QObject *parent) : QObject(parent)
{
    rsize = 0;

    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

bool Client::connectToHost(QString host)
{
    socket->connectToHost(host, 7755);
    return socket->waitForConnected();
}

bool Client::writeData(QByteArray data)
{
    MainWindow mainw;
    if (client_turn || !mainw.playing_game)
    {
        if(socket->state() == QAbstractSocket::ConnectedState)
        {
            client_turn = false;
            socket->write(IntToArray(data.size())); //write size of data
            socket->write(data); //write the data itself
            return socket->waitForBytesWritten();
        }else{return false;}
    }else{
        std::cout << "Not your turn" << std::endl;
        return false;
    }
}

void Client::readyRead()
{
    while (socket->bytesAvailable() > 0)
    {
        rbuf.append(socket->readAll());
        //emit dataReceived(QByteArray::number(rbuf.size()));
        while ((rsize == 0 && rbuf.size() >= 4) || (rsize > 0 && rbuf.size() >= rsize))
        {
            if (rsize == 0 && rbuf.size() >= 4) //if size of data has received completely, then store it on our global variable
            {
                rsize = ArrayToInt(rbuf.mid(0, 4));
                rbuf.remove(0, 4);
            }
            if (rsize > 0 && rbuf.size() >= rsize) // If data has received completely, then emit our SIGNAL with the data
            {
                QByteArray data = rbuf.mid(0, rsize);
                rbuf.remove(0, rsize);
                rsize = 0;
                emit dataReceived(data);
                client_turn = true;
            }
        }
    }
}

qint32 ArrayToInt(QByteArray source)
{
    qint32 temp;
    QDataStream data(&source, QIODevice::ReadWrite);
    data >> temp;
    return temp;
}

QByteArray IntToArray(qint32 source)
{
    QByteArray temp;
    QDataStream data(&temp, QIODevice::ReadWrite);
    data << source;
    return temp;
}
