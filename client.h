#ifndef CLIENT_H
#define CLIENT_H


#include <QtCore>
#include <QtNetwork/QTcpSocket>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);

signals:
    void dataReceived(QByteArray);

public slots:
    bool connectToHost(QString host);
    bool writeData(QByteArray data);

private:
    bool client_turn = true;
    QTcpSocket *socket;
    QByteArray rbuf;
    qint32 rsize;

private slots:
    void readyRead();
};

#endif // CLIENT_H
