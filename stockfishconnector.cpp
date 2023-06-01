#include "stockfishconnector.h"

StockfishConnector::StockfishConnector(QObject *parent) : QObject(parent)
{
    // Connect the readyRead() signal of the process to a slot
    connect(&Myprocess, &QProcess::readyRead, this, &StockfishConnector::processReadyRead);
    connect(&Myprocess, &QProcess::errorOccurred, this, &StockfishConnector::error);
    // Start the process with the Stockfish engine
    Myprocess.start("C:/Users/User/Documents/NWU/2023/Semester 1/REII 313/Coding/chess_engine_test/stockfish.exe", QStringList(), QIODevice::ReadWrite);
    // Wait for the process to start
    if (!Myprocess.waitForStarted())
    {
        qDebug() << "Error starting Stockfish process.";
    }
}

void StockfishConnector::processReadyRead()
{
    // Read the output from the process
    QString data = QString::fromUtf8(Myprocess.readAllStandardOutput());

    // Find the position of "bestmove" in the output
    int bestmoveIndex = data.indexOf("bestmove");
    int playerbestindex = data.indexOf("ponder");

    // Extract the characters after "bestmove"
    if (bestmoveIndex != -1)
    {
        QString bestmove = data.mid(bestmoveIndex + 9,4).trimmed();
        QString playerbest = data.mid(playerbestindex + 7,4).trimmed();
        if ((playerbest == "(non") || (bestmove == "(non"))
        {
            bestmove = "none";
        }
        emit dataReceived(bestmove.toUtf8());
    }

//            // Emit the outputReceived signal
//    emit dataReceived(data.toUtf8());
}

bool StockfishConnector::writeData(QByteArray data)
{
   // if (Myprocess.open(QIODevice::ReadWrite))
        {
        Myprocess.write(data);
//        Myprocess.waitForBytesWritten();
    }

    return Myprocess.waitForBytesWritten();
}

void StockfishConnector::error()
{
    qDebug()<<"error";
}
