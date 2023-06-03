#include "mainwindow.h"
#include <board.h>
#include <pawn.h>
#include <knight.h>
#include <rook.h>
#include <bishop.h>
#include <queen.h>
#include <king.h>
#include <client.h>
#include <server.h>
#include <highscore.h>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    SetupGUI();
}

void MainWindow::SetupGUI()
{
    if (multiplayer == true)
    {
        menustatus->hide();
        scoredisplay->hide();
        if (player_is_client)
        {
            client->deleteLater();
        }else if (player_is_server){
            QString str = "serverleft";
            serverSend(str.toUtf8());
        }
    }
    if (player_is_server)
    {
        serverSend("serverleft");
    }
    singleplayer = false;
    multiplayer = false;
    player_is_client = false;
    player_is_server = false;

    QVectorIterator<QPushButton*> buttons(GUI);
    while (buttons.hasNext())
    {
        QPushButton* button = buttons.next();
        GUI.removeOne(button);
        button->hide();
    }

    //    set background
    setFixedSize(1000, 800);
    background = new QLabel(this);
    background->setPixmap(QPixmap(":img/chess_background.jpg").scaled(1000,800));
    background->move(0,0);
    background->setFixedSize(1000,800);

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

    menustatus = new QLabel(this);
    menustatus->setGeometry(0,0,200,50);

    for (int k=0; k<=63 ; ++k)
    {
        possiblexmoves[k] = 0;
        possibleymoves[k] = 0;
    }
}

void MainWindow::singleplayer_clicked()
{
    singleplayer = true;
    name->deleteLater();
    name->hide();
    lblname->deleteLater();
    lblname->hide();

    playername = name->text();
    opponentname = "The computer";
    team = 'w';

    //startup engine
    connector = new StockfishConnector;
    connect(connector, SIGNAL(dataReceived(QByteArray)), this, SLOT(AIreceive(QByteArray)));

    timer = new QTimer(this);
    timer->setInterval(10);
    connect(timer, SIGNAL(timeout()), this, SLOT(moveAIpiece()));

    NewGame();
}

void MainWindow::AIreceive(QByteArray data)
{
    if (askbestmove)
    {
        status->setText("Your best move is " + data + ", " + QString(10)  + QString::number(hints) + " hints left.");
        askbestmove = false;
    }else{
        counter = 0;
        chessposition = data;
        if (chessposition == "none")
        {
            gamestatus->setText("Checkmate, Computer won!");
            EndGame();
            menustatus->setText("Game over, Computer won!");
        }
        gamestatus->show();
        board board;
        //move piece
        if (data != "none")
        {
           convertChessPosition(chessposition);
        }

        QVectorIterator<piecetracker*> tracker2(piece_tracker);
        QVectorIterator<QLabel*> piece2(all_pieces);

        while (tracker2.hasNext())
        {
            piecetracker *t2 = tracker2.next();
            QLabel *p2 = piece2.next();
            if ((board.AssignxCoord(AIfrom_x) == t2->x_cor) && (board.AssignyCoord(AIfrom_y) == t2->y_cor))
            {
                xdelta = (abs(board.AssignxCoord(AIfrom_x)-board.AssignxCoord(AIto_x)))/100;
                ydelta =(abs(board.AssignyCoord(AIfrom_y)-board.AssignyCoord(AIto_y)))/100;
                newx = board.AssignxCoord(AIfrom_x);
                newy = board.AssignyCoord(AIfrom_y);
                if (t2->num_moves<9)   //we are only interested in moves 0-2, important that it stays 1 char for transfer
                {++t2->num_moves;}
            }
        }

        timer->start();
        allmoves = allmoves + " " + chessposition;
        qDebug() << data;
    }
}

void MainWindow::moveAIpiece()
{
    board board;
    ++counter;
    QVectorIterator<piecetracker*> tracker2(piece_tracker);
    QVectorIterator<QLabel*> piece2(all_pieces);

    while (tracker2.hasNext())
    {
        piecetracker *t2 = tracker2.next();
        QLabel *p2 = piece2.next();
        if (counter == 100)
        {
            if (t2->team == 'w')
            {
                if ((board.AssignxCoord(AIto_x) == t2->x_cor) && (board.AssignyCoord(AIto_y) == t2->y_cor))
                {
                    piece_tracker.removeOne(t2);
                    p2->hide();
                    all_pieces.removeOne(p2);
                    player.setMedia(QUrl::fromLocalFile("capture.wav"));
                    player.play();
                    timer->stop();
                    break;
                }
            }
        }
        if ((board.AssignxCoord(AIfrom_x) == t2->x_cor) && (board.AssignyCoord(AIfrom_y) == t2->y_cor))
        {
            if (board.AssignyCoord(AIfrom_y) < board.AssignyCoord(AIto_y))
            {
                newy = newy + ydelta;
            }else{
                newy = newy - ydelta;
            }
            if (board.AssignxCoord(AIfrom_x) < board.AssignxCoord(AIto_x))
            {
                newx = newx + xdelta;
            }else{
                newx = newx - xdelta;
            }
           p2->move(newx, newy);
           if (counter == 100)
           {
               timer->stop();
               t2->x_cor = board.AssignxCoord(AIto_x);
               t2->y_cor = board.AssignyCoord(AIto_y);
               player.setMedia(QUrl::fromLocalFile("move-self.wav"));
               player.play();
           }
        }
    }
}

int MainWindow::convertLetterToNumber(char letter)
{
    return letter - 'a' + 1;
}

void MainWindow::convertChessPosition(const QString position)
{
    AIfrom_x = convertLetterToNumber(position[0].toLatin1());
    AIfrom_y = position[1].toLatin1() - '0';
    AIto_x = convertLetterToNumber(position[2].toLatin1());
    AIto_y = position[3].toLatin1() - '0';

//    allmoves = allmoves + " " + position;
//    command = "position startpos moves " + allmoves;

//    connector.writeCommand(command + "\ngo");
}

