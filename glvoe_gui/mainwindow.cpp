/****************************************************************************
**
** Copyright (C) 2018 The CUMAU INC.
** Author: Siwei CAI
** Date: 8.8.2018
**
** Mainwindow of GUI, use Qt form to build dashboard.
**
****************************************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
