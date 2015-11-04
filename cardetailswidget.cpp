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
