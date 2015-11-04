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
