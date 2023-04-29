#ifndef PAWN_H
#define PAWN_H
#include <QLabel>
#include <mainwindow.h>


class Pawn: public MainWindow
{
public:
    Pawn();
    bool ValidMove(char, int, bool, int, int, int, int, int, int);

private:
    int vertical_boundary;

};

#endif // PAWN_H
