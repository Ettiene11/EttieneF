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
    SetupGUI();
}

void MainWindow::SetupGUI()
{
    QVectorIterator<QPushButton*> buttons(GUI);
    while (buttons.hasNext())
    {
        QPushButton* button = buttons.next();
        button->deleteLater();
        button->hide();
    }

    //    set background
    setFixedSize(1000, 700);
    QPixmap bkgnd("C:/Users/User/Documents/NWU/2023/Semester 1/REII 313/Coding/Chess/chess_background.jpg");
    bkgnd = bkgnd.scaled(1000,700, Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);

    //Main menu
    QPushButton* new_button = new QPushButton(this);
    new_button->show();
    new_button->setText("Singleplayer");
    new_button->setGeometry(0,0,100,75);
    new_button->move(350, 300);
    connect(new_button, SIGNAL(clicked()), this, SLOT(singleplayer_clicked()));
    GUI.append(new_button);

    QPushButton* new_button2 = new QPushButton(this);
    new_button2->show();
    new_button2->setText("Multiplayer");
    new_button2->setGeometry(0,0,100,75);
    new_button2->move(500, 300);
    connect(new_button2, SIGNAL(clicked()), this, SLOT(multiplayer_clicked()));
    GUI.append(new_button2);

    name = new QLineEdit(this);
    name->show();
    name->setText("Player 1");
    name->move(450,250);

    lblname = new QLabel(this);
    lblname->setText("Name: ");
    lblname->move(400,250);
    lblname->show();
}

void MainWindow::singleplayer_clicked()
{

}

void MainWindow::multiplayer_clicked()
{
    name->deleteLater();
    name->hide();
    lblname->deleteLater();
    lblname->hide();

    playername = name->text();
    NewGame();
}

void MainWindow::NewGame()
{
    QVectorIterator<QPushButton*> buttons(GUI);
    while (buttons.hasNext())
    {
        QPushButton* button = buttons.next();
        button->deleteLater();
        button->hide();
    }

    //    set background
    setFixedSize(width, height);
    QPixmap bkgnd("C:/Users/User/Documents/NWU/2023/Semester 1/REII 313/Coding/Chess/board.png");
    bkgnd = bkgnd.scaled(width,height, Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);

    DefaultBoard();
}

void MainWindow::ResetGame()
{
    QVectorIterator<piecetracker*> tracker(piece_tracker);
    QVectorIterator<QLabel*> piece(all_pieces);
    while (tracker.hasNext())
    {
        piecetracker* pt = tracker.next();
        QLabel* p = piece.next();

        piece_tracker.removeOne(pt);
        all_pieces.removeOne(p);
        p->hide();
    }
}

