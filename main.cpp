/*
    ==================================================================================================
    |  main.cpp                                                                                |
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

    This is the entry point of the Automon Application. It sets up an Application instance and looks after
    handling the splash screen.
*/
#include <QApplication>
#include <QFile>
#include <QWidget>
#include <QSplashScreen>
#include "automonapp.h"

int main(int argc, char *argv[])
{
    /* Create an application */
    QApplication a(argc, argv);

    /* Set up styling of application */
    QFile styleSheet(":/files/automon.qss");

    /* Open file */
    if (!styleSheet.open(QIODevice::ReadOnly)) {
        qWarning("Unable to open :/files/automon.qss");
        exit(0);
    }

    /* Apply stylesheet to application */
    a.setStyleSheet(styleSheet.readAll());

    /* Disable the cursor since using touch screen */
//    a.setOverrideCursor( QCursor( Qt::BlankCursor )); [LA]

    /* Open Splashscreen image, create a SplashScreen with it and show splash screen */
    QPixmap pixmap(":/files/splashscreen.png");
    QSplashScreen splash(pixmap);
    splash.show();


    /* Start our automon application */
    AutomonApp automon(&splash);
//    automon.setCursor(QCursor(Qt::BlankCursor)); [LA]
    automon.show();

    /* Finsih the splashscreen once Automon has loaded */
    splash.finish(&automon);

    /* Run event loop */
    return a.exec();
}
