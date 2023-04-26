#ifndef KING_H
#define KING_H
#include <mainwindow.h>

class King: public MainWindow
{
public:
    King();
    bool ValidMove(char, bool, int, int, int, int);
};

#endif // KING_H
