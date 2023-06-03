#include "king.h"
#include "board.h"

King::King()
{
}

bool King::ValidMove(char team, bool check, int from_x, int from_y, int to_x, int to_y)
{
    if ((to_x == from_x) && (to_y == from_y))
    {
        return false;
    } else if ((to_x != from_x) && (abs(to_x-from_x) > 1))
    {
        return false;
    } else if ((to_y != from_y) && (abs(to_y-from_y) > 1))
    {
        return false;
    }
    return true;
}

void MainWindow::Calckingpossiblemoves(int nummoves, int from_x, int from_y, int vertical_up_bound, int vertical_down_bound,
                                       int horizontal_left_bound, int horizontal_right_bound, int right_up_diagonal_bound, int left_up_diagonal_bound,
                                       int right_down_diagonal_bound, int left_down_diagonal_bound)
{
    int i = 0;
    if (Castling(nummoves, horizontal_left_bound, horizontal_right_bound, from_x + 3, from_y))
    {
        possiblexmoves[i] = from_x + 2;
        possibleymoves[i] = from_y;
        ++i;
    }
    if (Castling(nummoves, horizontal_left_bound, horizontal_right_bound, from_x - 4, from_y))
    {
        possiblexmoves[i] = from_x - 2;
        possibleymoves[i] = from_y;
        ++i;
    }
    if (vertical_up_bound >= 1)
    {
        if ((from_y + 1 != 9) && (from_y + 1 != 0))
        {
            possiblexmoves[i] = from_x;
            possibleymoves[i] = from_y + 1;
            ++i;
        }
    }
    if (vertical_down_bound >= 1)
    {
        if ((from_y - 1 != 9) && (from_y - 1 != 0))
        {
            possiblexmoves[i] = from_x;
            possibleymoves[i] = from_y - 1;
            ++i;
        }
    }
    if (horizontal_left_bound >= 1)
    {
        if ((from_x - 1 != 9) && (from_x - 1 != 0))
        {
            possiblexmoves[i] = from_x - 1;
            possibleymoves[i] = from_y;
            ++i;
        }
    }
    if (horizontal_right_bound >= 1)
    {
        if ((from_x + 1 != 9) && (from_x + 1 != 0))
        {
            possiblexmoves[i] = from_x + 1;
            possibleymoves[i] = from_y;
            ++i;
        }
    }
    if (right_up_diagonal_bound >= 1)
    {
        if ((from_x + 1 != 9) && (from_x + 1 != 0) && (from_y + 1 != 9) && (from_y + 1 != 0))
        {
            possiblexmoves[i] = from_x + 1;
            possibleymoves[i] = from_y + 1;
            ++i;
        }
    }
    if (right_down_diagonal_bound >= 1)
    {
        if ((from_x + 1 != 9) && (from_x + 1 != 0) && (from_y - 1 != 9) && (from_y - 1 != 0))
        {
            possiblexmoves[i] = from_x + 1;
            possibleymoves[i] = from_y - 1;
            ++i;
        }
    }
    if (left_up_diagonal_bound >= 1)
    {
        if ((from_x - 1 != 9) && (from_x - 1 != 0) && (from_y + 1 != 9) && (from_y + 1 != 0))
        {
            possiblexmoves[i] = from_x - 1;
            possibleymoves[i] = from_y + 1;
            ++i;
        }
    }
    if (left_down_diagonal_bound >= 1)
    {
        if ((from_x - 1 != 9) && (from_x - 1 != 0) && (from_y - 1 != 9) && (from_y - 1 != 0))
        {
            possiblexmoves[i] = from_x - 1;
            possibleymoves[i] = from_y - 1;
            ++i;
        }
    }
}
