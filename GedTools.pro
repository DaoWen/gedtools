#################
# Configuration #
#################
TEMPLATE = app
CONFIG += qt console
QT += network
TARGET = GedTools

###############################
# Project Files & Directories #
###############################
HEADERS = inc/GFile.h \
          inc/GIndiEntry.h \
          inc/GIndiMap.h \
          inc/GIndiModel.h \
          inc/GFamily.h \
          inc/GFamilyMap.h\
          inc/GFamilyTree.h\
          inc/GFamilyTreeModel.h\
          inc/GNode.h \
          inc/GDateEstimator.h \
          inc/PinyinMap.h \
          inc/MainWindow.h \
          inc/MainMenuBar.h \
          inc/TreeWindow.h \
          inc/UpdateChecker.h
SOURCES = src/GFile.cpp \
          src/GIndiEntry.cpp \
          src/GIndiMap.cpp \
          src/GIndiModel.cpp \
          src/GFamily.cpp \
          src/GFamilyMap.cpp\
          src/GFamilyTree.cpp\
          src/GFamilyTreeModel.cpp\
          src/GNode.cpp \
          src/GDateEstimator.cpp \
          src/PinyinMap.cpp \
          src/MainWindow.cpp \
          src/MainMenuBar.cpp \
          src/TreeWindow.cpp \
          src/UpdateChecker.cpp \
          src/Main.cpp
TRANSLATIONS = lang/GedTools_en.ts \
               lang/GedTools_zh.ts \
               lang/qt_zh.ts
INCLUDEPATH += inc/
DEPENDPATH += inc/
OBJECTS_DIR = obj/
DESTDIR = bin/
MOC_DIR = moc/
