#ifndef QUEEN_H
#define QUEEN_H
#include <mainwindow.h>

class Queen: public MainWindow
{
public:
    Queen();
    bool ValidMove(char, int, int, int, int, int, int, int, int, int, int, int, int);
};

#endif // QUEEN_H
