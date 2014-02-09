/*
    ==================================================================================================
    |  errorhandler.cpp                                                                              |
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

    Implementation of the ErrorHandler class
*/

#include "errorhandler.h"

ErrorHandler::ErrorHandler(QWidget * parent)
        : QMessageBox(parent)
{
    /* Set no frame around window */
    setWindowFlags(Qt::FramelessWindowHint);
    setStyleSheet("background-color: rgba(51,51,51,80%); color:beige");
}

void ErrorHandler::errorSlot(QString message)
{

    QString msg(tr(message.toLatin1()));

    setText(msg);

    /* Modal means that nothign in background can be clicked. This dialog has full attention */
    setModal(true);

    /* Launch the message */
    exec();
    exit(1); /* Somehow if it could kill itself and restart instead of just exiting */
}
