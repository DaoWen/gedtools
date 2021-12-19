
#include "MainWindow.h"
#include "MainMenuBar.h"
#include "TreeWindow.h"

#include "GPinyinAppender.h"
#include "GFamilyTreeModel.h"
#include "GDateEstimator.h"

//=== Constants ===//

// Program Version
const char MainWindow::VERSION_NUMBER[] = "2021.12.19";
const char MainWindow::COPYRIGHT_YEAR[] = "2021";

// Settings file
static const char * SETTINGS_ORG = "ouuuuch.github.io";
static const char * SETTINGS_APP = "GedTools";
static const char * SETTINGS_KEY_NO_UPDATES = "disableAutoUpdates";
static const char * SETTINGS_KEY_RECENT_LOCATIONS = "recentLocations";
static const int MAX_RECENT_LOCATIONS = 10;

//=== Constructor/Destructor ===//

/* Constructor
 * Creates the window, menus, data display, etc.
 */
MainWindow::MainWindow() : _gedFile(0), _indiModel(0), _filteredModel(0), _trees(0) {
    _settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, SETTINGS_ORG, SETTINGS_APP);
    // Load Translation based on user's locale
    _appTranslator = new QTranslator();
    _appTranslator->load("lang/GedTools_" + QLocale::system().name());
    qApp->installTranslator(_appTranslator);
    _qtTranslator = new QTranslator();
    _qtTranslator->load("qt_" + QLocale::system().name());
    qApp->installTranslator(_qtTranslator);
    // Window Creation
    setWindowTitle(tr("GedTools"));
    setGeometry(75,100,600,450);
    // Menu Creation
    _menuBar = new MainMenuBar(this);
    setMenuBar(_menuBar);
    // Table View
    _tableView = new QTableView(this);
    _tableView->setSortingEnabled(true);
    // Tell the table to expand to the size of the window
    // (do this after adding it to the layout or it'll be the wrong size)
    setCentralWidget(_tableView);
    //_tableView->verticalHeader()->setStretchLastSection(true);
    // Create the status bar
    statusBar()->showMessage(tr("Open a GEDCOM file to begin."));
    if (this->autoUpdateEnabled()) {
        checkForUpdates();
    }
    QStringList args = qApp->arguments();
    // Open file if one was passed in
    if (args.size() > 1) {
        openFile(args.at(1));
    }
}

/* Destructor
 * Frees all of the memory allocated for GUI components
 * and data structures such as the GFile
 */
MainWindow::~MainWindow() {
// Children automatically deleted by Qt
    //delete _menuBar;
    //delete _tableView;
    clearFamilyTrees();
    delete _indiModel;
    delete _filteredModel;
    delete _gedFile;
    delete _appTranslator;
    delete _qtTranslator;
    delete _settings;
}

//=== Private Helper Methods ===//

/* Builds the GFamilyTree objects for this GEDCOM file
 * and stores them in the _trees class-level variable
 */
void MainWindow::createFamilyTrees() {
    // Create the list if necessary
    if (!_trees) _trees = new QList<GFamilyTree *>();
    // Destroy any pre-existing trees first
    else qDeleteAll(*_trees);
    // Build the trees
    GIndiMap & indiMap = _gedFile->indiMap();
    GFamilyMap & famMap = _gedFile->familyMap();
    GFamily * f;
    foreach (f, famMap) {
        if (f->isTreeRoot(indiMap)) {
            _trees->append(new GFamilyTree(f,famMap,indiMap));
        }
    }
    _indiMap = &indiMap;
}

/* Deletes the _trees variable and its elements */
void MainWindow::clearFamilyTrees() {
    if (_trees) {
        qDeleteAll(*_trees);
        delete _trees;
        _trees = 0;
    }
}

/* Resets the table model and related objects
 * Uses indiModel as the new display model
 */
void MainWindow::resetDisplayModel(GIndiModel * model) {
    if (_indiModel != model) {
        delete _filteredModel;
        delete _indiModel;
        _filteredModel = 0;
        _indiModel = model;
    }
    _menuBar->clearFilterToggle();
    _tableView->setModel(model);
    _tableView->resizeColumnsToContents();
    _tableView->horizontalHeader()->setStretchLastSection(true);
}

/* Queries the GedTools website to see if
 * there is a new version of GedTools available
 */
void MainWindow::checkForUpdates() {
    UpdateChecker * checker = new UpdateChecker(
      "http://ouuuuch.phoenixteam.org/released/gedTools/version/",
      VERSION_NUMBER
    );
    connect(checker, SIGNAL(checkFinished(const UpdateChecker *)),
      this, SLOT(updateCheckFinished(const UpdateChecker *)));
    checker->check();
}

