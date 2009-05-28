
#include "MainMenuBar.h"
#include "MainWindow.h"

/* Constructor
 * Builds the menus for the
 * main window of the GUI
 */
MainMenuBar::MainMenuBar(MainWindow * mainWin) : QMenuBar(mainWin) {
// TODO: Split this into sub-functions or something
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
    _saveAct->setEnabled(false); // Enable after a file is opened
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
    _appendPinyinAct->setShortcut(tr("F5"));
    _appendPinyinAct->setStatusTip(tr("Append Pinyin phonetic data to all entries in this GEDCOM file"));
    _appendPinyinAct->setEnabled(false); // Enable after a file is opened
    connect(_appendPinyinAct, SIGNAL(triggered()), mainWin, SLOT(appendPinyin()));
    _pinyinMenu->addAction(_appendPinyinAct);
    // Display Missing Pinyin (toggle item)
    _filterIncompleteAct = new QAction(tr("Display &Missing (?) Pinyin"), this);
    _filterIncompleteAct->setStatusTip(tr("Filter the display table to show only entries with missing (?) pinyin data"));
    _filterIncompleteAct->setShortcut(tr("F8"));
    _filterIncompleteAct->setCheckable(true);
    _filterIncompleteAct->setEnabled(false); // Enable after a file is opened
    connect(_filterIncompleteAct, SIGNAL(toggled(bool)), mainWin, SLOT(filterIncomplete(bool)));
    _pinyinMenu->addAction(_filterIncompleteAct);
// Family Tree Menu
    _toolsMenu = addMenu(tr("Family &Tree"));
    // View Family Tree
    _viewTreeAct = new QAction(tr("View Family Tree"), this);
    _viewTreeAct->setStatusTip(tr("View families in a tree format"));
    _viewTreeAct->setShortcut(tr("F10"));
    _viewTreeAct->setEnabled(false); // Enable after a file is opened
    connect(_viewTreeAct, SIGNAL(triggered()), mainWin, SLOT(viewTree()));
    _toolsMenu->addAction(_viewTreeAct);
    // Estimate Missing Dates
    _estimateDatesAct = new QAction(tr("Estimate Missing Dates"), this);
    _estimateDatesAct->setStatusTip(tr("Calculate estimated values for missing dates"));
    _estimateDatesAct->setShortcut(tr("Ctrl+E"));
    _estimateDatesAct->setEnabled(false); // Enable after a file is opened
    connect(_estimateDatesAct, SIGNAL(triggered()), mainWin, SLOT(estimateDates()));
    _toolsMenu->addAction(_estimateDatesAct);
// Language Menu
    _languageMenu = addMenu(tr("Language"));
    _languageActionGroup = new QActionGroup(this);
    connect(_languageActionGroup, SIGNAL(triggered(QAction *)), mainWin, SLOT(switchLanguage(QAction *)));
    // Loop Variables
    QString currentLanguage = tr("English","Current Language Name");
    QString locale;
    QString language;
    QAction * action;
    // Get the translation files
    QDir qmDir = QDir("./lang/");
    QStringList fileNames = qmDir.entryList(QStringList("GedTools_*.qm"));
    for (int i = 0; i < fileNames.size(); ++i) {
        locale = fileNames[i];
        locale.remove(0, locale.indexOf('_') + 1);
        locale.chop(3);
        QTranslator translator;
        translator.load(fileNames[i], qmDir.absolutePath());
        language = translator.translate("MainMenuBar","English","Current Language Name");
        action = new QAction(QString("&%1 %2").arg(i + 1).arg(language), this);
        action->setCheckable(true);
        action->setData(locale);
        _languageMenu->addAction(action);
        _languageActionGroup->addAction(action);
        if (language == currentLanguage)
            action->setChecked(true);
    }
// Help Menu
    _helpMenu = addMenu(tr("&Help"));
    // Visit Website
    _websiteAct = new QAction(tr("Visit GedTools &Website"), this);
    _websiteAct->setStatusTip(tr("Go to the GedTools web page"));
    connect(_websiteAct, SIGNAL(triggered()), mainWin, SLOT(launchWebsite()));
    _helpMenu->addAction(_websiteAct);
    // About
    _aboutAct = new QAction(tr("&About"), this);
    _aboutAct->setStatusTip(tr("Information about GedTools"));
    connect(_aboutAct, SIGNAL(triggered()), mainWin, SLOT(displayAbout()));
    _helpMenu->addAction(_aboutAct);
}

MainMenuBar::~MainMenuBar() {
// All child objects are automatically deleted by Qt
}

/* Used to clear the checkmark on the
 * "Display Missing Pinyin" item
 */
void MainMenuBar::clearFilterToggle() {
    _filterIncompleteAct->setChecked(false);
}

/* Enables certain menu items that are only useful
 * after a GEDCOM file has been opened for editing
 */
void MainMenuBar::enableLockedItems() {
    _saveAct->setEnabled(true);
    _appendPinyinAct->setEnabled(true);
    _filterIncompleteAct->setEnabled(true);
    _viewTreeAct->setEnabled(true);
    _estimateDatesAct->setEnabled(true);
}
