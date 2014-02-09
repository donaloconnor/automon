/*
    ==================================================================================================
    |  cardetailswidget.cpp                                                                          |
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

    The implementation file of the CarDetailsWidget. Further details below

*/

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "cardetailswidget.h"

CarDetailsWidget::CarDetailsWidget(Automon * kernel, QWidget * parent)
        : QWidget(parent), m_kernel(kernel)
{

    /* Create Layouts */
    m_mainLayout = new QHBoxLayout(this);
    m_verticalLayout = new QVBoxLayout();

    /* Create the labels that the user will see, including the tr() function for easy translation in future */
    m_header = new QLabel(tr("Car Details"));
    QLabel * carVin = new QLabel(tr("<strong>Vehicle ID:</strong> ") + kernel->getVin());
    QLabel * obdStandard = new QLabel(tr("<strong>Car's OBD Standard:</strong> ") + kernel->getOBDStandardType());
    QLabel * obdProtocol = new QLabel(tr("<strong>Car's OBD Protocol:</strong> ") + kernel->getOBDProtocol());
    QLabel * elmVersion = new QLabel(tr("<strong>ELM Interface Version:</strong> ") + kernel->getElmVersion());
    QLabel * voltage = new QLabel(tr("<strong>Current Battery Voltage:</strong> ") + kernel->getVoltage());
    QLabel * introduction = new QLabel(tr("This widget displays some very simple car details."));

    /* Set style of header */
    m_header->setStyleSheet("color: #ace413; font-size:25px");

    /* Organise layouts and add widgets to layouts */
    m_mainLayout->addLayout(m_verticalLayout);
    m_verticalLayout->addWidget(m_header);
    m_verticalLayout->addWidget(introduction);
    m_verticalLayout->addSpacing(10);
    m_verticalLayout->addWidget(carVin);
    m_verticalLayout->addWidget(obdStandard);
    m_verticalLayout->addWidget(obdProtocol);
    m_verticalLayout->addWidget(elmVersion);
    m_verticalLayout->addWidget(voltage);
    m_verticalLayout->setAlignment(Qt::AlignTop);
    m_mainLayout->setAlignment(Qt::AlignTop);

    /* Set the main layout */
    setLayout(m_mainLayout);
}
