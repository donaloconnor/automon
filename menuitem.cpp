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

#include "menuitem.h"


MenuItem::MenuItem(const QIcon & icon, const QString & name, const QSize & size, QWidget *parent)
        : QPushButton(icon, "", parent), m_name(name)
{       
    /* This is a reference value to the stacked widget index in the main GUI */
    m_stackedWidgetIndex = -1;

    /* Connect the clicked event of this push button to a custom slot so we can override the clicked event */
    connect(this, SIGNAL(clicked()), this, SLOT(changePanel()));

    /* Set the size of the button and icon to the defined one in constructor */
    setFixedSize(size);
    setIconSize(size);

    /* Set no background */
    setStyleSheet("background:none;");   
}

MenuItem::~MenuItem()
{

}

QString MenuItem::getName()
{
    /* Return name of menu item. */
    return m_name;
}

void MenuItem::setPanelIndex(int index)
{
    /* Set the index of the stacked widget that this button will load */
    m_stackedWidgetIndex = index;
}

void MenuItem::changePanel()
{
    /* Basically over riding the clicked signal. So we just emit another signal to change the panel to this index */
    emit displayPanel(m_stackedWidgetIndex);
}
