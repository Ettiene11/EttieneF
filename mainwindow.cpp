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
    singleplayer = false;
    multiplayer = false;
//    if (client)
//        client->deleteLater();

//    if (server)
//        server->deleteLater();
//    QVectorIterator<QPushButton*> buttons(GUI);
//    while (buttons.hasNext())
//    {
//        QPushButton* button = buttons.next();
//        GUI.removeOne(button);
//        button->hide();
//    }

    //    set background
    setFixedSize(1000, 800);
    background = new QLabel(this);
    background->setPixmap(QPixmap(":img/chess_background.jpg").scaled(1000,800));
    background->move(0,0);
    background->setFixedSize(1000,800);
//    QPixmap bkgnd(":img/chess_background.jpg");
//    bkgnd = bkgnd.scaled(1000,800, Qt::IgnoreAspectRatio);
//    QPalette palette;
//    palette.setBrush(QPalette::Background, bkgnd);
//    this->setPalette(palette);

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
    if (playername == nullptr)
    {
      name->setText("Player 1");
    }else{name->setText(playername); }
    name->move(450,250);

    lblname = new QLabel(this);
    lblname->setText("Name: ");
    lblname->move(400,250);
    lblname->show();
}

void MainWindow::singleplayer_clicked()
{
    singleplayer = true;
}

void MainWindow::multiplayer_clicked()
{
    multiplayer = true;
    name->deleteLater();
    name->hide();
    lblname->deleteLater();
    lblname->hide();

    playername = name->text();
//    NewGame();
    NetworkGUI();
}

void MainWindow::NetworkGUI()
{
    server = nullptr;
    client = nullptr;
    input = nullptr;
    output = nullptr;

    setFixedSize(1000, 800);
    background->setPixmap(QPixmap(":img/chess_background.jpg").scaled(1000,800));
    background->move(0,0);
    background->setFixedSize(1000,800);

    //Main menu
    QPushButton* new_button = new QPushButton(this);
    new_button->show();
    new_button->setText("Client");
    new_button->setGeometry(0,0,100,75);
    new_button->move(350, 300);
    connect(new_button, SIGNAL(clicked()), this, SLOT(makeClient()));
    GUI.append(new_button);

    QPushButton* new_button2 = new QPushButton(this);
    new_button2->show();
    new_button2->setText("Server");
    new_button2->setGeometry(0,0,100,75);
    new_button2->move(500, 300);
    connect(new_button2, SIGNAL(clicked()), this, SLOT(makeServer()));
    GUI.append(new_button2);
}

void MainWindow::makeClient()
{
    client = new Client;
    connect(client, SIGNAL(dataReceived(QByteArray)), this, SLOT(receive(QByteArray)));
    if (!client->connectToHost("127.0.0.1"))
    {
        client->deleteLater();
        return;
    }

    client->writeData(playername.toUtf8());
    player_is_client = true;
}

void MainWindow::makeServer()
{
    server = new Server;
    connect(server, SIGNAL(dataReceived(QByteArray)), this, SLOT(receive(QByteArray)));

    cout<<"Waiting for opponent"<<endl;
    player_is_server = true;
}

void MainWindow::serverSend(QByteArray data)
{
    server->writeData(data);
}

void MainWindow::clientSend(QByteArray data)
{
    client->writeData(data);
}

