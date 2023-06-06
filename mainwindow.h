#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <stdlib.h>

#include <QMainWindow>
#include <QTimer>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QKeyEvent>
#include <QString>
#include <QPixmap>
#include <QImage>
#include <QPalette>
#include <QDesktopWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QChar>
#include "server.h"
#include "client.h"
#include "stockfishconnector.h"

#include <vector>
#include <random>

#include <QApplication>
#include <QMediaPlayer>

typedef struct piecetracker
{
    int x_cor = 0;
    int y_cor = 0;
    char team;
    char type;
    int num_moves = 0;
}
piecetracker;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void Makepiece(QString, char, char, int, int, int);
    void Getboundaries(char, int, int);
    bool playing_game = false;
    QString AIboard;
    //game
    int possiblexmoves[64];
    int possibleymoves[64];
    int ogpossiblexmoves[64];
    int ogpossibleymoves[64];

    void Calcpawnpossiblemoves(char, int, int, int, int, int, int, int, int, int);
    void Calcknightpossiblemoves(int, int);
    void Calcrookpossiblemoves(int, int, int, int, int, int);
    void Calcbishoppossiblemoves(int, int, int, int, int, int);
    void Calcqueenpossiblemoves(int, int, int, int, int, int, int, int, int, int);
    void Calckingpossiblemoves(int, int, int, int, int, int, int, int, int, int, int);

protected:
    void mousePressEvent(QMouseEvent *event);

    //variables inherited by piece classes
    int click_counter = 0;

    //Vector lists
    QVector<QLabel*> all_pieces;
    QVector<piecetracker*> piece_tracker;
    QVector<QPushButton*> GUI;
    QLineEdit* name;

private:
    // QObjects

    // Vector lists
    QVector<QLabel*> possible_moves;

    // Variables
    int width = 800, height = 800, from_xcoord, from_ycoord, to_xcoord, to_ycoord, enemy_index, king_xpos, king_ypos,
    vertical_up_boundary, vertical_down_boundary, horizontal_left_boundary, horizontal_right_boundary,
    right_up_diagonal_boundary, right_down_diagonal_boundary, left_up_diagonal_boundary, left_down_diagonal_boundary, score,
    AIto_x, AIto_y, AIfrom_x, AIfrom_y;
    char turn = 'w', clicked_team, team;
    bool clicked_on_piece = false, capture = false, check = false, checkmate = false, queenside_castling = false, kingside_castling = false,
    player_is_server = false, player_is_client = false, multiplayer = false, singleplayer = false, serv_won = false, client_won = false,
    possiblequeenside_castling = false, possiblekingside_castling = false;
    piecetracker* castling_rook_pt;
    QLabel* castling_rook_lbl;

    //GUI
    QLabel* lblname, *status, *menustatus, *gamestatus, *background, *new_frame, *welc_message, *lblboard;
    QString playername, opponentname, leaderboardname;
    QPushButton* btnyes, *btnno, *btnreturntomain, *btnplayagain, *showboard;

    //interaction
    QString question;
    bool ans = false;

    //network variables
    Server *server;
    Client *client;
    QString *input;
    QString *output;

    //leaderboard
    QLabel *scoredisplay;

    //sound
    QMediaPlayer player;

private slots:
    void SetupGUI();
    void NewGame();
    void ResetGame();
    void EndGame();
    void NetworkGUI();
    void moveAIpiece();

    void makeClient();
    void makeServer();
    void clientSend(QByteArray);
    void serverSend(QByteArray);
    void receive(QByteArray);

    void singleplayer_clicked();
    void multiplayer_clicked();
    void DefaultBoard();
    bool Clicked_on_Piece(int, int);
    bool Validpiecemove(char, char, int, int, int, int, int);
    bool Validmove(char, int, int);
    void Delete_possible_moves();
    int GetxPosition(int);
    int GetyPosition(int);
    bool Checkcapture(char, int, int);
    bool Check_yourself(char, int, int, int, int, piecetracker*);
    bool Check_opponent(int, int, piecetracker*);
    bool Checkmate(char);
    bool L_En_passant(char, int, int);
    bool R_En_passant(char, int, int);
    bool Castling(int, int, int, int, int);
    bool PossibleCastling(piecetracker*);
    void arraytovector(QByteArray);
    QByteArray stringtoarray();

    //interaction
    void Forfeit();
    void Draw();
    void Playagain();
    void Askopponent();
    void ansyes();
    void ansno();

    //leaderboard
    void UpdateLeaderboard();

    //AI
    void AImove();
    void ResetAI();
    QString convertCoordinatesToChessPosition(int, int, int, int);
//    void startup_engine();
    void AIreceive(QByteArray);
//    void writedata();
    int convertLetterToNumber(char);
    void convertChessPosition(const QString);
    void Calcplayerbestmove();

private:
    StockfishConnector* connector;
    QPushButton *AI;
    QPushButton *btnwrite;
    QTimer *timer;
    QString allmoves, chessposition, command, position;
    bool askbestmove = false;
    int hints = 3, counter = 0, xdelta, ydelta, newx, newy;
};

#endif // MAINWINDOW_H
