
#include "MainMenuBar.h"
#include "MainWindow.h"

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
    connect(_openAct, SIGNAL(triggered()), mainWin, SLOT(openFileFromDialog()));
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
    // Use adoptions?
    _useAdoptionsToggleAct = new QAction(tr("Use &Adoptive Relations"), this);
    _useAdoptionsToggleAct->setCheckable(true);
    _useAdoptionsToggleAct->setChecked(true);
    _useAdoptionsToggleAct->setStatusTip(tr("Use adoptive relationships in the family tree when estimating dates."));
    _useAdoptionsToggleAct->setShortcut(tr("Ctrl+U"));
    _toolsMenu->addAction(_useAdoptionsToggleAct);
    // Add DECEASED annotation for people over 110 years old?
    _useDeceasedOver110ToggleAct = new QAction(tr("Use &Deceased after 110"), this);
    _useDeceasedOver110ToggleAct->setCheckable(true);
    _useDeceasedOver110ToggleAct->setChecked(false);
    _useDeceasedOver110ToggleAct->setStatusTip(tr("Automatically add DECEASED annotation to indivudals over 110 years old."));
    _useDeceasedOver110ToggleAct->setShortcut(tr("Ctrl+D"));
    _toolsMenu->addAction(_useDeceasedOver110ToggleAct);
    // Mark estimated dates with EST annotation
    _markingEstimated = new QAction(tr("&Mark estimated dates with \"EST\""), this);
    _markingEstimated->setCheckable(true);
    _markingEstimated->setChecked(false);
    _markingEstimated->setStatusTip(tr("Mark estimated dates with \"EST\" rather than \"ABT\"."));
    _markingEstimated->setShortcut(tr("Ctrl+M"));
    _toolsMenu->addAction(_markingEstimated);
    // Estimate Missing Dates
    _estimateDatesAct = new QAction(tr("Estimate Missing Dates"), this);
    _estimateDatesAct->setStatusTip(tr("Calculate estimated values for missing dates"));
    _estimateDatesAct->setShortcut(tr("Ctrl+E"));
    _estimateDatesAct->setEnabled(false); // Enable after a file is opened
    connect(_estimateDatesAct, SIGNAL(triggered()), mainWin, SLOT(estimateDates()));
    _toolsMenu->addAction(_estimateDatesAct);
// Language Menu
    _languageMenu = addMenu(tr("&Language"));
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
        if (language == currentLanguage) action->setChecked(true);
    }
// Help Menu
    _helpMenu = addMenu(tr("&Help"));
    // Visit Website
    _websiteAct = new QAction(tr("Visit GedTools &Website"), this);
    _websiteAct->setStatusTip(tr("Go to the GedTools web page"));
    connect(_websiteAct, SIGNAL(triggered()), mainWin, SLOT(launchWebsite()));
    _helpMenu->addAction(_websiteAct);
    // Report a Bug
    _bugReportAct = new QAction(tr("&Report Bugs"), this);
    _bugReportAct->setStatusTip(tr("Go to the GedTools bug report page"));
    connect(_bugReportAct, SIGNAL(triggered()), mainWin, SLOT(launchBugReport()));
    _helpMenu->addAction(_bugReportAct);
    // Enable/Disable Auto Updates (toggle item)
    _enableUpdateAct = new QAction(tr("Enable &Automatic Update Checks"), this);
    _enableUpdateAct->setStatusTip(tr("Allow GedTools to automatically check for newer versions"));
    _enableUpdateAct->setCheckable(true);
    _enableUpdateAct->setChecked(mainWin->autoUpdateEnabled());
    connect(_enableUpdateAct, SIGNAL(toggled(bool)), mainWin, SLOT(setAutoUpdate(bool)));
    _helpMenu->addAction(_enableUpdateAct);
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

/* Should adoptive relations being used? */
bool MainMenuBar::usingAdoptedRelations() {
    return _useAdoptionsToggleAct->isChecked();
}

/* Should add DECEASED annotation for people over 110 years old? */
bool MainMenuBar::usingDeceasedOver110() {
    return _useDeceasedOver110ToggleAct->isChecked();
}


/* Should add EST annotation for estimated dates?
 * Using ABT otherwise.
 */
bool MainMenuBar::markingEstimated() {
    return _markingEstimated->isChecked();
}
