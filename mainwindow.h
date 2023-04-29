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

#include <vector>
#include <random>

typedef struct piecetracker
{
    int x_cor = 0;
    int y_cor = 0;
    char team;
    char type;
}
piecetracker;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void Makepiece(QString, char, char, int, int);
    void Getboundaries(int, int);

protected:
    void mousePressEvent(QMouseEvent *event);

    //variables inherited by piece classes
    int click_counter = 0;

    //Vector lists
    QVector<QLabel*> all_pieces;
    QVector<piecetracker*> piece_tracker;

private:
    // QObjects

    // Vector lists
    QVector<QLabel*> possible_moves;

    // Variables
    int width = 800, height = 800, from_xcoord, from_ycoord, to_xcoord, to_ycoord, enemy_index, king_xpos, king_ypos;
    int vertical_up_boundary, vertical_down_boundary, horizontal_left_boundary, horizontal_right_boundary;
    int right_up_diagonal_boundary, right_down_diagonal_boundary, left_up_diagonal_boundary, left_down_diagonal_boundary;
    char turn = 'w', clicked_team;
    bool clicked_on_piece = false, firstmove, capture = false, check = false, checkmate = false;

private slots:
    void DefaultBoard();
    bool Clicked_on_Piece(int, int);
    bool Validpiecemove(char, char, int, int, int, int);
    bool Validmove(int, int);
    void Delete_possible_moves();
    int GetxPosition(int);
    int GetyPosition(int);
    bool Check_yourself(char, int, int, int, int, piecetracker*);
    bool Check_opponent(int, int, piecetracker*);
    bool Checkmate();

};

#endif // MAINWINDOW_H
