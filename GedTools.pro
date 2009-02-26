#################
# Configuration #
#################
TEMPLATE = app
CONFIG += console
TARGET = GedTools

###############################
# Project Files & Directories #
###############################
HEADERS = GFile.h \
          GIndiEntry.h \
          GIndiMap.h \
          GIndiModel.h \
          GNode.h \
          MainWindow.h \
          MainMenuBar.h
SOURCES = GFile.cpp \
          GIndiEntry.cpp \
          GIndiMap.cpp \
          GIndiModel.cpp \
          GNode.cpp \
          MainWindow.cpp \
          MainMenuBar.cpp \
          Main.cpp
DEPENDPATH = inc/ src/
INCLUDEPATH += inc/
OBJECTS_DIR = obj/
DESTDIR = bin/
MOC_DIR = moc/