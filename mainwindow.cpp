#include "mainwindow.h"
#include <board.h>
#include <pawn.h>
#include <knight.h>
#include <rook.h>
#include <bishop.h>
#include <queen.h>
#include <king.h>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    //    set background
    setFixedSize(width, height);
    QPixmap bkgnd("C:/Users/User/Documents/NWU/2023/Semester 1/REII 313/Coding/Chess/board.png");
    bkgnd = bkgnd.scaled(width,height, Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);

    DefaultBoard();
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    ++click_counter;

    if (click_counter == 1)
    {
        board board;
        QVectorIterator<piecetracker*> tracker(piece_tracker);

        from_xcoord = GetxPosition(e->x());
        from_ycoord = GetyPosition(e->y());

        while (tracker.hasNext())
        {
            piecetracker *t = tracker.next();

            if ((board.AssignxCoord(from_xcoord) == t->x_cor) && (board.AssignyCoord(from_ycoord) == t->y_cor))
            {
                clicked_on_piece = true;
                if(t->team != turn){
                    cout << "not your turn" << endl;
                    click_counter = 0;
                    break;
                }

                if (((t->y_cor == 600) && (t->team == 'w')) || ((t->y_cor == 100) && (t->team == 'b')))
                {firstmove = true; }else{ firstmove = false; }

                for (int i = 1; i <= 8; ++i)
                {
                    for (int j = 1; j <= 8; ++j)
                    {
                        if ((Validmove(i, j)) && (Validpiecemove(t->team,t->type,from_xcoord, from_ycoord,i,j)))
                        {
                            if (!Check_yourself(t->team,from_xcoord, from_ycoord,i,j,t))
                            {
                                QLabel* new_move = new QLabel(this);
                                new_move->setText("X");
                                new_move->show();
                                new_move->move(board.AssignxCoord(i), board.AssignyCoord(j));
                                possible_moves.append(new_move);
                            }
                        }
                    }
                }

                if (possible_moves.isEmpty())
                {
                    cout << "this piece cannot move" << endl;
                    click_counter = 0;
                }
            }
        }
        if (!clicked_on_piece)
        {
            cout << "did not click on piece" << endl;
            click_counter = 0;
        }else{
            clicked_on_piece = false;
        }
    }

    if (click_counter == 2)
    {
        to_xcoord = GetxPosition(e->x());
        to_ycoord = GetyPosition(e->y());
        click_counter = 0;

        if (!Clicked_on_Piece(from_xcoord,from_ycoord))
        {
            cout << "did not click on piece" << endl;
        }

    }
}

int MainWindow::GetxPosition(int xcoord)
{
    return int(xcoord/100)+1;
}

int MainWindow::GetyPosition(int ycoord)
{
    if ((((800-ycoord)%100) != 0) || (ycoord == 800))
    {
        return int((800-ycoord)/100)+1;
    }else{
        return int((800-ycoord)/100);
    }
}

void MainWindow::Makepiece(QString text, char type, char team, int x, int y)
{
    QLabel* new_piece = new QLabel(this);
    new_piece->setText(text);
    new_piece->show();
    new_piece->move(x, y);
    all_pieces.append(new_piece);

    piecetracker* new_piecetracker = new piecetracker;
    new_piecetracker->team = team;
    new_piecetracker->type = type;
    new_piecetracker->x_cor = x;
    new_piecetracker->y_cor = y;
    piece_tracker.append(new_piecetracker);
}

