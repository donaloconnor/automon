#ifndef MENUWIDGET_H
#define MENUWIDGET_H

/*
    ==================================================================================================
    |  menuwidget.h                                                                                  |
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

    This class is a custom widget that displays a menu consisting of MenuItem push buttons.
    It is what you see on top. It is responsible for seting up click events to the panel
*/

#include <QWidget>
#include <QList>

class QHBoxLayout;

#include "menuitem.h"

class MenuWidget : public QWidget
{
public:
    MenuWidget();
    ~MenuWidget();
    bool connectButtonToPanel(QString name, int panelID, QObject * receiverObj);

private:
    QList<MenuItem *> m_buttons;
    QHBoxLayout * m_layout;



};

#endif // MENUWIDGET_H
