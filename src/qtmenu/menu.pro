QT += core widgets
CONFIG += c++11

TARGET = sweetmenu
TEMPLATE = app

SOURCES += menu.cpp

QMAKE_CXXFLAGS += -Wall -O2 -std=gnu++11
QMAKE_LFLAGS += -lX11