bool MainWindow::Clicked_on_Piece(int x, int y)
{
    board board;
    QVectorIterator<piecetracker*> tracker(piece_tracker);
    QVectorIterator<QLabel*> piece(all_pieces);

    while (tracker.hasNext())
    {
        piecetracker *t = tracker.next();
        QLabel *p = piece.next();

        if ((board.AssignxCoord(x) == t->x_cor) && (board.AssignyCoord(y) == t->y_cor))
        {
            if ((!(Validmove(to_xcoord, to_ycoord))) || (!(Validpiecemove(t->team,t->type,from_xcoord, from_ycoord,to_xcoord,to_ycoord))) || Check_yourself(t->team,from_xcoord, from_ycoord,to_xcoord,to_ycoord,t))
            {
                cout << "not valid move" << endl;
            }else{
                    if (capture == true)
                    {
                        piece_tracker.removeOne(piece_tracker[enemy_index]);
                        all_pieces[enemy_index]->hide();
                        all_pieces.removeOne(all_pieces[enemy_index]);
                    }

                    p->move(board.AssignxCoord(to_xcoord), board.AssignyCoord(to_ycoord));
                    t->x_cor = board.AssignxCoord(to_xcoord);
                    t->y_cor = board.AssignyCoord(to_ycoord);

                    if (((t->type == 'p')&&(t->team == 'w')&&(t->y_cor == 0))             //see if pawn can be transformed
                            || ((t->type == 'p')&&(t->team == 'b')&&(t->y_cor == 700)))
                    {
                        if (t->team == 'w')
                        {
                           p->setText("WQ");
                        }else{p->setText("BQ");}
                        t->type = 'q';
                    }

                    if (turn == 'w')
                    {
                        turn = 'b';
                    }else{
                        turn = 'w';
                    }

                    if (Check_opponent(t->x_cor, t->y_cor, t))
                    {
                        cout << turn << "'s king is in check, checking for possible checkmate..." << endl;
                        if (Checkmate())
                        {
                           cout << "Checkmate, " << t->team << " won!" << endl;
                        }else{
                           cout << turn << "'s king is in check, no checkmate!" << endl;
                        }
                    }
                }
            Delete_possible_moves();
            return 1;
        }
    }
    return 0;
}

bool MainWindow::Validmove(int x, int y)  //check if piece moved into open space or onto enemy team's piece
{
    board board;
    QVectorIterator<piecetracker*> tracker(piece_tracker);

    while (tracker.hasNext())
    {
        piecetracker *t = tracker.next();

        if ((t->x_cor == board.AssignxCoord(x)) && (t->y_cor == board.AssignyCoord(y)))
        {
            if (t->team != turn)
            {
                capture = true;
                enemy_index = piece_tracker.indexOf(t);
                return true;
            }else{return false;}
        }
    }

    capture = false;
    return true;  //open space thus valid move
}

bool MainWindow::Validpiecemove(char team, char type, int from_x, int from_y, int x, int y) //check if specific piece has valid move based on its specific characteristical moves
{
    Getboundaries(from_x, from_y);
    Pawn pawn;
    Knight knight;
    Rook rook;
    Bishop bishop;
    Queen queen;
    King king;
    switch (type) {
    case 'p' :
        if (pawn.ValidMove(team, firstmove, capture, from_x, from_y, x, y, vertical_up_boundary, vertical_down_boundary)) {return true;} break;
    case 'n' :
        if (knight.ValidMove(team, from_x, from_y, x, y)) {return true;} break;
    case 'r' :
        if (rook.ValidMove(team, from_x, from_y, x, y, vertical_up_boundary, vertical_down_boundary, horizontal_left_boundary, horizontal_right_boundary)) {return true;} break;
    case 'b' :
        if (bishop.ValidMove(team, from_x, from_y, x, y, right_up_diagonal_boundary, left_up_diagonal_boundary, right_down_diagonal_boundary, left_down_diagonal_boundary)) {return true;} break;
    case 'q' :
        if (queen.ValidMove(team, from_x, from_y, x, y, vertical_up_boundary, vertical_down_boundary, horizontal_left_boundary, horizontal_right_boundary,right_up_diagonal_boundary, left_up_diagonal_boundary, right_down_diagonal_boundary, left_down_diagonal_boundary)) {return true;} break;
    case 'k' :
        if (king.ValidMove(team, check, from_x, from_y, x, y)) {return true;} break;
    }
    return false;
}

void MainWindow::Delete_possible_moves()
{
    QVectorIterator<QLabel*> pos_moves(possible_moves);

    while(pos_moves.hasNext())
    {
        pos_moves.next()->hide();
    }
    possible_moves.clear();
}

