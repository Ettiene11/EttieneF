#ifndef KNIGHT_H
#define KNIGHT_H
#include <QLabel>
#include <mainwindow.h>

class Knight: public MainWindow
{
public:
    Knight();
    bool ValidMove(char, int, int, int, int);
};

#endif // KNIGHT_H
