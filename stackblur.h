#ifndef DIAL_STACKBLUR_H
#define DIAL_STACKBLUR_H

#ifndef Q_WS_X11
#define Q_WS_X11
#endif

/*
    This file is a part of the KDE project

    Copyright (c) Zack Rusin <zack@kde.org>
    Copyright (c) Fredrik Höglund <fredrik@kde.org>
    Copyright (c) Michael Pyne <michael.pyne@kdemail.net> (Header addition)

    The stack blur algorithm was invented by Mario Klingemann
    <mario@quasimondo.com>

    This implementation also incorporates performance improvements
    from Anti-Grain Geometry Version 2.4,
    Copyright (c) Maxim Shemanarev (http://www.antigrain.com)

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

void stackBlur(QImage &image, int radius);

#endif
