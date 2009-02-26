
#include "MainWindow.h"
#include "MainMenuBar.h"

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
}

void MainWindow::saveFile() {
    if (!_gedFile) {
        statusBar()->showMessage(tr("No data to save."));
    }
    else {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Save GEDCOM File"), "", tr("GEDCOM Files (*.ged)"));
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
