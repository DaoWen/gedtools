#ifndef V_MAINWINDOW_H
#define V_MAINWINDOW_H

#include <QtGui>
#include <QtCore>
#include "GFile.h"
#include "GIndiModel.h"

/* MainWindow: Main Window for the GedTools GUI
 * This window is the heart of the GedTools GUI,
 * containing the main menu, which provides all
 * of the functionality of the application, as
 * well as containing the TableView, which is
 * the primary means of viewing the data
 * contained within an opened GEDCOM file.
 */
class MainWindow : public QMainWindow {

Q_OBJECT // Qt Library Macro

public:

    //=== Constructor/Destructor ===//

    /* Constructor
     * Creates the window, menus, data display, etc.
     */
    MainWindow();

    /* Destructor
     * Frees all of the memory allocated for GUI components
     * and data structures such as the GFile
     */
    ~MainWindow();

private:

    //=== Private Data Members ===//

    // Data from GEDCOM file opened by the user
    GFile * _gedFile;

    // Model for displaying individual's data in _tableView
    QAbstractItemModel * _indiModel;

    //=== GUI Components ===//

    QMenuBar * _menuBar;

    QTableView * _tableView;

    //=== Private Helper Methods ===//

public slots:

    //=== Menu Action Methods ===//

    void openFile();

    void saveFile();

    void appendPinyin();

};

#endif
