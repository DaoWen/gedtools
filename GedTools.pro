#################
# Configuration #
#################
TEMPLATE = app
CONFIG += console
TARGET = GedTools

###############################
# Project Files & Directories #
###############################
HEADERS = inc/GFile.h \
          inc/GIndiEntry.h \
          inc/GIndiMap.h \
          inc/GIndiModel.h \
          inc/GNode.h \
          inc/PinyinMap.h \
          inc/MainWindow.h \
          inc/MainMenuBar.h
SOURCES = src/GFile.cpp \
          src/GIndiEntry.cpp \
          src/GIndiMap.cpp \
          src/GIndiModel.cpp \
          src/GNode.cpp \
          src/PinyinMap.cpp \
          src/MainWindow.cpp \
          src/MainMenuBar.cpp \
          src/Main.cpp
TRANSLATIONS = lang/GedTools_en.ts \
               lang/GedTools_zh.ts
#DEPENDPATH = inc/ src/
INCLUDEPATH += inc/
OBJECTS_DIR = obj/
DESTDIR = bin/
MOC_DIR = moc/