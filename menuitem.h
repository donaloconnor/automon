#ifndef MENUITEM_H
#define MENUITEM_H

/*
    ==================================================================================================
    |  menuitem.h                                                                                    |
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

    This is a class that inherits from QPushButton. It forms the button that is screen in the menu widget
    on top of the Automon screen.
*/

#include <QPushButton>
#include <QIcon>
#include <QWidget>
#include <QString>

class MenuItem : public QPushButton
{
    Q_OBJECT
public:
    MenuItem(const QIcon & icon, const QString & name, const QSize & size, QWidget *parent = 0);
    ~MenuItem();
    void setPanelIndex(int index);
    QString getName();

signals:
    void displayPanel(int stackedWidgetIndex); /* Signal that is used in the AutomonApp to change the widget shown */

public slots:
    void changePanel();

private:
    QString m_name;
    int m_stackedWidgetIndex;
};

#endif // MENUITEM_H
