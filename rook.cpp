#include "rook.h"
#include "board.h"

Rook::Rook()
{
}

bool Rook::ValidMove(char team, int from_x, int from_y, int to_x,int to_y, int vertical_up_bound, int vertical_down_bound,
                     int horizontal_left_bound, int horizontal_right_bound)
{
    board board;

    if ((from_x == to_x) && (from_y == to_y))
    {
        return false;
    } else if ((from_x != to_x) && (from_y != to_y))
    {
        return false;
    } else if ((from_x == to_x) && (to_y > from_y) && ((to_y-from_y) > vertical_up_bound))
    {
        return false;
    } else if ((from_x == to_x) && (to_y < from_y) && ((from_y-to_y) > vertical_down_bound))
    {
        return false;
    } else if ((from_y == to_y) && (to_x < from_x) && ((from_x-to_x) > horizontal_left_bound))
    {
        return false;
    } else if ((from_y == to_y) && (to_x > from_x) && ((to_x-from_x) > horizontal_right_bound))
    {
        return false;
    }

    return true;
}

void MainWindow::Calcrookpossiblemoves(int from_x, int from_y, int vertical_up_bound, int vertical_down_bound,
                                       int horizontal_left_bound, int horizontal_right_bound)
{
    int i = 0;
    for (int j = 1; j <= vertical_up_bound; ++j)
    {
        if (!((from_y + j >= 9) || (from_y + j <= 0)))
        {
            possibleymoves[i] = from_y + j;
            possiblexmoves[i] = from_x;
            ++i;
        }
    }
    for (int j = 1; j <= vertical_down_bound; ++j)
    {
        if (!((from_y - j >= 9) || (from_y - j <= 0)))
        {
            possibleymoves[i] = from_y - j;
            possiblexmoves[i] = from_x;
            ++i;
        }
    }
    for (int j = 1; j <= horizontal_left_bound; ++j)
    {
        if (!((from_x - j >= 9) || (from_x - j <= 0)))
        {
            possibleymoves[i] = from_y;
            possiblexmoves[i] = from_x - j;
            ++i;
        }
    }
    for (int j = 1; j <= horizontal_right_bound; ++j)
    {
        if (!((from_x + j >= 9) || (from_x + j <= 0)))
        {
            possibleymoves[i] = from_y;
            possiblexmoves[i] = from_x + j;
            ++i;
        }
    }
}
