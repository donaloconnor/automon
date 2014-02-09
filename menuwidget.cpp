/*
    ==================================================================================================
    |  menuwidget.cpp                                                                                  |
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

    Implementation of the Menu Widget. More Details below
*/

#include <QHBoxLayout>
#include "menuwidget.h"


MenuWidget::MenuWidget()
{
    m_layout = new QHBoxLayout();

    /* Create all the buttons, type Menu Item, specifying their icons and the size */
    m_buttons.append(new MenuItem(QIcon(":/icons/monitoringicon.png"),QString("MonitoringButton"), QSize(139,56)));
    m_buttons.append(new MenuItem(QIcon(":/icons/dashboardicon.png"),QString("DigitalDashboardButton"), QSize(184,56)));
    m_buttons.append(new MenuItem(QIcon(":/icons/diagnosticsicon.png"),QString("DiagnosticsButton"), QSize(149,56)));
    m_buttons.append(new MenuItem(QIcon(":/icons/accelerationtesticon.png"),QString("AccelerationTestButton"),QSize(186,56)));
    m_buttons.append(new MenuItem(QIcon(":/icons/cardetailsicon.png"),QString("CarDetailsButton"),QSize(140,56)));


    /* Add each of the buttons to the horizontal layout manager */
    for (int i = 0; i < m_buttons.size(); i++)
        m_layout->addWidget(m_buttons.at(i));

    setLayout(m_layout);

    /* Set no spacing or margins so it fits across nicely top of screen with no gaps */
    m_layout->setMargin(0);
    m_layout->setSpacing(0);
    m_layout->setAlignment(Qt::AlignTop);

}

bool MenuWidget::connectButtonToPanel(QString name, int panelID, QObject * receiverObj)
{
    /*
        This method is responsibe for connecting a button, MenuButton to a slot.
        The name is the name of the button we want to connect, panel ID is which panel it will
        launch and the receiver object is the stacked widget pointer
    */

    /* Assume button not found */
    bool found = false;

    for (int i = 0; i < m_buttons.size(); i++)
    {
        if (m_buttons.at(i)->getName().compare(name) == 0)
        {
            /* We found the correct button. Set it's index to panel ID and connect its signal to the stacked widget slot */
            found = true;
            m_buttons.at(i)->setPanelIndex(panelID);
            connect(m_buttons.at(i), SIGNAL(displayPanel(int)), receiverObj, SLOT(changePanel(int)));
        }
    }

    /* Return if found or not */
    return found;
}

MenuWidget::~MenuWidget()
{

}
