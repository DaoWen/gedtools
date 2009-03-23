#ifndef V_MAINWINDOW_H
#define V_MAINWINDOW_H

#include <QtGui>
#include <QtCore>
#include "GFile.h"
#include "GIndiModel.h"
#include "MainMenuBar.h"

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

    // Translator for the application
    QTranslator * _translator;

    // Data from GEDCOM file opened by the user
    GFile * _gedFile;

    // Model for displaying individual's data in _tableView
    QAbstractItemModel * _indiModel;

    // Filtered display model
    QSortFilterProxyModel * _filteredModel;

    //=== GUI Components ===//

    MainMenuBar * _menuBar;

    QTableView * _tableView;

    //=== Private Helper Methods ===//

    /* Resets the table view and related objects
     * Uses indiModel as the new display model
     */
    void resetDisplayModel(QAbstractItemModel * model);

public slots:

    //=== Menu Action Methods ===//

    void openFile();

    void saveFile();

    void appendPinyin();

    /* Toggles display between the full set
      * of names, and the filtered set containg
      * only entries with incomplete pinyin.
      */
    void filterIncomplete(bool checked);

    void switchLanguage(QAction * source);

    void launchWebsite();

    void displayAbout();

};

#endif
