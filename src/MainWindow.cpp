

#include "MainWindow.h"
#include "MainMenuBar.h"
#include "PinyinMap.h"

//=== Constants ===//

// First unicode point for Chinese characters
const int CJK_CODEPOINT = 0x3400;

//=== Constructor/Destructor ===//

/* Constructor
 * Creates the window, menus, data display, etc.
 */
MainWindow::MainWindow() : _gedFile(0), _indiModel(0), _filteredModel(0) {
    // Load Translation based on user's locale
    _translator = new QTranslator();
    _translator->load("lang/GedTools_" + QLocale::system().name());
    qApp->installTranslator(_translator);
    // Window Creation
    setWindowTitle("GedTools Beta");
    setGeometry(75,100,400,250);
    // Menu Creation
    _menuBar = new MainMenuBar(this);
    setMenuBar(_menuBar);
    // Table View
    _tableView = new QTableView(this);
    // Tell the table to expand to the size of the window
    // (do this after adding it to the layout or it'll be the wrong size)
    setCentralWidget(_tableView);
    //_tableView->verticalHeader()->setStretchLastSection(true);
    // Create the status bar
    statusBar()->showMessage(tr("Open a GEDCOM file to begin."));
}

/* Destructor
 * Frees all of the memory allocated for GUI components
 * and data structures such as the GFile
 */
MainWindow::~MainWindow() {
// Children automatically deleted by Qt
    //delete _menuBar;
    //delete _tableView;
    delete _indiModel;
    delete _gedFile;
}

//=== Private Helper Methods ===//

/* Resets the table model and related objects
 * Uses indiModel as the new display model
 */
void MainWindow::resetDisplayModel(QAbstractItemModel * model) {
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

//=== Menu Action Methods ===//

void MainWindow::openFile() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open GEDCOM File"), "", tr("GEDCOM Files (*.ged)"));
    if (!fileName.isEmpty()) {
        try { // GFile may throw an exception on bad GEDCOM files
            GFile * gedFile = new GFile(fileName);
            GIndiMap & indiMap = gedFile->indiMap();
            QAbstractItemModel * indiModel = new GIndiModel(indiMap);
            _tableView->setModel(indiModel);
            delete _gedFile;
            resetDisplayModel(indiModel);
            _gedFile = gedFile;
            _menuBar->enableLockedItems();
            statusBar()->showMessage(tr("File opened."));
        }
        catch (...) {
            QMessageBox::critical(this, tr("Error"), fileName.prepend(tr("Unable to open file:\n")));
        }
    }
}

void MainWindow::saveFile() {
    // TODO: check for file write fails
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save GEDCOM File"), "", tr("GEDCOM Files (*.ged)"));
    if (!fileName.isEmpty()) {
        if (_gedFile->saveFile(fileName)) {
            statusBar()->showMessage(tr("File saved."));
        }
        else {
            statusBar()->showMessage(tr("Save failed."));
        }
    }
}

void MainWindow::appendPinyin() {
    // Todo: Separate this from the GUI
    int incompleteCount = 0;
    try { // PinyinMap may throw an exception if there's an IO problem
        PinyinMap pinyinMap;
        GIndiMap & indiMap = _gedFile->indiMap();
        GIndiMap::Iterator i, end = indiMap.end();
        QString name, romanName, pinyin;
        bool needSpace;
        // If there are no hanzi then we don't need a romanized name
        bool hasHanzi;
        // If this romanization has a "?" then it's incomplete
        bool incomplete;
        // Loop through every GIndiEntry in the GIndiMap
        for (i=indiMap.begin();i!=end;++i) {
            // Reset variables for this name entry
            name = i.value()->name();
            romanName = "";
            hasHanzi = false;
            needSpace = false;
            incomplete = false;
            // Loop through each character in their name to append Pinyin
            for (int j=0;j<name.length();++j) {
                // If it's a Hanzi then append pinyin for it
                if (name[j] >= CJK_CODEPOINT) {
                    // Append space if needed
                    if (needSpace) {
                        romanName.append(' ');
                    }
                    hasHanzi = true;
                    // Find the pinyin entry for this hanzi
                    pinyin = pinyinMap.lookup(name[j]);
                    // If pinyin data is found then append it
                    if (!pinyin.isNull()) {
                        romanName.append(pinyin);
                        needSpace = true;
                    }
                    // If there is no pinyin found then append "?"
                    else {
                        romanName.append('?');
                        needSpace = true;
                        incomplete = true;
                    }
                }
                // Otherwise append whatever the character is
                else {
                    romanName.append(name[j]);
                    needSpace = false;
                }
            }
            // Update the romanized name entry if needed
            if (hasHanzi) {
                i.value()->setRomanizedName(romanName);
            }
            // If this entry was incomplete then increment the count
            if (incomplete) {
                ++incompleteCount;
            }

        }
        // TODO: Find a better way to do update the display
        // Maybe using setData on the indiModel instead?
        // Update the View
        //_tableView->setModel(0);
        QAbstractItemModel * indiModel = new GIndiModel(indiMap);
        resetDisplayModel(indiModel);
        // TODO: Display "?" count (number of hanzi without pinyin)
        QString statusMsg = tr("Pinyin added successfully!");
        if (incompleteCount == 0) {
            statusBar()->showMessage(statusMsg);
        }
        else {
            statusMsg.append(tr(" Incomplete Entries: "));
            statusMsg.append(QString::number(incompleteCount));
            statusBar()->showMessage(statusMsg);
        }
    }
    catch (QString msg) {
        QMessageBox::critical(this, tr("Error"), tr("Unable to read PinyinMap.dat"));
    }
}

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

void MainWindow::switchLanguage(QAction * source) {
    _translator->load("lang/GedTools_" + source->data().toString());
    _menuBar = new MainMenuBar(this);
    setMenuBar(_menuBar);
    if (_gedFile) {
        _menuBar->enableLockedItems();
    }
    resetDisplayModel(_indiModel);
}

void MainWindow::launchWebsite() {
    QDesktopServices::openUrl(QUrl(tr("http://ouuuuch.phoenixteam.org/released/gedTools/")));
}

void MainWindow::displayAbout() {
    QMessageBox::about(this, tr("About GedTools"), tr(
        "GedTools\n"
        "Copyright \xA9 2009 Nick Vrvilo\n"
        "http://ouuuuch.phoenixteam.org/\n\n"
        "GedTools is distributed under the GNU General Public License version 3\n"
        "See the accompanying gpl-3.0.txt for details, or visit\n"
        "http://www.gnu.org/copyleft/gpl.html"
    ));
}