void MainWindow::multiplayer_clicked()
{
    multiplayer = true;
    name->deleteLater();
    name->hide();
    lblname->deleteLater();
    lblname->hide();

    QFile scoresFile("C:/Users/User/Documents/NWU/2023/Semester 1/REII 313/Coding/Chess_final/textfiles/scores.txt");
    if (scoresFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream stream(&scoresFile);
        QString line = stream.readAll();
        scoredisplay = new QLabel(this);
        scoredisplay->setText(line);
        scoredisplay->move(450,600);
        scoredisplay->setGeometry(425,300,500,500);
        scoredisplay->show();
    }
    scoresFile.close();

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

    QPushButton* new_button3 = new QPushButton(this);
    new_button3->show();
    new_button3->setText("Back to main menu");
    new_button3->setGeometry(0,0,150,30);
    new_button3->move(1, 1);
    connect(new_button3, SIGNAL(clicked()), this, SLOT(SetupGUI()));
    GUI.append(new_button3);
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
    menustatus = new QLabel(this);
}

void MainWindow::makeServer()
{
    server = new Server;
    connect(server, SIGNAL(dataReceived(QByteArray)), this, SLOT(receive(QByteArray)));

    menustatus->setText("Waiting for opponent...");
    menustatus->setGeometry(0,0,4000,50);
    menustatus->move(400,200);
    menustatus->show();
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
                              || (QString(data) == "yesdraw") || (QString(data) == "playagain") || (QString(data) == "playagainyes") || (QString(data) == "playagainno")
                              || (QString(data) == "serverleft"))))
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
                || (QString(data) == "yesdraw") || (QString(data) == "playagain") || (QString(data) == "playagainyes") || (QString(data) == "playagainno")
                || (QString(data) == "serverleft"))
        {
           if (QString(data) == "endgame")
           {
                EndGame();
                menustatus->setText(opponentname + " has won! Goodluck next time!");
           }else if (QString(data) == "forfeit")
           {
                EndGame();
                menustatus->setText(opponentname + " has forfeited! You won!");          //show leaderboard and update score
           }else if (QString(data) == "draw")
           {
                question = "draw";
                Askopponent();
           }else if (QString(data) == "nodraw")
           {
               status->setText(opponentname + " did not accept\n draw request. Continue playing.");
               playing_game = true;
           }else if (QString(data) == "yesdraw")
           {
               EndGame();
               menustatus->setText("Close one! It's a draw.");
           }else if ((QString(data) == "playagain"))
           {
               ans = true;
               question = "playagain";
               Askopponent();
           }else if ((QString(data) == "playagainyes"))
           {
                NewGame();
                status->setText(opponentname + " joined for another game.");
           }else if ((QString(data) == "playagainno"))
          {
               SetupGUI();
               menustatus->setText(opponentname + " returned to main menu.");
          }else if ((QString(data) == "serverleft"))
           {
                SetupGUI();
                menustatus->setText("Server has left the lobby.");
                server->deleteLater();
                server->disconnected();                              //bug
                client->deleteLater();
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
            status->setText("Game status:\n" + s + "'s turn to play!");
        }
    }
}

