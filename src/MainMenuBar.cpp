
#include "MainMenuBar.h"

/* Constructor
 * Builds the menus for the
 * main window of the GUI
 */
MainMenuBar::MainMenuBar(MainWindow * mainWin) : QMenuBar(mainWin) {
// File Menu
    _fileMenu = addMenu(tr("&File"));
    // Open
    _openAct = new QAction(tr("&Open"), this);
    _openAct->setShortcut(tr("Ctrl+O"));
    _openAct->setStatusTip(tr("Open a GEDCOM file for processing"));
    connect(_openAct, SIGNAL(triggered()), mainWin, SLOT(openFile()));
    _fileMenu->addAction(_openAct);
    // Save
    _saveAct = new QAction(tr("&Save"), this);
    _saveAct->setShortcut(tr("Ctrl+S"));
    _saveAct->setStatusTip(tr("Save your modified GEDCOM file"));
    connect(_saveAct, SIGNAL(triggered()), mainWin, SLOT(saveFile()));
    _fileMenu->addAction(_saveAct);
    // Exit
    _exitAct = new QAction(tr("&Quit"), this);
    _exitAct->setShortcut(tr("Ctrl+Q"));
    _exitAct->setStatusTip(tr("Quit the application"));
    connect(_exitAct, SIGNAL(triggered()), mainWin, SLOT(close()));
    _fileMenu->addAction(_exitAct);
// Pinyin Menu
    _pinyinMenu = addMenu(tr("&Pinyin"));
    // Append Pinyin
    _appendPinyinAct = new QAction(tr("&Append Pinyin"), this);
    _appendPinyinAct->setStatusTip(tr("Append Pinyin phonetic data to all entries in this GEDCOM file"));
    connect(_appendPinyinAct, SIGNAL(triggered()), mainWin, SLOT(appendPinyin()));
    _pinyinMenu->addAction(_appendPinyinAct);
}

MainMenuBar::~MainMenuBar() {
    delete _openAct;
    delete _exitAct;
    delete _fileMenu;
}
