#################
# Configuration #
#################
TEMPLATE = app
CONFIG += qt warn_on
QT += network
TARGET = GedTools

# Disable warnings in Qt header files
# (treat all Qt headers as system header files)
#QMAKE_CXXFLAGS += -isystem $$[QT_INSTALL_HEADERS]

# Windows default stack size of 1MB is too small!
# Linux pthreads have 8MB stacks by default.
# Bump it up to 16MB to be safe (we don't make many threads anyway).
# https://doc.qt.io/archives/qt-4.8/qmake-variable-reference.html#qmake-lflags-windows
# https://stackoverflow.com/a/3565282/1427124
QMAKE_LFLAGS_WINDOWS += -Wl,--stack,16000000

#########################
# Windows Resource File #
#########################
RC_FILE = res/GedTools.rc

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
          inc/GPinyinAppender.h \
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
          src/GPinyinAppender.cpp \
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
