#-------------------------------------------------
#
# Project created by QtCreator 2020-09-02T18:54:29
#
#-------------------------------------------------

QT       += core gui widgets multimedia
QT += multimedia multimediawidgets
CONFIG += resources_big

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = C_Game_project
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


SOURCES += main.cpp \
    mainwindow.cpp \
    calllordthread.cpp \
    cardpic.cpp \
    cards.cpp \
    gamecontrol.cpp \
    gamemusic.cpp \
    gamepanel.cpp \
    hand.cpp \
    markboard.cpp \
    player.cpp \
    playhandthread.cpp \
    robot.cpp \
    scoreshow.cpp \
    strategy.cpp \
    userplayer.cpp

HEADERS  += \
    mainwindow.h \
    calllordthread.h \
    cardpic.h \
    cards.h \
    gamecontrol.h \
    gamemusic.h \
    gamepanel.h \
    hand.h \
    markboard.h \
    player.h \
    playhandthread.h \
    robot.h \
    scoreshow.h \
    strategy.h \
    userplayer.h

FORMS    += \
    mainwindow.ui \
    markboard.ui \
    scoreshow.ui

RESOURCES += \
    image/res.qrc

DISTFILES +=
