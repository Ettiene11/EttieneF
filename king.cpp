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
