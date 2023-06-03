#include "pawn.h"
#include "mainwindow.h"
#include "board.h"

Pawn::Pawn()
{

}

bool Pawn::ValidMove(char team, int num_moves, bool capture, int from_x, int from_y, int to_x,
                     int to_y, int vertical_up_bound, int vertical_down_bound, int right_up_diagonal_bound, int left_up_diagonal_bound,
                     int right_down_diagonal_bound, int left_down_diagonal_bound)
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

void MainWindow::Calcpawnpossiblemoves(char team, int num_moves, int from_x, int from_y,
                             int vertical_up_bound, int vertical_down_bound, int right_up_diagonal_bound, int left_up_diagonal_bound,
                             int right_down_diagonal_bound, int left_down_diagonal_bound)
{
    int i = 0;

    if (team == 'w')
    {

        if (L_En_passant(team,from_x,from_y))
        {
            possibleymoves[i] = from_y + 1;
            possiblexmoves[i] = from_x - 1;
            ++i;
        }

        if (R_En_passant(team,from_x,from_y))
        {
            possibleymoves[i] = from_y + 1;
            possiblexmoves[i] = from_x + 1;
            ++i;
        }

        if ((vertical_up_bound > 1) && (from_y + 1 != 9))
        {
            possibleymoves[i] = from_y + 1;   //forward one block
            possiblexmoves[i] = from_x;
            ++i;
        }

        if ((num_moves == 0) && (vertical_up_bound > 2))
        {
            possibleymoves[i] = from_y + 2;   //forward 2 blocks if 1st move
            possiblexmoves[i] = from_x;
            ++i;
        }

        if ((right_up_diagonal_bound == 1) && (Checkcapture(team,from_x + 1,from_y + 1)) && (from_y + 1 != 9) && (from_x + 1 != 9))
        {
            possibleymoves[i] = from_y + 1;   //capture right
            possiblexmoves[i] = from_x + 1;
            ++i;
        }

        if ((left_up_diagonal_bound == 1) && (Checkcapture(team,from_x - 1,from_y + 1)) && (from_y + 1 != 9) && (from_x - 1 != 0))
        {
            possibleymoves[i] = from_y + 1;   //capture left
            possiblexmoves[i] = from_x - 1;
            ++i;
        }
    }

    if (team == 'b')
    {
        if (L_En_passant(team,from_x,from_y))
        {
            possibleymoves[i] = from_y - 1;
            possiblexmoves[i] = from_x - 1;
            ++i;
        }

        if (R_En_passant(team,from_x,from_y))
        {
            possibleymoves[i] = from_y - 1;
            possiblexmoves[i] = from_x + 1;
            ++i;
        }

        if ((vertical_down_bound > 1) && (from_y - 1 != 0))
        {
            possibleymoves[i] = from_y - 1;   //forward one block
            possiblexmoves[i] = from_x;
            ++i;
        }

        if ((num_moves == 0) && (vertical_down_bound > 2))
        {
            possibleymoves[i] = from_y - 2;   //forward 2 blocks if 1st move
            possiblexmoves[i] = from_x;
            ++i;
        }

        if ((right_down_diagonal_bound == 1) && (Checkcapture(team,from_x + 1,from_y - 1)) && (from_y - 1 != 0) && (from_x + 1 != 9))
        {
            possibleymoves[i] = from_y - 1;   //capture right
            possiblexmoves[i] = from_x + 1;
            ++i;
        }

        if ((left_down_diagonal_bound == 1) && (Checkcapture(team,from_x - 1,from_y - 1)) && (from_y - 1 != 0) && (from_x - 1 != 0))
        {
            possibleymoves[i] = from_y - 1;   //capture left
            possiblexmoves[i] = from_x - 1;
            ++i;
        }
    }
}
