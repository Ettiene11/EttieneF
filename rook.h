#ifndef ROOK_H
#define ROOK_H
#include <mainwindow.h>

class Rook: public MainWindow
{
public:
    Rook();
    bool ValidMove(char, int, int, int, int, int, int, int, int);

private:

};

#endif // ROOK_H
