/*

    This file is part of the Automon Project (OBD Diagnostics) - http://www.automon.io/
    Source Repository: https://github.com/donaloconnor/automon/
    
    Copyright (c) 2015, Donal O'Connor <donaloconnor@gmail.com>

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
    
*/

#ifndef AUTOMONAPP_H
#define AUTOMONAPP_H

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
