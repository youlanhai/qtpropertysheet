#-------------------------------------------------
#
# Project created by QtCreator 2015-11-21T12:35:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtpropertysheet
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qtproperty.cpp \
    qtpropertymanager.cpp \
    qtpropertyeditor.cpp \
    qtpropertyeditorfactory.cpp \
    qtpropertybrowser.cpp \
    qttreepropertybrowser.cpp

HEADERS  += mainwindow.h \
    qtproperty.h \
    qtpropertymanager.h \
    qtpropertyeditor.h \
    qtpropertyeditorfactory.h \
    qtpropertybrowser.h \
    qttreepropertybrowser.h

FORMS    += mainwindow.ui
