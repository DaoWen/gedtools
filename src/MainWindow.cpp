
#include "MainWindow.h"

//=== Constructor/Destructor ===//

/* Constructor
 * Creates the window, menus, data display, etc.
 */
MainWindow::MainWindow() {
    setWindowTitle("GedTools Beta");
    gedFile = new GFile("chen.ged");
    GIndiMap & indiMap = gedFile->indiMap();
    //GIndiMap indiMap;
    indiModel = new GIndiModel(indiMap);
    // Menu Creation
    createMenus();
    // Table View
    tableView = new QTableView();
    tableView->setModel(indiModel);
    // Tell the table to expand to the size of the window
    // (do this after adding it to the layout or it'll be the wrong size)
    setCentralWidget(tableView);
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->verticalHeader()->setStretchLastSection(true);
    // Create the status bar
    statusBar()->showMessage(tr("Open a GEDCOM file to begin."));
}

/* Destructor
 * Frees all of the memory allocated for GUI components
 */
MainWindow::~MainWindow() {
}

//=== Menu Action Methods ===//

void MainWindow::openFile() {
    statusBar()->showMessage(tr("File Opened."));
}

//=== Private Helper Methods ===//

void MainWindow::createMenus() {
// File Menu
    fileMenu = menuBar()->addMenu(tr("&File"));
    // Open
    openAct = new QAction(tr("&Open"), this);
    openAct->setShortcut(tr("Ctrl+O"));
    openAct->setStatusTip(tr("Open a GEDCOM file for processing"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openFile()));
    fileMenu->addAction(openAct);
    // Exit
    exitAct = new QAction(tr("&Quit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Quit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
    fileMenu->addAction(exitAct);
}
