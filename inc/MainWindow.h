#ifndef V_MAINWINDOW_H
#define V_MAINWINDOW_H

#include <QtGui>
#include <QtCore>
#include "GFile.h"
#include "GIndiMap.h"
#include "GIndiModel.h"
#include "GFamilyTree.h"
#include "MainMenuBar.h"
#include "UpdateChecker.h"

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

    //=== Constants ===//

    // Program Version
    static const char VERSION_NUMBER[];

    // Most recent copyright year
    static const char COPYRIGHT_YEAR[];

    // File that disables auto updates
    static const char NO_UPDATE_FILE[];

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
    QTranslator * _appTranslator, * _qtTranslator;

    // Data from GEDCOM file opened by the user
    GFile * _gedFile;

    // All individuals in the input data
    GIndiMap * _indiMap;

    // Model for displaying individual's data in _tableView
    GIndiModel * _indiModel;

    // Filtered display model
    QSortFilterProxyModel * _filteredModel;

    // List of family trees in this GEDCOM file
    GFTList * _trees;

    //=== GUI Components ===//

    // The main menu bar for the main window
    MainMenuBar * _menuBar;

    // Table-style view to display the GEDCOM file data
    QTableView * _tableView;

    //=== Private Helper Methods ===//

    /* Builds the GFamilyTree objects for this GEDCOM file
     * and stores them in the _trees class-level variable
     */
    void createFamilyTrees();

    /* Deletes the _trees variable and its elements */
    void clearFamilyTrees();

    /* Resets the table view and related objects
     * Uses indiModel as the new display model
     */
    void resetDisplayModel(GIndiModel * model);

    /* Queries the GedTools website to see if
     * there is a new version of GedTools available
     */
    void checkForUpdates();

    /* Open a GEDCOM file */
    void openFile(QString fileName);

public slots:

    //=== Menu Action Methods ===//

    /* Prompt the user to choose a GEDCOM file and open it */
    void openFileFromDialog();

    /* Save current GEDCOM file */
    void saveFile();

    /* Append Hanyu Pinyin data */
    void appendPinyin();

    /* Toggles display between the full set
     * of names, and the filtered set containg
     * only entries with incomplete pinyin.
     */
    void filterIncomplete(bool checked);

    /* Open a new tree view to view all families */
    void viewTree();

    /* Estimate missing dates in the family tree */
    void estimateDates();

    /* Switch between GUI translations */
    void switchLanguage(QAction * source);

    /* Launch browser to view the GedTools website */
    void launchWebsite();

    /* Launch browser to view the GedTools bug report page */
    void launchBugReport();

    /* Enable/Disable automatic update checks */
    void setAutoUpdate(bool enabled);

    /* Display "About" dialog */
    void displayAbout();

    //=== Other Actions ===//

    /* Called when the update check has been completed
     */
    void updateCheckFinished(const UpdateChecker * checker);

};

#endif