void MainWindow::Askopponent()
{
    btnyes = new QPushButton(this);
    btnyes->setText("Yes");
    btnyes->setFixedSize(50,50);
    GUI.append(btnyes);

    btnno = new QPushButton(this);
    btnno->setText("No");
    btnno->setFixedSize(50,50);
    GUI.append(btnno);

    if (question == "draw")
    {
        playing_game = false;
        status->setText(opponentname + " wants to draw,\ndo you accept?");
        status->move(815,250);
        btnyes->move(830,300);
        btnyes->show();
        connect(btnyes, SIGNAL(clicked()), this, SLOT(ansyes()));

        btnno->move(895,300);
        btnno->show();
        connect(btnno, SIGNAL(clicked()), this, SLOT(ansno()));
    }
    if (question == "playagain")
    {
        menustatus->setText(opponentname + " wants to play again, do you accept?");
        btnyes->move(445,250);
        btnyes->show();
        connect(btnyes, SIGNAL(clicked()), this, SLOT(ansyes()));

        btnno->move(505,250);
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
        menustatus->setText("Close one! It's a draw.");
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
    QVectorIterator<QPushButton*> buttons(GUI);
    while (buttons.hasNext())
    {
        QPushButton* button = buttons.next();
        GUI.removeOne(button);
        button->hide();
    }

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

    if (multiplayer)         //for singleplayer, player is always white
    {
        if (player_is_client)
        {
            team = 'w';
        }else{
            team = 'b';
        }
    }

    welc_message = new QLabel(this);
    welc_message->setText("Welcome, " + playername + "!\n\n\nYou are playing\n" + QString(2) +  " " + opponentname + " " + QString(2) +
                          + "\nYou are team " + team + ".\n\nGoodluck!!");
    welc_message->move(850,115);
    welc_message->setFixedWidth(999);
    welc_message->setFixedHeight(100);
    welc_message->show();

    menustatus->hide();
//    menustatus->deleteLater();
    if (multiplayer)
    {
        scoredisplay->hide();
    }

    QString s = QString(turn);
    status = new QLabel(this);
    status->setText("Game status:\n" + s + "'s turn to play!");
    status->setFixedWidth(999);
    status->move(855,255);
    status->show();

    gamestatus = new QLabel(this);
    gamestatus->setGeometry(100,100,200,100);
    gamestatus->move(835,300);
    gamestatus->show();

    QString title;
    if (multiplayer)
    {
        QPushButton* btnForfeit = new QPushButton(this);
        btnForfeit->show();
        btnForfeit->setText("Forfeit");
        btnForfeit->setGeometry(0,0,100,75);
        btnForfeit->move(845, 500);
        connect(btnForfeit, SIGNAL(clicked()), this, SLOT(Forfeit()));
        GUI.append(btnForfeit);

        QPushButton* btnDraw = new QPushButton(this);
        btnDraw->show();
        btnDraw->setText("Draw");
        btnDraw->setGeometry(0,0,100,75);
        btnDraw->move(845, 600);
        connect(btnDraw, SIGNAL(clicked()), this, SLOT(Draw()));
        GUI.append(btnDraw);
    }else if (singleplayer)
    {
        QPushButton* btnReset = new QPushButton(this);
        btnReset->show();
        btnReset->setText(QString(16) + " Reset");
        btnReset->setGeometry(0,0,100,75);
        btnReset->move(845, 500);
        connect(btnReset, SIGNAL(clicked()), this, SLOT(ResetAI()));
        GUI.append(btnReset);

        QPushButton* btnQuit = new QPushButton(this);
        btnQuit->show();
        btnQuit->setText(QString(17) + " Quit");
        btnQuit->setGeometry(0,0,100,75);
        btnQuit->move(845, 600);
        connect(btnQuit, SIGNAL(clicked()), this, SLOT(EndGame()));
        GUI.append(btnQuit);

        QPushButton* btnbestmove = new QPushButton(this);
        btnbestmove->show();
        btnbestmove->setText(QString(3) + " Hint");  //3 is heart
        btnbestmove->setGeometry(0,0,100,75);
        btnbestmove->move(845, 400);
        connect(btnbestmove, SIGNAL(clicked()), this, SLOT(Calcplayerbestmove()));
        GUI.append(btnbestmove);
    }

    DefaultBoard();
}

void MainWindow::Calcplayerbestmove()
{
    if (hints != 0)
    {
        askbestmove = true;
        command = "position startpos moves " + allmoves + "\ngo\n";
        connector->writeData(command.toUtf8());
        --hints;
    }else{
        status->setText("No hints left");
        player.setMedia(QUrl::fromLocalFile("notify.wav"));
        player.play();
    }

}

void MainWindow::ResetAI()
{
    EndGame();
    NewGame();
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
    gamestatus->hide();
    gamestatus->deleteLater();
    turn = 'w';
    chessposition.clear();
    allmoves.clear();
}

void MainWindow::EndGame()
{
    if (multiplayer)
    {
        QFile scoresFile("C:/Users/User/Documents/NWU/2023/Semester 1/REII 313/Coding/Chess_final/textfiles/scores.txt");
        if (scoresFile.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream stream(&scoresFile);
            QString line = stream.readAll();
            scoredisplay->setText(line);
            scoredisplay->move(450,600);
            scoredisplay->setGeometry(425,300,500,500);
            scoredisplay->show();
        }
        scoresFile.close();
    }
    playing_game = false;
    ResetGame();

    QVectorIterator<QPushButton*> buttons(GUI);
    while (buttons.hasNext())
    {
        QPushButton* button = buttons.next();
        GUI.removeOne(button);
        button->hide();
    }

//    menustatus->hide();
    //    set background
    setFixedSize(1000, 800);
    background->setPixmap(QPixmap(":img/chess_background.jpg").scaled(1000,800));
    background->move(0,0);
    background->setFixedSize(1000,800);

    btnreturntomain = new QPushButton(this);
    btnreturntomain->show();
    btnreturntomain->setText("Return to Main menu");
    btnreturntomain->setGeometry(0,0,150,80);
    btnreturntomain->move(300, 300);
    connect(btnreturntomain, SIGNAL(clicked()), this, SLOT(SetupGUI()));
    GUI.append(btnreturntomain);

    btnplayagain = new QPushButton(this);
    btnplayagain->show();
    btnplayagain->setText("Play again");
    btnplayagain->setGeometry(0,0,150,75);
    btnplayagain->move(550, 300);
    connect(btnplayagain, SIGNAL(clicked()), this, SLOT(Playagain()));
    GUI.append(btnplayagain);

//    menustatus = new QLabel(this);
    menustatus->setGeometry(0,0,4000,50);
    menustatus->move(430, 200);
    menustatus->show();
    if (((serv_won) && (player_is_server)) || ((client_won) && (player_is_client)))
    {
       menustatus->setText("Congratulations, you won!");       //update and show leaderboard
    }else if (((client_won) && (player_is_server)) || ((serv_won) && (player_is_client))){
       menustatus->setText(opponentname+ ", won! Good luck next time.");            //update and show leaderboard
    }
}

void MainWindow::Playagain()
{
    if (multiplayer)
    {
        QString str = "playagain";
        if (player_is_client)
        {
            clientSend(str.toUtf8());
        }else{serverSend(str.toUtf8());}
        menustatus->setText("Asking " + opponentname + " to play again.");
    }else{
        NewGame();
        chessposition.clear();
        allmoves.clear();
    }

}

void MainWindow::Forfeit()
{
    QString str = "forfeit";
    if (player_is_client)
    {
        clientSend(str.toUtf8());
    }else{serverSend(str.toUtf8());}
    EndGame();
    menustatus->setText("You forfeited the game.");
}

void MainWindow::Draw()
{
    playing_game = false;
    status->setText(playername + " wants to draw,\nwaiting for opponents response?");
    status->move(815,300);
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
            gamestatus->setText("not your turn");
            player.setMedia(QUrl::fromLocalFile("notify.wav"));
            player.play();
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
                            gamestatus->setText("not your team");
                            player.setMedia(QUrl::fromLocalFile("notify.wav"));
                            player.play();
                            click_counter = 0;
                            break;
                        }

//                        for (int i = 1; i <= 8; ++i)
//                        {
//                            for (int j = 1; j <= 8; ++j)
//                            {
//                                if ((Validmove(t->type, i, j)) && (Validpiecemove(t->team,t->type,t->num_moves,from_xcoord, from_ycoord,i,j)))
//                                {
//                                    if (!Check_yourself(t->team,from_xcoord, from_ycoord,i,j,t))
//                                    {
//                                        QLabel* new_move = new QLabel(this);
//                                        new_move->setPixmap(QPixmap(":img/possible_move.png").scaled(100,100));
//                                        new_move->setFixedSize(100, 100);
//                                        new_move->show();
//                                        new_move->move(board.AssignxCoord(i), board.AssignyCoord(j));
//                                        possible_moves.append(new_move);
//                                    }
//                                }
//                            }
//                        }
                        for (int k=0; k<=63 ; ++k)
                        {
                            possiblexmoves[k] = 0;
                            possibleymoves[k] = 0;
                            ogpossiblexmoves[k] = 0;
                            ogpossibleymoves[k] = 0;
                        }

                        Getboundaries(t->team,from_xcoord,from_ycoord);
                        if (t->type == 'p')
                        {
                            Calcpawnpossiblemoves(t->team, t->num_moves, from_xcoord, from_ycoord,
                                                   vertical_up_boundary, vertical_down_boundary, right_up_diagonal_boundary, left_up_diagonal_boundary,
                                                   right_down_diagonal_boundary, left_down_diagonal_boundary);
                        }
                        if (t->type == 'n')
                        {
                            Calcknightpossiblemoves(from_xcoord,from_ycoord);
                        }
                        if (t->type == 'r')
                        {
                            Calcrookpossiblemoves(from_xcoord, from_ycoord, vertical_up_boundary, vertical_down_boundary, horizontal_left_boundary, horizontal_right_boundary);
                        }
                        if (t->type == 'b')
                        {
                            Calcbishoppossiblemoves(from_xcoord,from_ycoord,right_up_diagonal_boundary,left_up_diagonal_boundary, right_down_diagonal_boundary, left_down_diagonal_boundary);
                        }
                        if (t->type == 'q')
                        {
                            Calcqueenpossiblemoves(from_xcoord, from_ycoord, vertical_up_boundary, vertical_down_boundary, horizontal_left_boundary, horizontal_right_boundary,right_up_diagonal_boundary,left_up_diagonal_boundary, right_down_diagonal_boundary, left_down_diagonal_boundary);
                        }
                        if (t->type == 'k')
                        {
                            Calckingpossiblemoves(t->num_moves, from_xcoord, from_ycoord, vertical_up_boundary, vertical_down_boundary, horizontal_left_boundary, horizontal_right_boundary,right_up_diagonal_boundary,left_up_diagonal_boundary, right_down_diagonal_boundary, left_down_diagonal_boundary);
                        }
                        //all other pieces here
                        for (int i = 0; i<= 63; ++i)
                        {
                            ogpossiblexmoves[i] = possiblexmoves[i];
                            ogpossibleymoves[i] = possibleymoves[i];
                        }
                        int counter = 0;
                        for (int i = 0; i <= 63; ++i)
                        {
                            if (ogpossiblexmoves[i] == 0)
                            {
                                break;
                            }else{
                                if ((!Check_yourself(t->team,from_xcoord, from_ycoord,ogpossiblexmoves[i],ogpossibleymoves[i],t)) && (Validmove(t->type,ogpossiblexmoves[i],ogpossibleymoves[i])))
                                {
                                    ++counter;
                                    QLabel* new_move = new QLabel(this);
                                    new_move->setPixmap(QPixmap(":img/possible_move.png").scaled(100,100));
                                    new_move->setFixedSize(100, 100);
                                    new_move->show();
                                    new_move->move(board.AssignxCoord(ogpossiblexmoves[i]), board.AssignyCoord(ogpossibleymoves[i]));
                                    possible_moves.append(new_move);
                                }
                            }
                        }

                        if (counter == 0)      //can use possible_moves.isemty()
                        {
                            gamestatus->setText("This piece cannot move");
                            player.setMedia(QUrl::fromLocalFile("notify.wav"));
                            player.play();
                            click_counter = 0;
                        }
                    }
                }
                if (!clicked_on_piece)
                {
                    gamestatus->setText("You did not click on piece");
                    player.setMedia(QUrl::fromLocalFile("notify.wav"));
                    player.play();
                    click_counter = 0;
                }else{
                    clicked_on_piece = false;
                }
            }

            if (click_counter == 2)
            {
                board board;
                to_xcoord = GetxPosition(e->x());
                to_ycoord = GetyPosition(e->y());
                click_counter = 0;

                if (!Clicked_on_Piece(from_xcoord,from_ycoord))
                {
                    gamestatus->setText("You did not click on piece");
                    player.setMedia(QUrl::fromLocalFile("notify.wav"));
                    player.play();
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
                gamestatus->setText("not valid move");
                player.setMedia(QUrl::fromLocalFile("notify.wav"));
                player.play();
            }else{
                    if (capture)
                    {
                        piece_tracker.removeOne(piece_tracker[enemy_index]);
                        all_pieces[enemy_index]->hide();
                        all_pieces.removeOne(all_pieces[enemy_index]);
                        //sound capture
                        player.setMedia(QUrl::fromLocalFile("capture.wav"));
                        player.play();
                    }else{
                        player.setMedia(QUrl::fromLocalFile("move-self.wav"));
                        player.play();
                    }

                    p->move(board.AssignxCoord(to_xcoord), board.AssignyCoord(to_ycoord));
                    t->x_cor = board.AssignxCoord(to_xcoord);
                    t->y_cor = board.AssignyCoord(to_ycoord);
                    if (t->num_moves<9)   //we are only interested in moves 0-2, important that it stays 1 char for transfer
                    {++t->num_moves;}                   

                    if ((kingside_castling) && (t->x_cor == board.AssignxCoord(7)))
                    {
                        kingside_castling = false;
                        castling_rook_lbl->move(board.AssignxCoord(to_xcoord-1), board.AssignyCoord(to_ycoord));  //move rook
                        castling_rook_pt->x_cor = board.AssignxCoord(to_xcoord-1);
                        castling_rook_pt->y_cor = board.AssignyCoord(to_ycoord);
                        ++castling_rook_pt->num_moves;

                    }else if ((queenside_castling) && (t->x_cor == board.AssignxCoord(3)))
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
                            p->setPixmap(QPixmap(":img/w_queen").scaled(100,100));
                            p->show();
                        }else{
                            p->setPixmap(QPixmap(":img/b_queen").scaled(100,100));
                            p->show();
                        }
                        t->type = 'q';
                    }

                    if (multiplayer)
                    {
                        if (turn == 'w')
                        {
                            turn = 'b';
                        }else{
                            turn = 'w';
                        }
                    }

                    if (Check_opponent(t->x_cor, t->y_cor, t))
                    {
                        gamestatus->setText(QString(turn) + "'s king is in check.\nChecking for possible checkmate...");
                        gamestatus->show();
                        if (Checkmate(t->team))  //team = your team, check if enemy is in checkmate
                        {
                        gamestatus->setText("Checkmate, " + QString(t->team) + " won!");
                        gamestatus->show();
                           if (player_is_client)
                           {
                               client_won = true;
                               UpdateLeaderboard();
                               clientSend("endgame");
                           }else{
                                serv_won = true;
                                UpdateLeaderboard();
                                serverSend("endgame");
                           }
                            EndGame();
                        }else{
                           gamestatus->setText(QString(turn) + "'s king is in check, no checkmate!");
                           gamestatus->show();
                        }
                    }

                    QString s = QString(turn);
                    status->setText("Game status:\n" + s + "'s turn to play!");
                    if (multiplayer)
                    {
                        if (player_is_client)
                        {
                            clientSend(stringtoarray());
                        }else{
                            serverSend(stringtoarray());
                        }
                    }
                    //AI move
                    if (singleplayer)
                    {
                        AImove();
                        turn = 'w';
                    }
                }
            Delete_possible_moves();
            return 1;
        }
    }
    return 0;
}

