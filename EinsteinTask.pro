QT += core
QT -= gui

CONFIG += c++11

TARGET = EinsteinTask
CONFIG += console
CONFIG -= app_bundle

LIBS += -lbdd

TEMPLATE = app

SOURCES += main.cpp
