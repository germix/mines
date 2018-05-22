#-------------------------------------------------
#
# Project created by QtCreator 2011-06-17T07:00:46
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

DEFINES += USE_CUSTOM

#---------------------------------------------------------------------------------------------------
# Versión
#---------------------------------------------------------------------------------------------------

DEFINES += MAJOR_VERSION=1
DEFINES += MINOR_VERSION=0

#---------------------------------------------------------------------------------------------------
# Target
#---------------------------------------------------------------------------------------------------
TARGET = Mines
TARGET = $$join(TARGET,,,-1.0)

CONFIG(debug, debug|release) {
TARGET = $$join(TARGET,,,_debug)
DEFINES += DEBUG
}

#---------------------------------------------------------------------------------------------------
# Directorio de destino
#---------------------------------------------------------------------------------------------------
DESTDIR = ../bin

#---------------------------------------------------------------------------------------------------
# Archivos de código
#---------------------------------------------------------------------------------------------------

SOURCES +=		src/main.cpp\
                src/MainWindow.cpp \
                src/AboutDialog.cpp \
                src/MineBoard.cpp \
    src/Languages.cpp \
    src/CustomDifficultyDialog.cpp

HEADERS  +=		src/MainWindow.h \
                src/AboutDialog.h \
                src/MineBoard.h \
    src/Languages.h \
    src/CustomDifficultyDialog.h

FORMS    +=		src/MainWindow.ui \
                src/AboutDialog.ui \
    src/CustomDifficultyDialog.ui

#---------------------------------------------------------------------------------------------------
# Archivos de recursos
#---------------------------------------------------------------------------------------------------

RESOURCES += res/resource.qrc

win32:RC_FILE = res/resource_win32.rc

#---------------------------------------------------------------------------------------------------
# Archivos de traducción
#---------------------------------------------------------------------------------------------------
TRANSLATIONS =	translate/mines_es.ts \
                translate/mines_en.ts \
                translate/mines_it.ts
