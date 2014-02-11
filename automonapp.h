#ifndef AUTOMONAPP_H
#define AUTOMONAPP_H

/*
    ==================================================================================================
    |  automonapp.h                                                                                  |
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

    This class AutomonApp is responsible for the GUI system of Automon on the TS7390.
    It creates a menu in which each menu item is connected to an associated widget on the widget stack.
    It responsible for creating all widget objects and updating status etc.

    Further details will be included in the implementation file.

*/

#include <QFrame>
#include "automon.h"
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QWidget>

class QLabel;
class QHBoxLayout;
class QVBoxLayout;
class QSplashScreen;
class QStackedWidget;


#include "menuwidget.h"
#include "monitoringwidget.h"
#include "diagnosticswidget.h"
#include "dashboardwidget.h"
#include "accelerationtestwidget.h"
#include "cardetailswidget.h"
#include "ruleeditorwidget.h"
#include "errorhandler.h"

using namespace AutomonKernel; /* Need to use the AutomonKernel namespace to access Automon Kernel */


class AutomonApp : public QFrame
{
    Q_OBJECT

public:
    AutomonApp(QSplashScreen * splashScreen);

public slots:
    void changePanel(int panelID); /* Used by the Menu items to change the current Widget shown */
    void setStatus(const QString & statusText); /* Used to update the status text */

protected:
    void closeEvent(QCloseEvent *event);

private:
    void init();  /* Set up Automon Kernel */
    Automon * m_automonKernel; /* Pointer to the Automon Kernel */

    QVBoxLayout * m_mainLayout;
    QVBoxLayout * m_centerLayout;
    QHBoxLayout * m_bottomLayout;
    QHBoxLayout * m_topLayout;
    QSplashScreen * m_splashScreen;
    QStackedWidget * m_panes;
    QLabel * m_status;


    /* All the widgets/views in Automon */
    MenuWidget * m_menu;
    AccelerationTestWidget * m_acellerationTestWidget;
    DiagnosticsWidget * m_diagnosticsWidget;
    MonitoringWidget * m_monitoringWidget;
    DashboardWidget * m_dashboardWidget;
    CarDetailsWidget * m_carDetailsWidget;
    RuleEditorWidget * m_ruleEditorWidget;

    ErrorHandler * m_errorHandler;
};

#endif // AUTOMONAPP_H
