#ifndef ACCELERATIONTESTWIDGET_H
#define ACCELERATIONTESTWIDGET_H

/*
    ==================================================================================================
    |  accelerationtestwidget.h                                                                      |
    |  Part of the Automon application.                                                              |
    |                                                                                                |
    |  Final Year Project - "An Embedded Automotive Monitoring Device"                               |
    |                                                                                                |
    |  By Donal O' Connor for completion of B.Sc (Hons) Software Development and Computer Networking |
    |  Email: donaloconnor@gmail.com                                                                 |
    |  Website/Blog: http://automon.killarneyonline.eu                                               |
    |                                                                                                |
    | Cork Institute of Technology, Cork, Ireland - http://www.cit.ie/                               |
    |                                                                                                |
    | Copyright © 2009 Donal O'Connor <donaloconnor@gmail.com>                                       |
    ==================================================================================================


    The Acceleration Test widget is part of the main GUI of Automon.
    It is one of the menu items on top. It's purpose is to provide a GUI interface
    and also provide the logic to do an acceleration test.
    The implementation file includes the detail of what occurs.


    AccelerationTest is a class derived from widget.
    It also includes a signal, changeStatus in which the main GUI widget connects to, to receieve
    messages from this Widget and display them on the screen

*/

#include <QWidget>

#include "automon.h"

class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QLCDNumber;
class QPushButton;
class QSpinBox;
class QTime;

using namespace AutomonKernel;

class AccelerationTestWidget : public QWidget
{
    Q_OBJECT

public:
    AccelerationTestWidget(Automon * kernel, QWidget * parent = 0);
    ~AccelerationTestWidget();

public slots:
    void startTest();
    void display(double speed);

signals:
    void changeStatus(const QString & status);

private:
    void setUpSensor();
    void removeSensor();
    QLabel * m_header;
    QHBoxLayout * m_mainLayout;
    QVBoxLayout * m_verticalLayout;
    QLCDNumber * m_speed;
    QLCDNumber * m_accelerationTimeDisplay;
    Automon * m_kernel;
    QPushButton * m_startTest;
    QSpinBox * m_destinationSpeed;
    QTime * m_time;
    bool m_carMoving;
    int m_accelerationTime;
    bool m_testFinished;
    int m_parkThreshold;
    bool m_testStarted;
    bool m_startButtonClicked;
};

#endif // ACCELERATIONTESTWIDGET_H
