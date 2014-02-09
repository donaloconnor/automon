include(../common.pri)
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

DEFINES += QT_EXPERIMENTAL_SOLUTION QT_STATEMACHINE_SOLUTION
#DEFINES += QT_NO_STATEMACHINE_EVENTFILTER

qtanimationframework-uselib:!qtanimationframework-buildlib {
    LIBS += -L$$QTANIMATIONFRAMEWORK_LIBDIR -l$$QTANIMATIONFRAMEWORK_LIBNAME
} else {
    include(corelib_statemachine.pri)
    contains(QT, gui) {
        include(gui_statemachine.pri)
    }
    include(animation.pri)
    SOURCES +=  $$PWD/qteasingcurve.cpp \
                $$PWD/easing.cpp \
                $$PWD/qtgraphicswidget.cpp
    HEADERS +=  $$PWD/qteasingcurve.h \
                $$PWD/qtgraphicswidget.h
}

win32 {
    contains(TEMPLATE, lib):contains(CONFIG, shared):DEFINES += Q_ANIMATION_EXPORT=__declspec(dllexport)
    else:qtanimationframework-uselib:DEFINES += Q_ANIMATION_EXPORT=__declspec(dllimport)
    else:DEFINES += Q_ANIMATION_EXPORT=
}
else {
    DEFINES += Q_ANIMATION_EXPORT=
}


