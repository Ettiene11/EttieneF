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
        return true;                                        // |  etc...
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

void MainWindow::Calcknightpossiblemoves(int from_x, int from_y)
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

    int i = 0;
    if (!((short_right >= 9) || (long_up >= 9) || (short_right <= 0) || (long_up <= 0)))
    {
        possiblexmoves[i] = short_right;
        possibleymoves[i] = long_up;
        ++i;
    }
    if (!((short_left >= 9) || (long_up >= 9) || (short_left <= 0) || (long_up <= 0)))
    {
        possiblexmoves[i] = short_left;
        possibleymoves[i] = long_up;
        ++i;
    }
    if (!((short_right >= 9) || (long_down >= 9) || (short_right <= 0) || (long_down <= 0)))
    {
        possiblexmoves[i] = short_right;
        possibleymoves[i] = long_down;
        ++i;
    }
    if (!((short_left >= 9) || (long_down >= 9) || (short_left <= 0) || (long_down <= 0)))
    {
        possiblexmoves[i] = short_left;
        possibleymoves[i] = long_down;
        ++i;
    }
    if (!((long_left >= 9) || (short_down >= 9) || (long_left <= 0) || (short_down <= 0)))
    {
        possiblexmoves[i] = long_left;
        possibleymoves[i] = short_down;
        ++i;
    }
    if (!((long_right >= 9) || (short_down >= 9) || (long_right <= 0) || (short_down <= 0)))
    {
        possiblexmoves[i] = long_right;
        possibleymoves[i] = short_down;
        ++i;
    }
    if (!((long_left >= 9) || (short_up >= 9) || (long_left <= 0) || (short_up <= 0)))
    {
        possiblexmoves[i] = long_left;
        possibleymoves[i] = short_up;
        ++i;
    }
    if (!((long_right >= 9) || (short_up >= 9) || (long_right <= 0) || (short_up <= 0)))
    {
        possiblexmoves[i] = long_right;
        possibleymoves[i] = short_up;
        ++i;
    }
}
