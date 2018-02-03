#-------------------------------------------------
#
# Project created by QtCreator 2018-01-29T14:53:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SafeGuard
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    cmyprocess.cpp \
    csnapshot.cpp \
    cpuusage.cpp \
    cmytrush.cpp \
    md5.cpp \
    cmyvirus.cpp \
    cmycloud.cpp \
    cmyservice.cpp \
    cmyram.cpp \
    cmypushbutton.cpp \
    cmyrocket.cpp \
    cmygrub.cpp \
    cmype.cpp \
    cmynativeeventfilter.cpp

HEADERS  += mainwindow.h \
    cmyprocess.h \
    csnapshot.h \
    cpuusage.h \
    cmytrush.h \
    md5.h \
    cmyvirus.h \
    cmycloud.h \
    cmyservice.h \
    cmyram.h \
    cmypushbutton.h \
    cmyrocket.h \
    cmygrub.h \
    cmype.h \
    cmynativeeventfilter.h

LIBS += -ladvapi32 -luser32 -lShlwapi -lPowrProf -lWininet
QT += network
QT += gui
QT += widgets

FORMS    += mainwindow.ui \
    cmyprocess.ui \
    cmytrush.ui \
    cmyvirus.ui \
    cmycloud.ui \
    cmyservice.ui \
    cmyram.ui \
    cmyrocket.ui \
    cmygrub.ui \
    cmype.ui

RESOURCES += \
    resources.qrc

RC_ICONS = ico/pigeon.ico
