#ifndef PAWN_H
#define PAWN_H
#include <QLabel>
#include <mainwindow.h>


class Pawn: public MainWindow
{
public:
    Pawn();
    bool ValidMove(char, bool, bool, int, int, int, int);

};

#endif // PAWN_H