void MainWindow::receive(QByteArray data)
{
    if ((!playing_game) && (!((QString(data) == "endgame") || (QString(data) == "forfeit") || (QString(data) == "draw") || (QString(data) == "nodraw")
                              || (QString(data) == "yesdraw") || (QString(data) == "playagain") || (QString(data) == "playagainyes") || (QString(data) == "playagainno"))))
    {
        if (player_is_server == true)
        {
            QString string = QString(data);
            opponentname = string;
            server->writeData(playername.toUtf8());
            playing_game = true;
            NewGame();
        }else{
            QString string = QString(data);
            opponentname = string;
            playing_game = true;
            NewGame();
        }
    }else{
        if ((QString(data) == "endgame") || (QString(data) == "forfeit") || (QString(data) == "draw") || (QString(data) == "nodraw")
                || (QString(data) == "yesdraw") || (QString(data) == "playagain") || (QString(data) == "playagainyes") || (QString(data) == "playagainno"))
        {
           if (QString(data) == "endgame")
           {
                EndGame();
           }else if (QString(data) == "forfeit")
           {
                EndGame();
           }else if (QString(data) == "draw")
           {
                question = "draw";
                Askopponent();
           }else if (QString(data) == "nodraw")
           {
               status->setText(opponentname + " did not accept draw request. Continue playing.");
               playing_game = true;
           }else if (QString(data) == "yesdraw")
           {
               EndGame();
           }else if ((QString(data) == "playagain"))
           {
               ans = true;
               question = "playagain";
               Askopponent();
           }else if ((QString(data) == "playagainyes"))
           {
                NewGame();
                status->setText(opponentname + " joined for another game.");
           }
           else if ((QString(data) == "playagainno"))
          {
               SetupGUI();
               menustatus->setText(opponentname + " returned to main menu.");
          }
        }else{
            arraytovector(data);
            if (turn == 'w')
            {
                turn = 'b';
            }else{
                turn = 'w';
            }
            QString s = QString(turn);
            status->setText("Game status: " + s + "'s turn to play!");
        }
    }
}

void MainWindow::Askopponent()
{
    btnyes = new QPushButton(this);
    btnyes->setText("Yes");
    btnyes->setFixedSize(50,50);

    btnno = new QPushButton(this);
    btnno->setText("No");
    btnno->setFixedSize(50,50);

    if (question == "draw")
    {
        playing_game = false;
        status->setText(opponentname + " wants to draw, do you accept?");
        btnyes->move(810,270);
        btnyes->show();
        connect(btnyes, SIGNAL(clicked()), this, SLOT(ansyes()));

        btnno->move(870,270);
        btnno->show();
        connect(btnno, SIGNAL(clicked()), this, SLOT(ansno()));
    }
    if (question == "playagain")
    {
        menustatus->setText(opponentname + " wants to play again, do you accept?");
        btnyes->move(400,270);
        btnyes->show();
        connect(btnyes, SIGNAL(clicked()), this, SLOT(ansyes()));

        btnno->move(460,270);
        btnno->show();
        connect(btnno, SIGNAL(clicked()), this, SLOT(ansno()));
    }
}

void MainWindow::ansyes()
{
    if (question == "draw")
    {
        btnyes->hide();
        btnno->hide();
        btnyes->deleteLater();
        btnno->deleteLater();
        EndGame();
        if (player_is_client)
        {
            clientSend("yesdraw");
        }else{serverSend("yesdraw");}
    }else if (question == "playagain"){
        if (player_is_client)
        {
            clientSend("playagainyes");
        }else{serverSend("playagainyes");}
        NewGame();
    }
}

void MainWindow::ansno()
{
    if (question == "draw")
    {
        playing_game = true;
        btnyes->hide();
        btnno->hide();
        btnyes->deleteLater();
        btnno->deleteLater();
        if (player_is_client)
        {
            clientSend("nodraw");
        }else{serverSend("nodraw");}
    }else if (question == "playagain"){
        if (player_is_client)
        {
            clientSend("playagainno");
        }else{serverSend("playagainno");}
        SetupGUI();
    }
}

