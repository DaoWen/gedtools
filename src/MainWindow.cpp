
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
MainWindow::MainWindow() : _gedFile(0), _indiModel(0) {
    setWindowTitle("GedTools Beta");
    // Menu Creation
    _menuBar = new MainMenuBar(this);
    setMenuBar(_menuBar);
    // Table View
    _tableView = new QTableView();
    // Tell the table to expand to the size of the window
    // (do this after adding it to the layout or it'll be the wrong size)
    setCentralWidget(_tableView);
    _tableView->horizontalHeader()->setStretchLastSection(true);
    //_tableView->verticalHeader()->setStretchLastSection(true);
    // Create the status bar
    statusBar()->showMessage(tr("Open a GEDCOM file to begin."));
}

/* Destructor
 * Frees all of the memory allocated for GUI components
 * and data structures such as the GFile
 */
MainWindow::~MainWindow() {
    delete _tableView;
    delete _indiModel;
    delete _gedFile;
    delete _menuBar;
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
            delete _indiModel;
            delete _gedFile;
            _indiModel = indiModel;
            _gedFile = gedFile;
            statusBar()->showMessage(tr("File opened."));
        }
        catch (...) {
            QMessageBox::critical(this, tr("Error"), fileName.prepend(tr("Unable to open file:\n")));
        }
    }
}

void MainWindow::saveFile() {
    // TODO: check for file write fails
    // Make sure there's a file open first
    if (!_gedFile) {
        statusBar()->showMessage(tr("Open a GEDCOM file to begin."));
    }
    else {
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
}

void MainWindow::appendPinyin() {
    // Make sure there's a file open first
    if (!_gedFile) {
        statusBar()->showMessage(tr("Open a GEDCOM file to begin."));
    }
    else {
        try { // PinyinMap may throw an exception if there's an IO problem
            PinyinMap pinyinMap;
            GIndiMap & indiMap = _gedFile->indiMap();
            GIndiMap::Iterator i, end = indiMap.end();
            QString name, romanName, pinyin;
            bool needSpace;
            // Loop through every GIndiEntry in the GIndiMap
            for (i=indiMap.begin();i!=end;++i) {
                name = i.value()->name();
                romanName = "";
                needSpace = false;
                // Loop through each character in their name to append Pinyin
                for (int j=0;j<name.length();++j) {
                    // If it's a Hanzi then append pinyin for it
                    if (name[j] >= CJK_CODEPOINT) {
                        // Find the pinyin entry for this hanzi
                        pinyin = pinyinMap.lookup(name[j]);
                        // If pinyin data is found then append it
                        if (!pinyin.isNull()) {
                            if (needSpace) {
                                romanName.append(' ');
                            }
                            romanName.append(pinyin);
                            needSpace = true;
                        }
                        // If there is no pinyin found then append "?"
                        else {
                            romanName.append('?');
                            needSpace = false;
                        }
                    }
                    // Otherwise append whatever the character is
                    else {
                        romanName.append(name[j]);
                        needSpace = false;
                    }
                }
                // Update the romanized name entry
                i.value()->setRomanizedName(romanName);

            }
            // TODO: Find a better way to do this
            // Maybe using setData on the indiModel instead?
            // Update the View
            //_tableView->setModel(0);
            QAbstractItemModel * indiModel = new GIndiModel(indiMap);
            _tableView->setModel(indiModel);
            delete _indiModel;
            _indiModel = indiModel;
            // TODO: Display "?" count (number of hanzi without pinyin)
            statusBar()->showMessage(tr("Pinyin added successfully!"));
        }
        catch (QString msg) {
            QMessageBox::critical(this, tr("Error"), tr("Unable to read PinyinMap.dat"));
        }
    }
}
