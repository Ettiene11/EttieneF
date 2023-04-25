#include "board.h"

board::board()
{

}

int board::AssignxCoord(int x)
{
    return (100*x) - 100;           //assume image coord is upper left corner
}

int board::AssignyCoord(int y)
{
    return 800 - (100*y);
}