void MainWindow::arraytovector(QByteArray array)
{
    board board;
    QString string, filename;
    string = "";
    string = QString(array);
    int num_pieces, h;
    h = string.size();
    num_pieces = h/5;

    QVectorIterator<QLabel*> pieces(all_pieces);
    QVectorIterator<piecetracker*> tracker(piece_tracker);
    while (tracker.hasNext())
    {
        QLabel* p = pieces.next();
        piecetracker* t = tracker.next();
        p->hide();
        all_pieces.removeOne(p);
        piece_tracker.removeOne(t);
    }

    for (int i = 0; i < num_pieces; ++i)
    {
        piecetracker* pt = new piecetracker;
        pt->team = string[(i*5)].toLatin1();
        pt->type = string[(i*5)+1].toLatin1();
        pt->x_cor = board.AssignxCoord(string[(i*5)+2].digitValue());
        pt->y_cor = board.AssignyCoord(string[(i*5)+3].digitValue());
        pt->num_moves = string[(i*5)+4].digitValue();

        if (pt->team == 'w')
        {
            switch (pt->type){
            case 'p' :{filename = "w_pawn.png";break;}
            case 'r' :{filename = "w_rook.png";break;}
            case 'n' :{filename = "w_knight.png";break;}
            case 'b' :{filename = "w_bishop.png";break;}
            case 'q' :{filename = "w_queen.png";break;}
            case 'k' :{filename = "w_king.png";break;}
            }
        }else{
            switch (pt->type){
            case 'p' :{filename = "b_pawn.png";break;}
            case 'r' :{filename = "b_rook.png";break;}
            case 'n' :{filename = "b_knight.png";break;}
            case 'b' :{filename = "b_bishop.png";break;}
            case 'q' :{filename = "b_queen.png";break;}
            case 'k' :{filename = "b_king.png";break;}
            }
        }

        Makepiece(filename,pt->type,pt->team,pt->x_cor,pt->y_cor,pt->num_moves);
    }
}

QByteArray MainWindow::stringtoarray()
{
    QString string = "";
    QVectorIterator<piecetracker*> tracker(piece_tracker);
    while (tracker.hasNext())
    {
        piecetracker* t = tracker.next();
        string = string + t->team+t->type+QString::number(GetxPosition(t->x_cor))+QString::number(GetyPosition(t->y_cor))+QString::number(t->num_moves);  //eg. wp110
    }
    return string.toUtf8();
}

void MainWindow::NewGame()
{
//    QVectorIterator<QPushButton*> buttons(GUI);
//    while (buttons.hasNext())
//    {
//        QPushButton* button = buttons.next();
//        button->hide();
//        button->deleteLater();
//    }
    playing_game = true;
    //    set background
    background->setPixmap(QPixmap(":img/board.png").scaled(width,height));
    background->setFixedSize(width+200, height);
    background->show();

    new_frame = new QLabel(this);
    new_frame->setPixmap(QPixmap(":img/frame.png").scaled(200,800));
    new_frame->setFixedSize(200, 940);
    new_frame->move(width,-70);
    new_frame->show();

    if (player_is_client)
    {
        team = 'w';
    }else{
        team = 'b';
    }

    welc_message = new QLabel(this);
    welc_message->setText("Welcome, " + playername + "!\nYou are playing " + opponentname
                          + "\nYou are team " + team + ".\n\nGoodluck!!");
    welc_message->move(840,115);
    welc_message->setFixedWidth(999);
    welc_message->setFixedHeight(100);
    welc_message->show();

    QString s = QString(turn);
    status = new QLabel(this);
    status->setText("Game status: " + s + "'s turn to play!");
    status->setFixedWidth(999);
    status->move(815,255);
    status->show();

    QString title;
    if (multiplayer)
    {
        QPushButton* btnForfeit = new QPushButton(this);
        btnForfeit->show();
        btnForfeit->setText("Forfeit");
        btnForfeit->setGeometry(0,0,100,75);
        btnForfeit->move(825, 500);
        connect(btnForfeit, SIGNAL(clicked()), this, SLOT(Forfeit()));//"forfeit"
        GUI.append(btnForfeit);

        QPushButton* btnDraw = new QPushButton(this);
        btnDraw->show();
        btnDraw->setText("Draw");
        btnDraw->setGeometry(0,0,100,75);
        btnDraw->move(825, 600);
        connect(btnDraw, SIGNAL(clicked()), this, SLOT(Draw()));
        GUI.append(btnDraw);
    }else if (singleplayer)
    {
        QPushButton* btnReset = new QPushButton(this);
        btnReset->show();
        btnReset->setText("Reset");
        btnReset->setGeometry(0,0,100,75);
        btnReset->move(825, 500);
        connect(btnReset, SIGNAL(clicked()), this, SLOT(ResetGame()));
        GUI.append(btnReset);

        QPushButton* btnQuit = new QPushButton(this);
        btnQuit->show();
        btnQuit->setText("Quit");
        btnQuit->setGeometry(0,0,100,75);
        btnQuit->move(825, 600);
        connect(btnQuit, SIGNAL(clicked()), this, SLOT(Endgame()));
        GUI.append(btnQuit);
    }

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

    status->hide();
    status->deleteLater();
    welc_message->hide();
    welc_message->deleteLater();
    new_frame->hide();
    new_frame->deleteLater();
    turn = 'w';
}

