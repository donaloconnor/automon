#ifndef CARDETAILSWIDGET_H
#define CARDETAILSWIDGET_H

/*
    ==================================================================================================
    |  cardetailswidget.h                                                                            |
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

    The CarDetailsWidget class is a widget used in the AutomonApp stacked widget. It is a view that is shown
    when the user clicks the Car Details menu button. It displays a very simply summary of details such as
    the car's VIN, OBD Standard and protocol, car battery voltage etc.

*/

#include <QWidget>

#include "automon.h"

class QLabel;
class QVBoxLayout;
class QHBoxLayout;

using namespace AutomonKernel;

class CarDetailsWidget : public QWidget
{
    Q_OBJECT

public:
    CarDetailsWidget(Automon * kernel, QWidget * parent = 0);

signals:
    void changeStatus(const QString & status);
    
private:
    QLabel * m_header;
    QHBoxLayout * m_mainLayout;
    QVBoxLayout * m_verticalLayout;
    Automon * m_kernel;
};

#endif // CARDETAILSWIDGET_H
