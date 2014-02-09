#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

/*
    ==================================================================================================
    |  errorhandler.h                                                                                |
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

    This class is called anytime there is an error pressent.
*/

#include <QMessageBox>

class ErrorHandler : public QMessageBox
{
    Q_OBJECT
public:
    ErrorHandler(QWidget * parent = 0);

public slots:
    void errorSlot(QString);

};

#endif // ERRORHANDLER_H
