#include "knight.h"

Knight::Knight()
{
}

bool Knight::ValidMove(int from_x, int from_y, int to_x, int to_y)
{
    int short_right, long_right, short_left, long_left, short_up, short_down, long_up, long_down;
    short_right = from_x+1;
    long_right = from_x+2;
    short_left = from_x-1;
    long_left = from_x-2;
    short_up = from_y+1;
    short_down = from_y-1;
    long_up = from_y+2;
    long_down = from_y-2;

    if ((to_x == short_right) && ((to_y == long_up)))
    {                                                       //   |
        return true;                                        //  _|
    }
    if ((to_x == short_left) && ((to_y == long_up)))
    {                                                       //  |
        return true;                                        //  |_
    }
    if ((to_x == short_right) && ((to_y == long_down)))     //  _
    {                                                       //   |
        return true;                                        //   |
    }
    if ((to_x == short_left) && ((to_y == long_down)))      //  _
    {                                                       // |
        return true;                                        // |  ect...
    }
    if ((to_x == long_right) && ((to_y == short_down)))
    {
        return true;
    }
    if ((to_x == long_right) && ((to_y == short_up)))
    {
        return true;
    }
    if ((to_x == long_left) && ((to_y == short_down)))
    {
        return true;
    }
    if ((to_x == long_left) && ((to_y == short_up)))
    {
        return true;
    }
    return false;
}