/* Open a GEDCOM file */
void MainWindow::openFile(QString fileName) {
    if (!fileName.isEmpty()) {
        try { // GFile may throw an exception on bad GEDCOM files
            // Attempt to open the new file and parse the data
            GFile * gedFile = new GFile(fileName);
            GIndiMap & indiMap = gedFile->indiMap();
            GIndiModel * indiModel = new GIndiModel(indiMap);
            // Clear old data
            delete _gedFile;
            clearFamilyTrees();
            // Display new data
            resetDisplayModel(indiModel);
            // Store new data
            _gedFile = gedFile;
            // Enable menu items that require an open GEDCOM file
            _menuBar->enableLockedItems();
            // Display success message
            statusBar()->showMessage(tr("File opened."));
        }
        catch (...) {
            QMessageBox::critical(this, tr("Error"), tr("Unable to open file:\n").append(fileName));
        }
    }
}

/* helper for default location drop-down values */
static void makeDefaultRecentLocations(QStringList & result) {
    QString taiwan;
    taiwan.resize(2);
    taiwan[0] = QChar(0x81fa); // Tai2
    taiwan[1] = QChar(0x7063); // Wan1
    result.append(taiwan);
    QString china;
    china.resize(2);
    china[0] = QChar(0x4e2d); // Zhong1
    china[1] = QChar(0x570b); // Guo2
    result.append(china);
}

/* Load recent default locations from local settings file */
void MainWindow::loadRecentLocations(QStringList & result) {
    QVariant dataFromSettings = _settings->value(SETTINGS_KEY_RECENT_LOCATIONS);
    if (!dataFromSettings.isValid()) {
        makeDefaultRecentLocations(result);
    }
    else {
        result = dataFromSettings.toStringList();
        if (result.isEmpty()) {
            makeDefaultRecentLocations(result);
        }
    }
}

/* Save recent default locations to local settings file */
void MainWindow::saveRecentLocations(QString & newLocation, QStringList & recentLocations) {
    // Save location in recent locations
    int index = recentLocations.indexOf(newLocation);
    if (index != 0) {
        if (index > 0) {
            recentLocations.move(index, 0);
        }
        else {
            recentLocations.prepend(newLocation);
        }
        if (recentLocations.size() > MAX_RECENT_LOCATIONS) {
            recentLocations = recentLocations.mid(0, MAX_RECENT_LOCATIONS);
        }
        _settings->setValue(SETTINGS_KEY_RECENT_LOCATIONS, recentLocations);
        _settings->sync();
    }
}

//=== Menu Action Methods ===//

/* Prompt the user to choose a GEDCOM file and open it */
void MainWindow::openFileFromDialog() {
    QString fileName = QFileDialog::getOpenFileName(
      this, tr("Open GEDCOM File"),
      QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation),
      tr("GEDCOM Files (*.ged)")
    );
    openFile(fileName);
}

/* Save current GEDCOM file */
void MainWindow::saveFile() {
    // TODO: check for file write fails
    QString fileName = QFileDialog::getSaveFileName(
      this, tr("Save GEDCOM File"),
      QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation),
      tr("GEDCOM Files (*.ged)")
    );
    if (!fileName.isEmpty()) {
        if (_gedFile->saveFile(fileName)) {
            statusBar()->showMessage(tr("File saved."));
        }
        else {
            statusBar()->showMessage(tr("Save failed."));
        }
    }
}

/* Append Hanyu Pinyin data */
void MainWindow::appendPinyin() {
    // Todo: Separate this from the GUI
    try { // PinyinMap may throw an exception if there's an IO problem
        // Append pinyin data to all individuals in the GIndiMap
        GPinyinAppender pinyinAppender;
        int response = QMessageBox::information(
          this, tr("Replace Pinyin"),
          tr("Do you want to replace existing pinyin entries?"
          ), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes
        );
        _indiModel->invalidateViews();
        int incompleteCount = pinyinAppender.appendTo(_gedFile->indiMap(), response == QMessageBox::Yes);
        // Update the Model/View now that data has been changed
        _indiModel->revalidateViews();
        // Update status bar message
        QString statusMsg = tr("Pinyin added successfully!");
        // Display "?" count (number of hanzi without pinyin)
        if (incompleteCount > 0) { // Display "?" count (hanzi w/o pinyin)
            statusMsg.append(tr(" Incomplete Entries: "));
            statusMsg.append(QString::number(incompleteCount));
        }
        statusBar()->showMessage(statusMsg);
    }
    catch (QString msg) {
        QMessageBox::critical(this, tr("Error"), tr("Unable to read PinyinMap.dat"));
    }
}

/* Toggles display between the full set
 * of names, and the filtered set containg
 * only entries with incomplete pinyin.
 */
