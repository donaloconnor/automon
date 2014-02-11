HEADERS += $$PWD/qtstatemachine.h \
	   $$PWD/qtstatemachine_p.h \
	   $$PWD/qtstateaction.h \
	   $$PWD/qtstateaction_p.h \
	   $$PWD/qtsignaleventgenerator_p.h \
	   $$PWD/qtabstractstate.h \
	   $$PWD/qtabstractstate_p.h \
	   $$PWD/qtactionstate.h \
	   $$PWD/qtactionstate_p.h \
	   $$PWD/qtstate.h \
	   $$PWD/qtstate_p.h \
	   $$PWD/qtfinalstate.h \
	   $$PWD/qthistorystate.h \
	   $$PWD/qthistorystate_p.h \
	   $$PWD/qtabstracttransition.h \
	   $$PWD/qtabstracttransition_p.h \
	   $$PWD/qttransition.h \
	   $$PWD/qttransition_p.h \
	   $$PWD/qtstatefinishedevent.h \
	   $$PWD/qtstatefinishedtransition.h \
	   $$PWD/qtsignalevent.h \
	   $$PWD/qtsignaltransition.h \
	   $$PWD/qtsignaltransition_p.h

SOURCES += $$PWD/qtstatemachine.cpp \
	   $$PWD/qtstateaction.cpp \
	   $$PWD/qtabstractstate.cpp \
	   $$PWD/qtactionstate.cpp \
	   $$PWD/qtstate.cpp \
	   $$PWD/qtfinalstate.cpp \
	   $$PWD/qthistorystate.cpp \
	   $$PWD/qtabstracttransition.cpp \
	   $$PWD/qttransition.cpp \
	   $$PWD/qtstatefinishedtransition.cpp \
	   $$PWD/qtsignaltransition.cpp

!contains(DEFINES, QT_NO_STATEMACHINE_EVENTFILTER) {
HEADERS += $$PWD/qtboundevent_p.h \
	   $$PWD/qteventtransition.h \
	   $$PWD/qteventtransition_p.h
SOURCES += $$PWD/qteventtransition.cpp
}

!contains(DEFINES, QT_NO_ANIMATION) {
HEADERS += $$PWD/qtanimationstate.h
SOURCES += $$PWD/qtanimationstate.cpp
}
