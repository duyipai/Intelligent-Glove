/****************************************************************************
**
** Copyright (C) 2018 The CUMAU INC.
** Author: Siwei CAI
** Date: 8.8.2018
**
** Main of the GUI.
**
**
****************************************************************************/

#include "mainwindow.h"
#include <QApplication>
#include <qpushbutton.h>
#include <qfont.h>

#include <QLabel>
#include <QSurfaceFormat>
#include "mainwidget.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "valuewidget.h"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow mainWindow;

    mainWindow.show();

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(format);

    return a.exec();
}

