/*

    This file is part of the Automon Project (OBD Diagnostics) - http://www.automon.io/
    Source Repository: https://github.com/donaloconnor/automon/
    
    Copyright (c) 2015, Donal O'Connor

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

#ifndef DIAGNOSTICSWIDGET_H
#define DIAGNOSTICSWIDGET_H

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
