#include "mainwindow.h"
#include <board.h>
#include <pawn.h>

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

void MainWindow::DefaultBoard()
{
    //white pawns
    board board;
    for (int i = 1; i<=8; ++i)
    {
        QLabel* new_piece = new QLabel(this);
        new_piece->setText("WP");
        new_piece->show();
        new_piece->move(board.AssignxCoord(i), board.AssignyCoord(2));
        all_pieces.append(new_piece);

        piecetracker* new_piecetracker = new piecetracker;
        new_piecetracker->team = 'w';
        new_piecetracker->type = 'p';
        new_piecetracker->x_cor = board.AssignxCoord(i);
        new_piecetracker->y_cor = board.AssignyCoord(2);
        piece_tracker.append(new_piecetracker);
    }

    //black pawns
    for (int i = 1; i<=8; ++i)
    {
        QLabel* new_piece = new QLabel(this);
        new_piece->setText("BP");
        new_piece->show();
        new_piece->move(board.AssignxCoord(i), board.AssignyCoord(7));
        all_pieces.append(new_piece);

        piecetracker* new_piecetracker = new piecetracker;
        new_piecetracker->team = 'b';
        new_piecetracker->type = 'p';
        new_piecetracker->x_cor = board.AssignxCoord(i);
        new_piecetracker->y_cor = board.AssignyCoord(7);
        piece_tracker.append(new_piecetracker);
    }

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
                p->move(board.AssignxCoord(to_xcoord), board.AssignyCoord(to_ycoord));
                t->x_cor = board.AssignxCoord(to_xcoord);
                t->y_cor = board.AssignyCoord(to_ycoord);
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
    QVectorIterator<QLabel*> piece(all_pieces);

    while (tracker.hasNext())
    {
        piecetracker *t = tracker.next();
        QLabel *p = piece.next();

        if ((t->x_cor == board.AssignxCoord(x)) && (t->y_cor == board.AssignyCoord(y)))
        {
            if (t->team != turn)
            {
                capture = true;
                if (click_counter == 0) //to distinguish from possible moves call
                {
                    piece_tracker.removeOne(t);
                    p->hide();
                    all_pieces.removeOne(p);
                }
                return true;
            }else{
                return false;   //moved onto own team's piece, thus invalid move
            }
        }
    }

    capture = false;
    return true;  //open space thus valid move
}

bool MainWindow::Validpiecemove(char team, char type, int x, int y) //check if specific piece has valid move based on its specific characteristical moves
{
    switch (type) {
    case 'p' : {
        Pawn pawn;
        if (pawn.ValidMove(team, firstmove, capture, from_xcoord, from_ycoord, x, y)) {return true;}
    }

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

MainWindow::~MainWindow()
{
}