TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lpthread

SOURCES += \
    main.cpp \
    ../src/SimpleTimer.cpp \
    ../src/TimingWheelTimer.cpp

HEADERS += \
    ../src/AbstractTimer.h \
    ../src/SimpleTimer.h \
    ../src/TimingWheelTimer.h \
    ../src/SingleTon.h
