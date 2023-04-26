#include "knight.h"
#include "board.h"

Knight::Knight()
{
}

bool Knight::ValidMove(char team, int from_x, int from_y, int to_x, int to_y)
{
    board board;
    int short_right, long_right, short_left, long_left, short_up, short_down, long_up, long_down;
    short_right = from_x+1;
    long_right = from_x+2;
    short_left = from_x-1;
    long_left = from_x-2;
    short_up = from_y+1;
    short_down = from_y-1;
    long_up = from_y+2;
    long_down = from_y-2;

    QVectorIterator<piecetracker*> tracker(piece_tracker);
    while (tracker.hasNext())
    {
        piecetracker *t = tracker.next();
        if ((t->team == team) && (t->x_cor == board.AssignxCoord(to_x)) && (t->y_cor == board.AssignyCoord(to_y)))  //may not move onto own team's piece
        {
            return false;
        }
    }

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
