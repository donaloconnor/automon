INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += $$PWD/qtguistatemachine.cpp
!contains(DEFINES, QT_NO_STATEMACHINE_EVENTFILTER) {
    HEADERS += \
	   $$PWD/qtkeyeventtransition.h \
	   $$PWD/qtmouseeventtransition.h \
	   $$PWD/qtbasickeyeventtransition_p.h \
	   $$PWD/qtbasicmouseeventtransition_p.h
    SOURCES += \
	   $$PWD/qtkeyeventtransition.cpp \
	   $$PWD/qtmouseeventtransition.cpp \
	   $$PWD/qtbasickeyeventtransition.cpp \
	   $$PWD/qtbasicmouseeventtransition.cpp
}
