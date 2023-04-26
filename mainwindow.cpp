#include "mainwindow.h"
#include <board.h>
#include <pawn.h>
#include <knight.h>

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
                        if ((Validmove(i, j)) && (Validpiecemove(t->team,t->type,i,j)))
                        {
                            QLabel* new_move = new QLabel(this);
                            new_move->setText("X");
                            new_move->show();
                            new_move->move(board.AssignxCoord(i), board.AssignyCoord(j));
                            possible_moves.append(new_move);
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
            if ((!(Validmove(to_xcoord, to_ycoord))) || (!(Validpiecemove(t->team,t->type,to_xcoord,to_ycoord))))
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
                        t->type = 'Q';
                    }

                    if (turn == 'w')
                    {
                        turn = 'b';
                    }else{
                        turn = 'w';
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
            }else{
//                return false;   //moved onto own team's piece, thus invalid move
            }
        }
    }

    capture = false;
    return true;  //open space thus valid move
}

bool MainWindow::Validpiecemove(char team, char type, int x, int y) //check if specific piece has valid move based on its specific characteristical moves
{
    Pawn pawn;
    Knight knight;
    switch (type) {
    case 'p' :
        if (pawn.ValidMove(team, firstmove, capture, from_xcoord, from_ycoord, x, y, Vertical_up_boundary(), Vertical_down_boundary())) {return true;} break;
    case 'k' :
        if (knight.ValidMove(team, from_xcoord, from_ycoord, x, y)) {return true;} break;
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

    //knights
    for (int i = 2; i<=7; i = i+5)
    {
        Makepiece("WK",'k','w',board.AssignxCoord(i),board.AssignyCoord(1));
        Makepiece("BK",'k','b',board.AssignxCoord(i),board.AssignyCoord(8));
    }
}

int MainWindow::Vertical_up_boundary()
{
    QVectorIterator<piecetracker*> tracker(piece_tracker);        //check pieces in path vertically from piece
    while(tracker.hasNext())
    {
        piecetracker *t = tracker.next();
        if (GetxPosition(t->x_cor) == from_xcoord)
        {
            if (GetyPosition(t->y_cor) > from_ycoord)  //above of white piece or below black piece
            {
                int distance = GetyPosition(t->y_cor)-from_ycoord;
                if (min_distance>distance)
                {
                    min_distance = distance;
                }
            }
        }
    }
    int temp = min_distance;
    min_distance = 999;
    return temp;
}

int MainWindow::Vertical_down_boundary()
{
    QVectorIterator<piecetracker*> tracker(piece_tracker);        //check pieces in path vertically from piece
    while(tracker.hasNext())
    {
        piecetracker *t = tracker.next();
        if (GetxPosition(t->x_cor) == from_xcoord)
        {
            if (GetyPosition(t->y_cor) < from_ycoord)  //above of white piece or below black piece
            {
                int distance = from_ycoord-GetyPosition(t->y_cor);
                if (min_distance>distance)
                {
                    min_distance = distance;
                }
            }
        }
    }
    int temp = min_distance;
    min_distance = 999;
    return temp;
}

MainWindow::~MainWindow()
{
}