void MainWindow::filterIncomplete(bool checked) {
    // Make sure the filtered model exists
    if (!_filteredModel) {
        _filteredModel = new QSortFilterProxyModel();
        _filteredModel->setSourceModel(_indiModel);
    }
    // Filter the display for ROMN fields containing "?"
    if (checked) {
        _filteredModel->setFilterRegExp(QRegExp(".*\\?.*"));
        _filteredModel->setFilterKeyColumn(GIndiModel::ROMAN_COL);
        _filteredModel->sort(GIndiModel::NAME_COL, Qt::DescendingOrder);
        // Don't change the display if it's empty
        if (_filteredModel->rowCount() == 0) {
            _menuBar->clearFilterToggle();
            QMessageBox::information(this, tr("Notice"), tr("No incomplete entries to display"));
        }
        else {
            _tableView->setModel(_filteredModel);
        }
    }
    // Return to the normal display
    else {
        _tableView->setModel(_indiModel);
    }
}

/* Open a new tree view to view all families */
void MainWindow::viewTree() {
    // Build the family trees first if necessary
    if (!_trees) createFamilyTrees();
    // Create and display the gui elements in a model window
    GFamilyTreeModel * treeModel = new GFamilyTreeModel(*_trees);
    TreeWindow tw(treeModel, _gedFile, this);
    tw.exec(); // Hangs here until the new window is closed
    // Destroy the model when the window is closed
    delete treeModel;
}

/* Estimate missing dates in the family tree */
void MainWindow::estimateDates() {
    // Build the family trees first if necessary
    if (!_trees) createFamilyTrees();
    // Load default location data (persisted across sessions)
    QStringList recentLocations;
    loadRecentLocations(recentLocations);
    // Prompt for a default location
    bool okPressed;
    QString defaultLocation = QInputDialog::getItem(this,
            tr("Enter Default Location"),
            tr("Automatically use this location to\nfill in blank birth, marriage and death places:"),
            recentLocations, 0, true, &okPressed);
    // Continue only if the user pressed OK
    if (okPressed) {
        saveRecentLocations(defaultLocation, recentLocations);
        // Estimate the dates
        const bool adoptOpt = _menuBar->usingAdoptedRelations();
        const bool deceasedOpt = _menuBar->usingDeceasedOver110();
        const QString datePrefix = _menuBar->markingEstimated() ? "EST " : "ABT ";
        GDateEstimator estimator(*_trees, *_indiMap, defaultLocation, datePrefix, adoptOpt, deceasedOpt);
        _indiModel->invalidateViews();
        try {
            int datesAdded = estimator.estimateMissingDates();
            // Alert the user as to how many dates were appened
            statusBar()->showMessage(tr("%1 new dates were added").arg(datesAdded));
        }
        catch (QString e) {
            QMessageBox::critical(this, tr("Error"), e);
        }
        // Update the Model/View now that data has been changed
        _indiModel->revalidateViews();
    }
}

/* Switch between GUI translations */
void MainWindow::switchLanguage(QAction * source) {
    QString languageCode = source->data().toString();
    // Set translators
    _appTranslator->load("lang/GedTools_" + languageCode);
    _qtTranslator->load("lang/qt_" + languageCode);
    // Rebuild the menus and display area
    _menuBar = new MainMenuBar(this);
    setMenuBar(_menuBar);
    if (_gedFile) {
        _menuBar->enableLockedItems();
    }
    resetDisplayModel(_indiModel);
}

/* Launch browser to view the GedTools website */
void MainWindow::launchWebsite() {
    if (!QDesktopServices::openUrl(QUrl(tr("http://ouuuuch.phoenixteam.org/released/gedTools/")))) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to open web browser"));
    }
}

/* Launch browser to view the GedTools bug report page */
void MainWindow::launchBugReport() {
    if (!QDesktopServices::openUrl(QUrl(tr("http://ouuuuch.phoenixteam.org/bugs/?prog=GedTools")))) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to open web browser"));
    }
}

/* Automatic update predicate */
bool MainWindow::autoUpdateEnabled() {
    return !_settings->value(SETTINGS_KEY_NO_UPDATES).toBool();
}

/* Enable/Disable automatic update checks */
void MainWindow::setAutoUpdate(bool enabled) {
    _settings->setValue(SETTINGS_KEY_NO_UPDATES, !enabled);
    _settings->sync();
}

/* Display "About" dialog */
void MainWindow::displayAbout() {
    QMessageBox::about(this, tr("About GedTools"), QString(tr(
        "GedTools v%1\n"
        "Copyright \xA9 2009-%2 Nick Vrvilo\n"
        "http://ouuuuch.phoenixteam.org/\n\n"
        "GedTools is distributed under the GNU General Public License version 3\n"
        "See the accompanying gpl-3.0.txt for details, or visit\n"
        "http://www.gnu.org/copyleft/gpl.html"
    )).arg(VERSION_NUMBER, COPYRIGHT_YEAR));
}

//=== Other Actions ===//

/* Called when the update check has been completed
 */
void MainWindow::updateCheckFinished(const UpdateChecker * checker) {
    if (checker->hasUpdate()) {
        int response = QMessageBox::information(
          this, tr("Update"),
          tr("A new version of GedTools is available for download.\n"
             "Would you like to visit the GedTools website?"
          ), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes
        );
        if (response == QMessageBox::Yes) {
            launchWebsite();
        }
    }
}