void MainWindow::EndGame()
{
    ResetGame();

    //    set background
    setFixedSize(1000, 700);
    QPixmap bkgnd("C:/Users/User/Documents/NWU/2023/Semester 1/REII 313/Coding/Chess/chess_background.jpg");
    bkgnd = bkgnd.scaled(1000,700, Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);

    setFixedSize(1000,700);
    QPushButton* new_button = new QPushButton(this);
    new_button->show();
    new_button->setText("Return to Main menu");
    new_button->setGeometry(0,0,150,75);
    new_button->move(300, 300);
    connect(new_button, SIGNAL(clicked()), this, SLOT(SetupGUI()));
    GUI.append(new_button);

    QPushButton* new_button2 = new QPushButton(this);
    new_button2->show();
    new_button2->setText("Play again");
    new_button2->setGeometry(0,0,150,75);
    new_button2->move(550, 300);
    connect(new_button2, SIGNAL(clicked()), this, SLOT(NewGame()));
    GUI.append(new_button2);
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

                for (int i = 1; i <= 8; ++i)
                {
                    for (int j = 1; j <= 8; ++j)
                    {
                        if ((Validmove(t->type, i, j)) && (Validpiecemove(t->team,t->type,t->num_moves,from_xcoord, from_ycoord,i,j)))
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

//    new_piece->setPixmap(image);
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
            if ((!(Validmove(t->type, to_xcoord, to_ycoord))) || (!(Validpiecemove(t->team,t->type,t->num_moves,from_xcoord, from_ycoord,to_xcoord,to_ycoord))) || Check_yourself(t->team,from_xcoord, from_ycoord,to_xcoord,to_ycoord,t))
            {
                cout << "not valid move" << endl;
            }else{
                    if (capture)
                    {
                        piece_tracker.removeOne(piece_tracker[enemy_index]);
                        all_pieces[enemy_index]->hide();
                        all_pieces.removeOne(all_pieces[enemy_index]);
                    }

                    p->move(board.AssignxCoord(to_xcoord), board.AssignyCoord(to_ycoord));
                    t->x_cor = board.AssignxCoord(to_xcoord);
                    t->y_cor = board.AssignyCoord(to_ycoord);
                    ++t->num_moves;

                    if (kingside_castling)
                    {
                        kingside_castling = false;
                        castling_rook_lbl->move(board.AssignxCoord(to_xcoord-1), board.AssignyCoord(to_ycoord));  //move rook
                        castling_rook_pt->x_cor = board.AssignxCoord(to_xcoord-1);
                        castling_rook_pt->y_cor = board.AssignyCoord(to_ycoord);
                        ++castling_rook_pt->num_moves;

                    }else if (queenside_castling)
                    {
                        queenside_castling = false;
                        castling_rook_lbl->move(board.AssignxCoord(to_xcoord+1), board.AssignyCoord(to_ycoord));  //move rook
                        castling_rook_pt->x_cor = board.AssignxCoord(to_xcoord+1);
                        castling_rook_pt->y_cor = board.AssignyCoord(to_ycoord);
                        ++castling_rook_pt->num_moves;
                    }

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
                           EndGame();
                           ResetGame();
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

bool MainWindow::Validmove(char type, int x, int y)  //check if piece moved into open space or onto enemy team's piece
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
                if ((PossibleCastling(t)) && (type == 'k'))
                {
                    capture = false;
                    return true;
                }else{return false;}
            }
        }
    }
    capture = false;

    return true;  //open space thus valid move
}

bool MainWindow::Validpiecemove(char team, char type, int num_moves, int from_x, int from_y, int x, int y) //check if specific piece has valid move based on its specific characteristical moves
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
        if (En_passant(team,from_x, from_y, x, y)){return true;}
        if (pawn.ValidMove(team, num_moves, capture, from_x, from_y, x, y, vertical_up_boundary, vertical_down_boundary)) {return true;} break;
    case 'n' :
        if (knight.ValidMove(team, from_x, from_y, x, y)) {return true;} break;
    case 'r' :
        if (rook.ValidMove(team, from_x, from_y, x, y, vertical_up_boundary, vertical_down_boundary, horizontal_left_boundary, horizontal_right_boundary)) {return true;} break;
    case 'b' :
        if (bishop.ValidMove(team, from_x, from_y, x, y, right_up_diagonal_boundary, left_up_diagonal_boundary, right_down_diagonal_boundary, left_down_diagonal_boundary)) {return true;} break;
    case 'q' :
        if (queen.ValidMove(team, from_x, from_y, x, y, vertical_up_boundary, vertical_down_boundary, horizontal_left_boundary, horizontal_right_boundary,right_up_diagonal_boundary, left_up_diagonal_boundary, right_down_diagonal_boundary, left_down_diagonal_boundary)) {return true;} break;
    case 'k' :
        if (Castling(num_moves, horizontal_left_boundary, horizontal_right_boundary, x, y)) {return true;}
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
//        QPixmap image("C:/Users/User/Documents/NWU/2023/Semester 1/REII 313/Coding/Chess/board.png");
//        image.scaled(100,100);
        Makepiece("WP",'p','w',board.AssignxCoord(i),board.AssignyCoord(2));
        Makepiece("BP",'p','b',board.AssignxCoord(i),board.AssignyCoord(7));
    }

    //knights (symbol n)
    for (int i = 2; i<=7; i = i+5)
    {
//        QPixmap image("C:/Users/User/Documents/NWU/2023/Semester 1/REII 313/Coding/Chess_final/test.jpg");
        Makepiece("WN",'n','w',board.AssignxCoord(i),board.AssignyCoord(1));
        Makepiece("BN",'n','b',board.AssignxCoord(i),board.AssignyCoord(8));
    }

    //rooks
    for (int i = 1; i<=8; i = i+7)
    {
//        QPixmap image("C:/Users/User/Documents/NWU/2023/Semester 1/REII 313/Coding/Chess_final/test.jpg");
        Makepiece("WR",'r','w',board.AssignxCoord(i),board.AssignyCoord(1));
        Makepiece("BR",'r','b',board.AssignxCoord(i),board.AssignyCoord(8));
    }

    //bishops
    for (int i = 3; i<=6; i = i+3)
    {
//        QPixmap image("C:/Users/User/Documents/NWU/2023/Semester 1/REII 313/Coding/Chess_final/test.jpg");
        Makepiece("WB",'b','w',board.AssignxCoord(i),board.AssignyCoord(1));
        Makepiece("BB",'b','b',board.AssignxCoord(i),board.AssignyCoord(8));
    }

    //queens
