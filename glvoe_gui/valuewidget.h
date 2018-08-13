/****************************************************************************
**
** Copyright (C) 2018 The CUMAU INC.
** Author: Siwei CAI
** Date: 8.8.2018
**
** This is the header file of class ValueWidget, the widget class
** used to display text infos of XDK.
**
**
****************************************************************************/

#ifndef VALUEWIDGET_H
#define VALUEWIDGET_H

#include <QFrame>
#include <Qpainter>
#include <QBasicTimer>
#include <fstream>
#include <sstream>
#include <QString>

class ValueWidget : public QFrame
{
    Q_OBJECT

public:
    ValueWidget(QWidget *parent);
    ~ValueWidget();

protected:
    void timerEvent(QTimerEvent *e) override;

private:
    QBasicTimer timer;
    std::ifstream filein;
    void paintEvent(QPaintEvent* event) override;
    QString glo_state;
    QString glo_hint;
};
#endif // VALUEWIDGET_H
