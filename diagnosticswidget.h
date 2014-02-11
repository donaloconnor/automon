#ifndef DIAGNOSTICSWIDGET_H
#define DIAGNOSTICSWIDGET_H

/*
    ==================================================================================================
    |  diagnosticswidget.h                                                                           |
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

    This is the DiagnosticsWidget class. It is a widget that forms part of the GUI of Automon.
    It is responsible for displaying DTC codes, giving the neccessary buttons/widgets for the user
    to clear the MIL and re check for DTCs etc.
*/


#include <QWidget>
#include "automon.h"

class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QTableWidget;
class QPushButton;
class QMessageBox;

using namespace AutomonKernel;

class DiagnosticsWidget : public QWidget
{
    Q_OBJECT

public:
    DiagnosticsWidget(Automon * kernel, QWidget * parent = 0);

signals:
    void changeStatus(const QString & status);

public slots:
    void resetMIL();    /* Slot for the reset MIL button */
    void checkECU();    /* Slot for the CheckECU button */

private:
    void setupDTCTable();
    void displayNoDTCs();
    void displayMilResetMsg();
    void setMilPicOn();
    void setMilPicOff();
    QLabel * m_header;
    QLabel * m_noDTCMsg;
    QHBoxLayout * m_mainLayout;
    QVBoxLayout * m_verticalLayout;
    QHBoxLayout * m_contentArea;
    QHBoxLayout * m_buttonsTop;
    QVBoxLayout * m_leftSide;
    QVBoxLayout * m_rightSide;
    Automon * m_kernel;
    QTableWidget * m_tableList;
    QPushButton * m_resetMilButton;
    QPushButton * m_checkButton;
    QLabel * m_milResetMsg;
    QString engineMilOnPic;
    QString engineMilOffPic;
    QLabel * engineMilPic;
    QMessageBox * m_resetMilPromptBox;


};

#endif // DIAGNOSTICSWIDGET_H