void MainWindow::EndGame()
{
    playing_game = false;
    ResetGame();

//    QVectorIterator<QPushButton*> buttons(GUI);
//    while (buttons.hasNext())
//    {
//        QPushButton* button = buttons.next();
//        button->hide();
//        button->deleteLater();
//    }

    //    set background
    setFixedSize(1000, 800);
    background->setPixmap(QPixmap(":img/chess_background.jpg").scaled(1000,800));
    background->move(0,0);
    background->setFixedSize(1000,800);
//    QPixmap bkgnd(":img/chess_background.jpg");
//    bkgnd = bkgnd.scaled(1000,700, Qt::IgnoreAspectRatio);
//    QPalette palette;
//    palette.setBrush(QPalette::Background, bkgnd);
//    this->setPalette(palette);

    btnreturntomain = new QPushButton(this);
    btnreturntomain->show();
    btnreturntomain->setText("Return to Main menu");
    btnreturntomain->setGeometry(0,0,150,75);
    btnreturntomain->move(300, 300);
    connect(btnreturntomain, SIGNAL(clicked()), this, SLOT(SetupGUI()));
//    GUI.append(btnreturntomain);

    btnplayagain = new QPushButton(this);
    btnplayagain->show();
    btnplayagain->setText("Play again");
    btnplayagain->setGeometry(0,0,150,75);
    btnplayagain->move(550, 300);
    connect(btnplayagain, SIGNAL(clicked()), this, SLOT(Playagain()));
//    GUI.append(btnplayagain);

    menustatus = new QLabel(this);
    menustatus->setGeometry(0,0,200,50);
    menustatus->move(450, 200);
    menustatus->show();
    if (((serv_won) && (player_is_server)) || ((client_won) && (player_is_client)))
    {
       menustatus->setText("Congratulations, you won!");       //update and show leaderboard
    }else{
       menustatus->setText(opponentname+ ", won! Good luck next time.");            //update and show leaderboard
    }

}

void MainWindow::Playagain()
{
    QString str = "playagain";
    if (player_is_client)
    {
        clientSend(str.toUtf8());
    }else{serverSend(str.toUtf8());}
    menustatus->setText("Asking " + opponentname + " to play again.");
}

void MainWindow::Forfeit()
{
    QString str = "forfeit";
    if (player_is_client)
    {
        clientSend(str.toUtf8());
    }else{serverSend(str.toUtf8());}
    EndGame();
}

