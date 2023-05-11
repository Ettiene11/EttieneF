#ifndef SERVER_H
#define SERVER_H

#include <QtCore>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    QTcpServer *server;

signals:
    void dataReceived(QByteArray);

public slots:
    bool writeData(QByteArray data);

private slots:
    void newConnection();
    void disconnected();
    void readyRead();


private:
    bool server_turn = false, first_time_setup = true;
    QTcpSocket *socket;
    QHash<QTcpSocket*, QByteArray*> buffers; //We need a buffer to store data until block has completely received
    QHash<QTcpSocket*, qint32*> sizes; //We need to store the size to verify if a block has received completely
};

#endif // SERVER_H
