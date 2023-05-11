#include "server.h"
#include "iostream"
#include "mainwindow.h"

static inline qint32 ArrayToInt(QByteArray source);
static inline QByteArray IntToArray(qint32 source);

Server::Server(QObject *parent) : QObject(parent)
{
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), SLOT(newConnection()));
    qDebug() << "Listening:" << server->listen(QHostAddress::Any, 7755);
}

void Server::newConnection()
{
    while (server->hasPendingConnections())
    {
        socket = server->nextPendingConnection();
        connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
        QByteArray *buffer = new QByteArray();
        qint32 *s = new qint32(0);
        buffers.insert(socket, buffer);
        sizes.insert(socket, s);
    }
}

void Server::disconnected()
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    QByteArray *buffer = buffers.value(socket);
    buffers.remove(socket);
    qint32 *s = sizes.value(socket);
    sizes.remove(socket);
    socket->deleteLater();
    delete buffer;
    delete s;
}

void Server::readyRead()
{
    QByteArray *buffer = buffers.value(socket);
    qint32 *s = sizes.value(socket);
    qint32 size = *s;
    while (socket->bytesAvailable() > 0)
    {
        buffer->append(socket->readAll());
        while ((size == 0 && buffer->size() >= 4) || (size > 0 && buffer->size() >= size)) //While can process data, process it
        {
            if (size == 0 && buffer->size() >= 4) //if size of data has received completely, then store it on our global variable
            {
                size = ArrayToInt(buffer->mid(0, 4));
                *s = size;
                buffer->remove(0, 4);
            }
            if (size > 0 && buffer->size() >= size) // If data has received completely, then emit our SIGNAL with the data
            {
                QByteArray data = buffer->mid(0, size);
                buffer->remove(0, size);
                size = 0;
                *s = size;
                emit dataReceived(data);
                server_turn = true;
            }
        }
    }
}

bool Server::writeData(QByteArray data)
{
    if ((server_turn) || (first_time_setup))
    {
        if(socket->state() == QAbstractSocket::ConnectedState)
        {
            first_time_setup = false;
            server_turn = false;
            socket->write(IntToArray(data.size())); //write size of data
            socket->write(data); //write the data itself
            return socket->waitForBytesWritten();
        }else{return false;}
    }else
    {
        std::cout << "Not your turn" << std::endl;
        return false;
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