void MainWindow::Draw()
{
    playing_game = false;
    status->setText(playername + " wants to draw, waiting for opponents response?");
    QString str = "draw";
    if (player_is_client)
    {
        clientSend(str.toUtf8());
    }else{serverSend(str.toUtf8());}
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    if (playing_game)
    {
        ++click_counter;
        if (turn != team)
        {
            cout << "not your turn" << endl;
            click_counter = 0;
        }else{
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
                            cout << "not your team" << endl;
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
                                        new_move->setPixmap(QPixmap(":img/possible_move.png").scaled(100,100));
                                        new_move->setFixedSize(100, 100);
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

void MainWindow::Makepiece(QString text, char type, char team, int x, int y, int moves)
{
    QLabel* new_piece = new QLabel(this);
    new_piece->setPixmap(QPixmap(":img/"+text).scaled(100,100));
    new_piece->setFixedSize(100, 100);
    new_piece->show();
    new_piece->move(x, y);
    all_pieces.append(new_piece);

    piecetracker* new_piecetracker = new piecetracker;
    new_piecetracker->team = team;
    new_piecetracker->type = type;
    new_piecetracker->x_cor = x;
    new_piecetracker->y_cor = y;
    new_piecetracker->num_moves = moves;
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
                    if (t->num_moves<5)   //we are only interested in moves 0-2, important that it stays 1 char for transfer
                    {++t->num_moves;}

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
                           if (player_is_client)
                           {
                               client_won = true;
                               clientSend("endgame");
                           }else{serverSend("endgame"); serv_won = true;}
                           EndGame();
                        }else{
                           cout << turn << "'s king is in check, no checkmate!" << endl;
                        }
                    }

                    QString s = QString(turn);
                    status->setText("Game status: " + s + "'s turn to play!");
                    if (player_is_client)
                    {
                        clientSend(stringtoarray());
                    }else{
                        serverSend(stringtoarray());
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
        Makepiece("w_pawn.png",'p','w',board.AssignxCoord(i),board.AssignyCoord(2),0);
        Makepiece("b_pawn.png",'p','b',board.AssignxCoord(i),board.AssignyCoord(7),0);
    }

    //knights (symbol n)
    for (int i = 2; i<=7; i = i+5)
    {
        Makepiece("w_knight.png",'n','w',board.AssignxCoord(i),board.AssignyCoord(1),0);
        Makepiece("b_knight.png",'n','b',board.AssignxCoord(i),board.AssignyCoord(8),0);
    }

    //rooks
    for (int i = 1; i<=8; i = i+7)
    {
        Makepiece("w_rook.png",'r','w',board.AssignxCoord(i),board.AssignyCoord(1),0);
        Makepiece("b_rook.png",'r','b',board.AssignxCoord(i),board.AssignyCoord(8),0);
    }

    //bishops
    for (int i = 3; i<=6; i = i+3)
    {
        Makepiece("w_bishop.png",'b','w',board.AssignxCoord(i),board.AssignyCoord(1),0);
        Makepiece("b_bishop.png",'b','b',board.AssignxCoord(i),board.AssignyCoord(8),0);
    }

    //queens
    Makepiece("w_queen.png",'q','w',board.AssignxCoord(4),board.AssignyCoord(1),0);
    Makepiece("b_queen.png",'q','b',board.AssignxCoord(4),board.AssignyCoord(8),0);

    //kings
    Makepiece("w_king.png",'k','w',board.AssignxCoord(5),board.AssignyCoord(1),0);
    Makepiece("b_king.png",'k','b',board.AssignxCoord(5),board.AssignyCoord(8),0);
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

    piecetrack->x_cor = board.AssignxCoord(to_x);   //move temporarily
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
        if ((capture == true) && (t == piece_tracker[enemy_index]))
        {
            check_opponent = false;
        }else{
            if (t->team != team)
            {
    //            capture = true;
                if (Validpiecemove(t->team,t->type,t->num_moves,GetxPosition(t->x_cor),GetyPosition(t->y_cor),GetxPosition(king_xpos),GetyPosition(king_ypos)))
                {
                    check_opponent = true;
                    break;
                }else{
                    check_opponent = false;
                }
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
//    if (client)
//        client->deleteLater();

//    if (server)
//        server->deleteLater();
}