void MainWindow::UpdateLeaderboard()
{
    int score = 100;
    QVectorIterator<piecetracker*> tracker(piece_tracker);

    while (tracker.hasNext())
    {
        piecetracker *t = tracker.next();
        if (client_won)
        {
            if (t->team == 'w')
            {
               score = score - t->num_moves;
            }
        }else{
            if (t->team == 'b')
            {
               score = score - t->num_moves;
            }
        }
    }

    HighScore hs;
    hs.addScore(playername.toUtf8(), score);
//    hs.UpdateScore("piet");
}

void MainWindow::AImove()
{
    //player's move
    QString position;
    position = convertCoordinatesToChessPosition(from_xcoord, from_ycoord, to_xcoord, to_ycoord);
    if (allmoves.isNull())
    {
        allmoves = position;
    }else{
        allmoves = allmoves + " " + position;
    }
    command = "position startpos moves " + allmoves + "\ngo\n";
    connector->writeData(command.toUtf8());//position startpos moves e2e3
    //AIreadyread will emit and we get AI's move
}

QString MainWindow::convertCoordinatesToChessPosition(int from_x, int from_y, int to_x, int to_y) {
    QString position;
    position.append(QChar(from_x - 1 + 'a'));
    position.append(QString::number(from_y));
    position.append(QChar(to_x - 1 + 'a'));
    position.append(QString::number(to_y));
    return position;                             //format e2e4
}

