#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

/*
    ==================================================================================================
    |  dashboardwidget.h                                                                             |
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

    This class, DashBoardWidget is a widget that is used in the Main GUI, as one of the stacked widgets
    in the AutomonApp class. It is displayed when the Digital Dashboard menu button is clicked.

    It includes two instances of the S5WDial class. This class is third party and is licensed under the GPL.
    I've modified it to my liking.
*/

#include <QWidget>
#include "automon.h"

class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class S5WDial;

using namespace AutomonKernel;

class DashboardWidget : public QWidget
{
    Q_OBJECT

public:
    DashboardWidget(Automon * kernel, QWidget * parent = 0);

signals:
    void changeStatus(const QString & status);

public slots:
    void startStopDashboard();


private:
    QLabel * m_header;
    QHBoxLayout * m_mainLayout;
    QVBoxLayout * m_verticalLayout;
    QHBoxLayout * m_dialLayouts;
    QPushButton * m_startStopButton;
    QHBoxLayout * m_buttonLayout;
    S5WDial * m_revDial;
    S5WDial * m_speedDial;
    Automon * m_kernel;
    bool m_started;
};

#endif // DASHBOARDWIDGET_H
