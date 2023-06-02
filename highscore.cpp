#include "highscore.h"
#include <iostream>
using namespace std;

HighScore::HighScore()
{
    //cout << "Constructor: skryf iets\n";
//    scoresFile.setFileName("C:/Users/User/Documents/NWU/2023/Semester 1/REII 313/Coding/Chess_final/textfiles/scores.txt");
    scoresFile.setFileName("C:/Users/User/Documents/NWU/2023/Semester 1/REII 313/Coding/Chess_final/textfiles/scores.txt");
    if (scoresFile.open(QIODevice::ReadOnly))
    {
        //cout << "Constructor: file open\n";
        while (!scoresFile.atEnd())
        {
            QByteArray line = scoresFile.readLine().trimmed();
            int pos = line.indexOf("\t");
            if (pos != -1)
            {
                QByteArray p1 = line.mid(0, pos);
                QByteArray p2 = line.mid(pos+1);
                //cout << "Constructor read line: " << p1.toStdString() << " " << p2.toStdString() << endl;
                scores.insert(p1.toInt(), p2);
            }
        }
    }
    scoresFile.close();
}

void HighScore::addScore(QByteArray naam, int punt)
{
//    getScore(naam);
//    QFile scoresFile("C:/Users/User/Documents/NWU/2023/Semester 1/REII 313/Coding/build highscore/highscore/scores.txt");

//    if (scoresFile.open(QIODevice::ReadOnly))
//    {
//        //cout << "Constructor: file open\n";
//        while (!scoresFile.atEnd())
//        {
//            QByteArray line = scoresFile.readLine().trimmed();
//            int pos = line.indexOf(naam);       //soek vir piet ipv \t
//            if (pos != -1)      //kry piet ja
//            {
//                //cout << "Constructor read line: " << p1.toStdString() << " " << p2.toStdString() << endl;
//                punt=punt+1;

//                QByteArray byte = QString::number(punt).toUtf8() + "\t" +naam;
////                QString::number(punt);
////                scoresFile.write("\n"+byte);
//                break;
//            }
//            punt = 0;
//        }
//    }

//    scores.insert(punt, naam);
    scores.insert(punt, naam);

//    QFile scoresFile(":textfiles/scores.txt");
//    scoresFile.open(":textfiles/scores.txt");
//    scoresFile.setFileName(":textfiles/scores.txt");
//    QFile scoresFile("C:/Users/User/Documents/NWU/2023/Semester 1/REII 313/Coding/build highscore/highscore/scores.txt");
    QFile scoresFile("C:/Users/User/Documents/NWU/2023/Semester 1/REII 313/Coding/Chess_final/textfiles/scores.txt");

    if (scoresFile.open(QIODevice::Truncate | QIODevice::WriteOnly))
//    if (scoresFile.open(QIODevice::WriteOnly))
    {
        //cout << "addScore: file open\n";
        int n = 1;
        QMapIterator<int, QByteArray> i(scores);
        i.toBack();
        while (i.hasPrevious())
        {
            i.previous();
            QByteArray line;
            line.append(QByteArray::number(i.key()) + "\t" + i.value() + "\n");
            scoresFile.write(line);
            if (n >= 10)
                break;

            ++n;
        }
    }
    scoresFile.close();
}

void HighScore::UpdateScore(QByteArray naam)
{

    QFile scoresFile("C:/Users/User/Documents/NWU/2023/Semester 1/REII 313/Coding/Chess_final/scores.txt");

    if (scoresFile.open(QIODevice::WriteOnly))
    {
        //cout << "Constructor: file open\n";
        while (!scoresFile.atEnd())
        {
            QByteArray line = scoresFile.readLine().trimmed();
            int pos = line.indexOf(naam);       //soek vir piet ipv \t
            if (pos != -1)      //kry piet ja
            {
//                QString stringscore = line.mid(pos-3,2);
                QString stringscore = line.mid(0,pos-1);
                int score = stringscore.toInt();
                qDebug() << score;
                scores.insert(++score, naam);
                scoresFile.write("0\t" + naam);
            }
        }
    }
    qDebug() << 0;
}

