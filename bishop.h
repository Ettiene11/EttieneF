#ifndef BISHOP_H
#define BISHOP_H
#include <mainwindow.h>

class Bishop: MainWindow
{
public:
    Bishop();
    bool ValidMove(char, int, int, int, int, int, int, int, int);
};

#endif // BISHOP_H
