#include "bishop.h"
#include "board.h"

Bishop::Bishop()
{
}

bool Bishop::ValidMove(char team, int from_x, int from_y, int to_x,int to_y, int right_up_diagonal_bound, int left_up_diagonal_bound,
                       int right_down_diagonal_bound, int left_down_diagonal_bound)
{
    board board;

    if ((to_x == from_x) && (to_y == from_y))
    {
        return false;
    } else if ((abs(to_x - from_x)) != (abs(to_y - from_y)))
    {
        return false;
    } else if ((to_x > from_x) && (to_y > from_y) && ((to_x-from_x) > right_up_diagonal_bound))
    {
        return false;
    } else if ((to_x < from_x) && (to_y > from_y) && ((to_y-from_y) > left_up_diagonal_bound))
    {
        return false;
    } else if ((to_x > from_x) && (to_y < from_y) && ((to_x-from_x) > right_down_diagonal_bound))
    {
        return false;
    } else if ((to_x < from_x) && (to_y < from_y) && ((from_x-to_x) > left_down_diagonal_bound))
    {
        return false;
    }
    return true;
}
