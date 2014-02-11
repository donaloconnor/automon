/*
    ==================================================================================================
    |  menuitem.cpp                                                                                  |
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

    Implementation of the Menu Item Pushbutton
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
