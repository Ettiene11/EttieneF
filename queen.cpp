#include "queen.h"
#include "board.h"

Queen::Queen()
{
}

bool Queen::ValidMove(char team, int from_x, int from_y, int to_x,int to_y, int vertical_up_bound, int vertical_down_bound,
                      int horizontal_left_bound, int horizontal_right_bound, int right_up_diagonal_bound, int left_up_diagonal_bound,
                      int right_down_diagonal_bound, int left_down_diagonal_bound)
{
    board board;

    if ((to_x == from_x) && (to_y == from_y))
    {
        return false;
    } else if (((to_x != from_x) && (to_y != from_y)) && ((abs(to_x - from_x)) != (abs(to_y - from_y))))
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
