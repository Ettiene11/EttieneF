#include "pawn.h"
#include "mainwindow.h"

Pawn::Pawn()
{

}

bool Pawn::ValidMove(char team, bool firstmove, bool capture, int from_x, int from_y, int to_x, int to_y)
{
    MainWindow mainw;

    if ((firstmove == true) && ((abs(to_y-from_y)) >= 3))
    {
        return false;
    }else if (((to_y-from_y) > 0) && (team == 'b'))  //black team moves back (up)
    {
        return false;
    }else if (((to_y-from_y) < 0) && (team == 'w')) //white team moves back (down)
    {
        return false;
    }else if ((from_x != to_x) && (capture == false))  //diagonally without take
    {
        return false;
    }else if ((firstmove == false) && ((abs(to_y-from_y)) >= 2))
    {
        return false;
    }else if ((from_x != to_x) && (from_y == to_y))
    {
        return false;
    }else if ((from_x == to_x) && (capture == true))
    {
        return false;
    }else if (((from_y != to_y)) && (from_x != to_x))  //more than 1 block diagonally
    {
        if ((((abs(to_y-from_y)) > 1) || ((abs(to_x-from_x)) > 1)))
        {
           return false;
        }
    }
    return true;
}
