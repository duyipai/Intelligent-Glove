/****************************************************************************
**
** Copyright (C) 2018 The CUMAU INC.
** Author: Siwei CAI
** Date: 8.8.2018
**
** This file is the header of Class MainWindow. MainWindow is a QT UI
** design form that display widgets together in a window.
**
**
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
