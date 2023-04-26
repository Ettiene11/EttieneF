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

protected:
    void mousePressEvent(QMouseEvent *event);

    //variables inherited by piece classes
    int click_counter = 0;

private:
    // QObjects

    // Vector lists
    QVector<QLabel*> all_pieces;
    QVector<piecetracker*> piece_tracker;
    QVector<QLabel*> possible_moves;

    // Variables
    int width = 800, height = 800, from_xcoord, from_ycoord, to_xcoord, to_ycoord, enemy_index;
    char turn = 'w', clicked_team;
    bool clicked_on_piece = false, firstmove, capture = false;

private slots:
    void DefaultBoard();
    int GetxPosition(int);
    int GetyPosition(int);
    bool Clicked_on_Piece(int, int);
    bool Validpiecemove(char, char, int, int);
    bool Validmove(int, int);
    void Delete_possible_moves();

};

#endif // MAINWINDOW_H
