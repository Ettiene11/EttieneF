#ifndef STOCKFISHCONNECTOR_H
#define STOCKFISHCONNECTOR_H

#include <QObject>
#include <QProcess>
#include <QDebug>
#include <QtCore>

class StockfishConnector : public QObject
{
    Q_OBJECT

public:
    explicit StockfishConnector(QObject *parent = nullptr);

signals:
    void dataReceived(QByteArray);

public slots:
    bool writeData(QByteArray data);

private slots:
    void processReadyRead();
    void error();

private:
    QProcess Myprocess;
};

#endif // STOCKFISHCONNECTOR_H
