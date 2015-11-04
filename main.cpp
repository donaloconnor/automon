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
