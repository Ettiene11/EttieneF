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