bool MainWindow::Validmove(char type, int x, int y)  //check if piece moved into open space or onto enemy team's piece or in En_passant position
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
                    if (t->num_moves == 0)
                    {
                        QVectorIterator<QLabel*> piece2(all_pieces);
                        QVectorIterator<piecetracker*> tracker2(piece_tracker);
                        while (tracker2.hasNext())
                        {
                            piecetracker* t2 = tracker2.next();
                            QLabel* p2 = piece2.next();
                            if ((GetxPosition(t2->x_cor) == x) && (GetyPosition(t2->y_cor) == y))
                            {
                                if ((x == 1) && (horizontal_left_boundary == 3) && (t2->type == 'r') && (t2->team == turn))
                                {
                                    to_xcoord = to_xcoord + 2;
                                    queenside_castling = true;
                                    castling_rook_pt = t2;
                                    castling_rook_lbl = p2;
                                    capture = false;
                                    return true;
                                }else if ((x == 8) && (horizontal_right_boundary == 2) && (t2->type == 'r') && (t2->team == turn))
                                {
                                    to_xcoord = to_xcoord - 1;
                                    kingside_castling = true;
                                    castling_rook_pt = t2;
                                    castling_rook_lbl = p2;
                                    capture = false;
                                    return true;
                                }
                            }
                        }
                    }
                    capture = false;
                    return true;
                }else{return false;}
            }
        }
        //En_passant
        if ((((GetyPosition(t->y_cor) == 5) && (team == 'w')) || ((GetyPosition(t->y_cor) == 4) && (team == 'b'))) && (t->num_moves == 1))
        {
            if (((GetxPosition(t->x_cor) == x) && ((GetyPosition(t->y_cor)-y) == 1) && (team == 'b')) ||
                    ((GetxPosition(t->x_cor) == x) && (y-(GetyPosition(t->y_cor)) == 1) && (team == 'w')))
            {
                if ((abs(from_xcoord-GetxPosition(t->x_cor)) == 1) && (from_ycoord == GetyPosition(t->y_cor)))
                {
                    capture = true;
                    enemy_index = piece_tracker.indexOf(t);
                    return true;
                }
            }
        }
    }
    capture = false;

    return true;  //open space thus valid move
}