void MainWindow::DefaultBoard()
{
    board board;
    //pawns
    for (int i = 1; i<=8; ++i)
    {
        Makepiece("WP",'p','w',board.AssignxCoord(i),board.AssignyCoord(2));
        Makepiece("BP",'p','b',board.AssignxCoord(i),board.AssignyCoord(7));
    }

    //knights (symbol n)
    for (int i = 2; i<=7; i = i+5)
    {
        Makepiece("WN",'n','w',board.AssignxCoord(i),board.AssignyCoord(1));
        Makepiece("BN",'n','b',board.AssignxCoord(i),board.AssignyCoord(8));
    }

    //rooks
    for (int i = 1; i<=8; i = i+7)
    {
        Makepiece("WR",'r','w',board.AssignxCoord(i),board.AssignyCoord(1));
        Makepiece("BR",'r','b',board.AssignxCoord(i),board.AssignyCoord(8));
    }

    //bishops
    for (int i = 3; i<=6; i = i+3)
    {
        Makepiece("WB",'b','w',board.AssignxCoord(i),board.AssignyCoord(1));
        Makepiece("BB",'b','b',board.AssignxCoord(i),board.AssignyCoord(8));
    }

    //queens
    Makepiece("WQ",'q','w',board.AssignxCoord(4),board.AssignyCoord(1));
    Makepiece("BQ",'q','b',board.AssignxCoord(4),board.AssignyCoord(8));

    //kings
    Makepiece("WK",'k','w',board.AssignxCoord(5),board.AssignyCoord(1));
    Makepiece("BK",'k','b',board.AssignxCoord(5),board.AssignyCoord(8));
}

void MainWindow::Getboundaries(int from_x, int from_y)
{
    int vert_up_min = 999, vert_down_min = 999, hort_left_min = 999, hort_right_min = 999,
            right_up_diagonal_bound = 999, right_down_diagonal_bound = 999, left_up_diagonal_bound = 999, left_down_diagonal_bound = 999;
    QVectorIterator<piecetracker*> tracker(piece_tracker);
    while(tracker.hasNext())
    {
        piecetracker *t = tracker.next();
        //vertical
        if (GetxPosition(t->x_cor) == from_x)
        {
            //vert_up
            if (GetyPosition(t->y_cor) > from_y)
            {
                int distance = abs(GetyPosition(t->y_cor)-from_y);
                if (vert_up_min>distance)
                {
                    vert_up_min = distance;
                }
            }
            //vert_down
            if (GetyPosition(t->y_cor) < from_y)
            {
                int distance = abs(GetyPosition(t->y_cor)-from_y);
                if (vert_down_min>distance)
                {
                    vert_down_min = distance;
                }
            }
        }
        //horisontal
        if (GetyPosition(t->y_cor) == from_y)
        {
            //hort_right
            if (GetxPosition(t->x_cor) > from_x)
            {
                int distance = abs(GetxPosition(t->x_cor)-from_x);
                if (hort_right_min>distance)
                {
                    hort_right_min = distance;
                }
            }
            //hort_left
            if (GetxPosition(t->x_cor) < from_x)
            {
                int distance = abs(GetxPosition(t->x_cor)-from_x);
                if (hort_left_min>distance)
                {
                    hort_left_min = distance;
                }
            }
        }
        //diagonal
        if ((abs(GetyPosition(t->y_cor) - from_y)) == (abs(GetxPosition(t->x_cor) - from_x)))
        {
            //right_up
            if (((GetyPosition(t->y_cor) - from_y) > 0) && ((GetxPosition(t->x_cor) - from_x) > 0))
            {
                int distance = abs(GetxPosition(t->x_cor)-from_x);
                if (right_up_diagonal_bound>distance)
                {
                    right_up_diagonal_bound = distance;
                }
            }
            //left_up
            if (((GetyPosition(t->y_cor) - from_y) > 0) && ((from_x - GetxPosition(t->x_cor)) > 0))
            {
                int distance = abs(GetxPosition(t->x_cor)-from_x);
                if (left_up_diagonal_bound>distance)
                {
                    left_up_diagonal_bound = distance;
                }
            }
            //left_down
            if (((from_y - GetyPosition(t->y_cor)) > 0) && ((from_x - GetxPosition(t->x_cor)) > 0))
            {
                int distance = abs(GetxPosition(t->x_cor)-from_x);
                if (left_down_diagonal_bound>distance)
                {
                    left_down_diagonal_bound = distance;
                }
            }
            //right_down
            if (((from_y - GetyPosition(t->y_cor)) > 0) && ((GetxPosition(t->x_cor) - from_x) > 0))
            {
                int distance = abs(GetxPosition(t->x_cor)-from_x);
                if (right_down_diagonal_bound>distance)
                {
                    right_down_diagonal_bound = distance;
                }
            }
        }
    }
    vertical_up_boundary = vert_up_min;
    vertical_down_boundary = vert_down_min;
    horizontal_left_boundary = hort_left_min;
    horizontal_right_boundary = hort_right_min;
    right_up_diagonal_boundary = right_up_diagonal_bound;
    left_up_diagonal_boundary = left_up_diagonal_bound;
    right_down_diagonal_boundary = right_down_diagonal_bound;
    left_down_diagonal_boundary = left_down_diagonal_bound;
}