//    QPixmap image("C:/Users/User/Documents/NWU/2023/Semester 1/REII 313/Coding/Chess_final/test.jpg");
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
            if (Validpiecemove(t->team,t->type,t->num_moves,GetxPosition(t->x_cor),GetyPosition(t->y_cor),GetxPosition(king_xpos),GetyPosition(king_ypos)))
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
    if (!Validpiecemove(piecetrack->team,piecetrack->type,piecetrack->num_moves,GetxPosition(from_x),GetyPosition(from_y),GetxPosition(king_xpos),GetyPosition(king_ypos)))
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
                    if ((Validmove(pt->type, i, j)) && (Validpiecemove(pt->team,pt->type,pt->num_moves,GetxPosition(pt->x_cor), GetyPosition(pt->y_cor),i,j)))
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

bool MainWindow::En_passant(char team, int from_x, int from_y, int to_x, int to_y)
{
    QVectorIterator<piecetracker*> tracker(piece_tracker);
    while (tracker.hasNext())
    {
        piecetracker* t = tracker.next();
        if ((((GetyPosition(t->y_cor) == 5) && (team == 'w')) || ((GetyPosition(t->y_cor) == 4) && (team == 'b'))) && (t->num_moves == 1))
        {
            if (((GetxPosition(t->x_cor) == to_x) && ((GetyPosition(t->y_cor)-to_y) == 1) && (team == 'b')) ||
                    ((GetxPosition(t->x_cor) == to_x) && (to_y-(GetyPosition(t->y_cor)) == 1) && (team == 'w')))
            {
                if ((abs(from_x-GetxPosition(t->x_cor)) == 1) && (from_y == GetyPosition(t->y_cor)))
                {
                    capture = true;
                    enemy_index = piece_tracker.indexOf(t);
                    return true;
                }
            }
        }
    }
    return false;
}

bool MainWindow::PossibleCastling(piecetracker* pt)
{
    if ((pt->type == 'r') && (pt->num_moves == 0))
    {
        return true;
    }
    return false;
}

bool MainWindow::Castling(int num_moves, int left_bound, int right_bound, int to_x, int to_y)
{
    if (num_moves == 0)
    {
        QVectorIterator<QLabel*> piece(all_pieces);
        QVectorIterator<piecetracker*> tracker(piece_tracker);
        while (tracker.hasNext())
        {
            piecetracker* t = tracker.next();
            QLabel* p = piece.next();
            if ((GetxPosition(t->x_cor) == to_x) && (GetyPosition(t->y_cor) == to_y))
            {
                if ((to_x == 1) && (left_bound == 4) && (t->type == 'r') && (t->team == turn))
                {
                    to_xcoord = to_xcoord + 2;
                    queenside_castling = true;
                    castling_rook_pt = t;
                    castling_rook_lbl = p;
                    return true;
                }else if ((to_x == 8) && (right_bound == 3) && (t->type == 'r') && (t->team == turn))
                {
                    to_xcoord = to_xcoord - 1;
                    kingside_castling = true;
                    castling_rook_pt = t;
                    castling_rook_lbl = p;
                    return true;
                }
            }
        }
    }
    return false;
}

MainWindow::~MainWindow()
{
}
