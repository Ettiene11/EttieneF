#ifndef HIGHSCORE_H
#define HIGHSCORE_H
#include <QMap>
#include <QByteArray>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDir>
#include <QDebug>

class HighScore
{
public:
    HighScore();
    void addScore(QByteArray, int);
    void UpdateScore(QByteArray);

private:
    QMultiMap<int, QByteArray> scores;
    QFile scoresFile;
};

#endif // HIGHSCORE_H
