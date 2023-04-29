#include "pawn.h"
#include "mainwindow.h"
#include "board.h"

Pawn::Pawn()
{

}

bool Pawn::ValidMove(char team, int num_moves, bool capture, int from_x, int from_y, int to_x,
                     int to_y, int vertical_up_bound, int vertical_down_bound)
{
    MainWindow mainw;

    if (team == 'w')
    {
        vertical_boundary = vertical_up_bound;
    }else{
        vertical_boundary = vertical_down_bound;
    }

    if ((to_x == from_x) && (to_y == from_y))
    {
        return false;
    }else if ((abs(to_y-from_y)) >= vertical_boundary)
    {
        return false;
    }else if ((num_moves == 0) && ((abs(to_y-from_y)) >= 3))
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
    }else if ((num_moves != 0) && ((abs(to_y-from_y)) >= 2))
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