bool MainWindow::Validpiecemove(char team, char type, int num_moves, int from_x, int from_y, int x, int y) //check if specific piece has valid move based on its specific characteristical moves
{
    board board;
    for (int k=0; k<=63 ; ++k)
    {
        possiblexmoves[k] = 0;
        possibleymoves[k] = 0;
//        ogpossiblexmoves[k] = 0;
//        ogpossibleymoves[k] = 0;
    }
//    Getboundaries(team, from_x, from_y);
//    Pawn pawn;
//    Knight knight;
//    Rook rook;
//    Bishop bishop;
//    Queen queen;
//    King king;
//    switch (type) {
//    case 'p' :
//        if (En_passant(team,from_x, from_y, x, y)){return true;}
////        if (pawn.ValidMove(team, num_moves, capture, from_x, from_y, x, y, vertical_up_boundary, vertical_down_boundary)) {return true;} break;
//    case 'n' :
//        if (knight.ValidMove(team, from_x, from_y, x, y)) {return true;} break;
//    case 'r' :
//        if (rook.ValidMove(team, from_x, from_y, x, y, vertical_up_boundary, vertical_down_boundary, horizontal_left_boundary, horizontal_right_boundary)) {return true;} break;
//    case 'b' :
//        if (bishop.ValidMove(team, from_x, from_y, x, y, right_up_diagonal_boundary, left_up_diagonal_boundary, right_down_diagonal_boundary, left_down_diagonal_boundary)) {return true;} break;
//    case 'q' :
//        if (queen.ValidMove(team, from_x, from_y, x, y, vertical_up_boundary, vertical_down_boundary, horizontal_left_boundary, horizontal_right_boundary,right_up_diagonal_boundary, left_up_diagonal_boundary, right_down_diagonal_boundary, left_down_diagonal_boundary)) {return true;} break;
//    case 'k' :
//        if (Castling(num_moves, horizontal_left_boundary, horizontal_right_boundary, x, y)) {return true;}
//        if (king.ValidMove(team, check, from_x, from_y, x, y)) {return true;} break;
//    }
//    return false;
    Getboundaries(team,from_x,from_y);
    if (type == 'p')
    {
        Calcpawnpossiblemoves(team, num_moves, from_x, from_y,
                               vertical_up_boundary, vertical_down_boundary, right_up_diagonal_boundary, left_up_diagonal_boundary,
                               right_down_diagonal_boundary, left_down_diagonal_boundary);
    }
    if (type == 'n')
    {
        Calcknightpossiblemoves(from_x,from_y);
    }
    if (type == 'r')
    {
        Calcrookpossiblemoves(from_x, from_y, vertical_up_boundary, vertical_down_boundary, horizontal_left_boundary, horizontal_right_boundary);
    }
    if (type == 'b')
    {
        Calcbishoppossiblemoves(from_x,from_y,right_up_diagonal_boundary,left_up_diagonal_boundary, right_down_diagonal_boundary, left_down_diagonal_boundary);
    }
    if (type == 'q')
    {
        Calcqueenpossiblemoves(from_x, from_y, vertical_up_boundary, vertical_down_boundary, horizontal_left_boundary, horizontal_right_boundary,right_up_diagonal_boundary,left_up_diagonal_boundary, right_down_diagonal_boundary, left_down_diagonal_boundary);
    }
    if (type == 'k')
    {
        Calckingpossiblemoves(num_moves, from_x, from_y, vertical_up_boundary, vertical_down_boundary, horizontal_left_boundary, horizontal_right_boundary,right_up_diagonal_boundary,left_up_diagonal_boundary, right_down_diagonal_boundary, left_down_diagonal_boundary);
    }
    //all other pieces
    for (int i = 0; i <= 63; ++i)
    {
        if (possiblexmoves[i] == 0)
        {
            break;
        }else{
                if ((possiblexmoves[i] == x) && (possibleymoves[i] == y))
                {
                    return true;
                }
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

void MainWindow::Getboundaries(char team, int from_x, int from_y)
{
    int vert_up_min = 999, vert_down_min = 999, hort_left_min = 999, hort_right_min = 999,
            right_up_diagonal_bound = 999, right_down_diagonal_bound = 999, left_up_diagonal_bound = 999, left_down_diagonal_bound = 999,
            distance;
    QVectorIterator<piecetracker*> tracker(piece_tracker);
    while(tracker.hasNext())
    {
        piecetracker *t = tracker.next();
        //vertical
        if (GetxPosition(t->x_cor) == from_x)
        {
            //vert_down
            if (GetyPosition(t->y_cor) < from_y)
            {
                if (team == t->team)
                {
                   distance = abs(GetyPosition(t->y_cor)-from_y)-1;
                }else{
                   distance = abs(GetyPosition(t->y_cor)-from_y);
                }
                 //if not enemy
                if (vert_down_min>distance)
                {
                    vert_down_min = distance;
                }
            }
            //vert_up
            if (GetyPosition(t->y_cor) > from_y)
            {
                if (team == t->team)
                {
                   distance = abs(GetyPosition(t->y_cor)-from_y)-1;
                }else{
                   distance = abs(GetyPosition(t->y_cor)-from_y);
                }
                if (vert_up_min>distance)
                {
                    vert_up_min = distance;
                }
            }
        }
        //horisontal
        if (GetyPosition(t->y_cor) == from_y)
        {
            //hort_right
            if (GetxPosition(t->x_cor) > from_x)
            {
                if (team == t->team)
                {
                   distance = abs(GetxPosition(t->x_cor)-from_x)-1;
                }else{
                   distance = abs(GetxPosition(t->x_cor)-from_x);
                }

                if (hort_right_min>distance)
                {
                    hort_right_min = distance;
                }
            }
            //hort_left
            if (GetxPosition(t->x_cor) < from_x)
            {
                if (team == t->team)
                {
                   distance = abs(GetxPosition(t->x_cor)-from_x)-1;
                }else{
                   distance = abs(GetxPosition(t->x_cor)-from_x);
                }

                if (hort_left_min>distance)
                {
                    hort_left_min = distance;
                }
            }
        }
        //diagonal
        if ((abs(GetyPosition(t->y_cor) - from_y)) == (abs(GetxPosition(t->x_cor) - from_x)))
        {
            //right_down
            if (((from_y - GetyPosition(t->y_cor)) > 0) && ((GetxPosition(t->x_cor) - from_x) > 0))
            {
                if (team == t->team)
                {
                   distance = abs(GetxPosition(t->x_cor)-from_x)-1;
                }else{
                   distance = abs(GetxPosition(t->x_cor)-from_x);
                }
                if (right_down_diagonal_bound>distance)
                {
                    right_down_diagonal_bound = distance;
                }
            }
            //left_down
            if (((from_y - GetyPosition(t->y_cor)) > 0) && ((from_x - GetxPosition(t->x_cor)) > 0))
            {
                if (team == t->team)
                {
                   distance = abs(GetxPosition(t->x_cor)-from_x)-1;
                }else{
                   distance = abs(GetxPosition(t->x_cor)-from_x);
                }
                if (left_down_diagonal_bound>distance)
                {
                    left_down_diagonal_bound = distance;
                }
            }
            //left_up
            if (((GetyPosition(t->y_cor) - from_y) > 0) && ((from_x - GetxPosition(t->x_cor)) > 0))
            {
                if (team == t->team)
                {
                   distance = abs(GetxPosition(t->x_cor)-from_x)-1;
                }else{
                   distance = abs(GetxPosition(t->x_cor)-from_x);
                }
                if (left_up_diagonal_bound>distance)
                {
                    left_up_diagonal_bound = distance;
                }
            }
            //right_up
            if (((GetyPosition(t->y_cor) - from_y) > 0) && ((GetxPosition(t->x_cor) - from_x) > 0))
            {
                if (team == t->team)
                {
                   distance = abs(GetxPosition(t->x_cor)-from_x)-1;
                }else{
                   distance = abs(GetxPosition(t->x_cor)-from_x);
                }
                if (right_up_diagonal_bound>distance)
                {
                    right_up_diagonal_bound = distance;
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

bool MainWindow::Checkcapture(char team, int to_x, int to_y)
{
    board board;
    QVectorIterator<piecetracker*> tracker(piece_tracker);
    while (tracker.hasNext())
    {
        piecetracker *t = tracker.next();

        if ((team != t->team) && (board.AssignxCoord(to_x) == t->x_cor) && (board.AssignyCoord(to_y) == t->y_cor))
        {
            return true;
        }
    }
    return false;
}

bool MainWindow::Check_yourself(char team, int from_x, int from_y, int to_x, int to_y, piecetracker* piecetrack)
{
    board board;
    bool check_opponent = false;
//    capture_state = Checkcapture(team,to_x,to_y);

//    if (kingside_castling)
//    {
//        to_x = 7;
//        to_y = 1;
//    }

    if (possiblekingside_castling)
    {
        castling_rook_pt->x_cor = board.AssignxCoord(to_x-1);   //move temporarily
        castling_rook_pt->y_cor = board.AssignyCoord(to_y);
//        if (team == 'w')
//        {
//            king_xpos = 5;      //coords of my king
//            king_ypos = 1;
//        }else{
//            king_xpos = 5;      //coords of my king
//            king_ypos = 8;
//        }

    }
    if (possiblequeenside_castling)
    {
        castling_rook_pt->x_cor = board.AssignxCoord(to_x+1);   //move temporarily
        castling_rook_pt->y_cor = board.AssignyCoord(to_y);
//        if (team == 'w')
//        {
//            king_xpos = 5;      //coords of my king
//            king_ypos = 1;
//        }else{
//            king_xpos = 5;      //coords of my king
//            king_ypos = 8;
//        }
    }

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
                king_xpos = t->x_cor;      //coords of my king
                king_ypos = t->y_cor;
                break;
            }
        }
    }

    tracker.toFront();
    while (tracker.hasNext())
    {
        piecetracker *t = tracker.next();
        if ((team != t->team) && (board.AssignxCoord(to_x) == t->x_cor) && (board.AssignyCoord(to_y) == t->y_cor))
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

    if (possiblekingside_castling)
    {
        castling_rook_pt->x_cor = board.AssignxCoord(8);   //move temporarily
        castling_rook_pt->y_cor = board.AssignyCoord(1);
    }
    if (possiblequeenside_castling)
    {
        castling_rook_pt->x_cor = board.AssignxCoord(1);   //move temporarily
        castling_rook_pt->y_cor = board.AssignyCoord(1);
    }

    piecetrack->x_cor = board.AssignxCoord(from_x);
    piecetrack->y_cor = board.AssignyCoord(from_y);
//    capture = capture_state;
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
                king_xpos = t->x_cor;            //ander team se king
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

bool MainWindow::Checkmate(char team)
{
//    QVectorIterator<piecetracker*> tracker(piece_tracker);
//    while (tracker.hasNext())
//    {
//        piecetracker *pt = tracker.next();
//        if (pt->team == turn)
//        {
//            for (int i = 1; i <= 8; ++i)
//            {
//                for (int j = 1; j <= 8; ++j)
//                {
//                    if ((Validmove(pt->type, i, j)) && (Validpiecemove(pt->team,pt->type,pt->num_moves,GetxPosition(pt->x_cor), GetyPosition(pt->y_cor),i,j)))
//                    {
//                        if (!Check_yourself(pt->team,GetxPosition(pt->x_cor),GetyPosition(pt->y_cor),i,j,pt))
//                        {
//                            return false;
//                        }
//                    }
//                }
//            }
//        }
//    }

    int temp_ogpossiblexmoves[64];
    int temp_ogpossibleymoves[64];
    QVectorIterator<piecetracker*> tracker(piece_tracker);
    while (tracker.hasNext())
    {
        piecetracker *pt = tracker.next();
        if (pt->team != team)  //check if enemy has any moves
        {
            for (int k=0; k<=63 ; ++k)
            {
                possiblexmoves[k] = 0;
                possibleymoves[k] = 0;
                temp_ogpossiblexmoves[k] = 0;
                temp_ogpossibleymoves[k] = 0;
            }
            Getboundaries(pt->team,GetxPosition(pt->x_cor),GetyPosition(pt->y_cor));
            if (pt->type == 'p')
            {
                Calcpawnpossiblemoves(pt->team, pt->num_moves, GetxPosition(pt->x_cor), GetyPosition(pt->y_cor),
                                       vertical_up_boundary, vertical_down_boundary, right_up_diagonal_boundary, left_up_diagonal_boundary,
                                       right_down_diagonal_boundary, left_down_diagonal_boundary);
            }
            if (pt->type == 'n')
            {
                Calcknightpossiblemoves(GetxPosition(pt->x_cor),GetyPosition(pt->y_cor));
            }
            if (pt->type == 'r')
            {
                Calcrookpossiblemoves(GetxPosition(pt->x_cor), GetyPosition(pt->y_cor), vertical_up_boundary, vertical_down_boundary, horizontal_left_boundary, horizontal_right_boundary);
            }
            if (pt->type == 'b')
            {
                Calcbishoppossiblemoves(GetxPosition(pt->x_cor),GetyPosition(pt->y_cor),right_up_diagonal_boundary,left_up_diagonal_boundary, right_down_diagonal_boundary, left_down_diagonal_boundary);
            }
            if (pt->type == 'q')
            {
                Calcqueenpossiblemoves(GetxPosition(pt->x_cor), GetyPosition(pt->y_cor), vertical_up_boundary, vertical_down_boundary, horizontal_left_boundary, horizontal_right_boundary,right_up_diagonal_boundary,left_up_diagonal_boundary, right_down_diagonal_boundary, left_down_diagonal_boundary);
            }
            if (pt->type == 'k')
            {
                Calckingpossiblemoves(pt->num_moves, GetxPosition(pt->x_cor), GetyPosition(pt->y_cor), vertical_up_boundary, vertical_down_boundary, horizontal_left_boundary, horizontal_right_boundary,right_up_diagonal_boundary,left_up_diagonal_boundary, right_down_diagonal_boundary, left_down_diagonal_boundary);
            }
            //all other pieces
            for (int k=0; k<=63 ; ++k)
            {
                temp_ogpossiblexmoves[k] = possiblexmoves[k];
                temp_ogpossibleymoves[k] = possibleymoves[k];
            }
            for (int i = 0; i <= 63; ++i)
            {
                if (temp_ogpossiblexmoves[i] == 0)
                {
                    break;
                }else{
                    if (!Check_yourself(pt->team,GetxPosition(pt->x_cor), GetyPosition(pt->y_cor),temp_ogpossiblexmoves[i],temp_ogpossibleymoves[i],pt))
                    {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

bool MainWindow::L_En_passant(char team, int from_x, int from_y)
{
    board board;
    QVectorIterator<piecetracker*> tracker(piece_tracker);
    while (tracker.hasNext())
    {
        piecetracker* t = tracker.next();
        if (team == 'w')
        {
            if ((t->type == 'p') && (t->team != team) && (t->num_moves == 1) && (t->y_cor == board.AssignyCoord(5)))
            {
                if ((t->x_cor == board.AssignxCoord(from_x-1)) && (t->y_cor == board.AssignyCoord(from_y)))
                {
                    return true;
                }
            }
        }
        if (team == 'b')
        {
            if ((t->type == 'p') && (t->team != team) && (t->num_moves == 1) && (t->y_cor == board.AssignyCoord(4)))
            {
                if ((t->x_cor == board.AssignxCoord(from_x-1)) && (t->y_cor == board.AssignyCoord(from_y)))
                {
                    return true;
                }
            }
        }
//        if ((((GetyPosition(t->y_cor) == 5) && (team == 'w')) || ((GetyPosition(t->y_cor) == 4) && (team == 'b'))) && (t->num_moves == 1))
//        {
//            if (((GetxPosition(t->x_cor) == to_x) && ((GetyPosition(t->y_cor)-to_y) == 1) && (team == 'b')) ||
//                    ((GetxPosition(t->x_cor) == to_x) && (to_y-(GetyPosition(t->y_cor)) == 1) && (team == 'w')))
//            {
//                if ((abs(from_x-GetxPosition(t->x_cor)) == 1) && (from_y == GetyPosition(t->y_cor)))
//                {
//                    capture = true;
//                    enemy_index = piece_tracker.indexOf(t);
//                    return true;
//                }
//            }
//        }
    }
    return false;
}

bool MainWindow::R_En_passant(char team, int from_x, int from_y)
{
    board board;
    QVectorIterator<piecetracker*> tracker(piece_tracker);
    while (tracker.hasNext())
    {
        piecetracker* t = tracker.next();

        if (team == 'w')
        {
            if ((t->type == 'p') && (t->team != team) && (t->num_moves == 1) && (t->y_cor == board.AssignyCoord(5)))
            {
                if ((t->x_cor == board.AssignxCoord(from_x+1)) && (t->y_cor == board.AssignyCoord(from_y)))
                {
                    return true;
                }
            }
        }
        if (team == 'b')
        {
            if ((t->type == 'p') && (t->team != team) && (t->num_moves == 1) && (t->y_cor == board.AssignyCoord(4)))
            {
                if ((t->x_cor == board.AssignxCoord(from_x+1)) && (t->y_cor == board.AssignyCoord(from_y)))
                {
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
                if ((to_x == 1) && (left_bound == 3) && (t->type == 'r') && (t->team == turn))
                {
//                    if (click_counter == 0)
//                    {
//                        to_xcoord = to_xcoord + 2;
                        queenside_castling = true;
                        castling_rook_pt = t;
                        castling_rook_lbl = p;
//                    }
                    return true;
                }else if ((to_x == 8) && (right_bound == 2) && (t->type == 'r') && (t->team == turn))
                {
//                    if (click_counter == 0)
//                    {
//                        to_xcoord = to_xcoord - 1;
                        kingside_castling = true;
                        castling_rook_pt = t;
                        castling_rook_lbl = p;
//                    }
                    return true;
                }
            }
        }
    }
    return false;
}

//int MainWindow::getScore(QString name)
//{
//    int temp_score;

//    //file creation
//    return temp_score;
//}

//void MainWindow::updateScore(QByteArray name, int temp_score)
//{

//}

MainWindow::~MainWindow()
{
//    if (client)
//        client->deleteLater();

//    if (server)
//        server->deleteLater();
}