bool MainWindow::Check_yourself(char team, int from_x, int from_y, int to_x, int to_y, piecetracker* piecetrack)
{
    board board;
    bool check_opponent = false, capture_state;
    capture_state = capture;

    piecetrack->x_cor = board.AssignxCoord(to_x);
    piecetrack->y_cor = board.AssignyCoord(to_y);

    QVectorIterator<piecetracker*> tracker(piece_tracker);
    while (tracker.hasNext())
    {
        piecetracker *t = tracker.next();

        if (t->type == 'k')
        {
            if (t->team == team)
            {
                king_xpos = t->x_cor;
                king_ypos = t->y_cor;
                break;
            }
        }
    }

    tracker.toFront();
    while (tracker.hasNext())
    {
        piecetracker *t = tracker.next();

        if (t->team != team)
        {
            capture = true;
            if (Validpiecemove(t->team,t->type,GetxPosition(t->x_cor),GetyPosition(t->y_cor),GetxPosition(king_xpos),GetyPosition(king_ypos)))
            {
                check_opponent = true;
                break;
            }else{
                check_opponent = false;
            }
        }
    }

    piecetrack->x_cor = board.AssignxCoord(from_x);
    piecetrack->y_cor = board.AssignyCoord(from_y);
    capture = capture_state;
    return check_opponent;
}

bool MainWindow::Check_opponent(int from_x, int from_y, piecetracker* piecetrack)
{
    bool capture_state;
    capture_state = capture;
    board board;
    QVectorIterator<piecetracker*> tracker(piece_tracker);
    while (tracker.hasNext())
    {
        piecetracker *t = tracker.next();

        if (t->type == 'k')
        {
            if (t->team != piecetrack->team)
            {
                king_xpos = t->x_cor;
                king_ypos = t->y_cor;
                break;
            }
        }
    }

    capture = true;
    if (!Validpiecemove(piecetrack->team,piecetrack->type,GetxPosition(from_x),GetyPosition(from_y),GetxPosition(king_xpos),GetyPosition(king_ypos)))
    {
        capture = capture_state;
        return false;
    }else{
        capture = capture_state;
        return true;
    }
}

bool MainWindow::Checkmate()
{
    QVectorIterator<piecetracker*> tracker(piece_tracker);
    while (tracker.hasNext())
    {
        piecetracker *pt = tracker.next();
        if (pt->team == turn)
        {
            for (int i = 1; i <= 8; ++i)
            {
                for (int j = 1; j <= 8; ++j)
                {
                    if ((Validmove(i, j)) && (Validpiecemove(pt->team,pt->type,GetxPosition(pt->x_cor), GetyPosition(pt->y_cor),i,j)))
                    {
                        if (!Check_yourself(pt->team,GetxPosition(pt->x_cor),GetyPosition(pt->y_cor),i,j,pt))
                        {
                            return false;
                        }
                    }
                }
            }
        }
    }

    return true;
}

MainWindow::~MainWindow()
{
}
