#ifndef V_MAINMENUBAR_H
#define V_MAINMENUBAR_H

#include <QtGui>

// Forward declaration to avoid circular definition
class MainWindow;

/* MainMenuBar: Primary Menu Bar for the GedTools GUI
 */
class MainMenuBar : public QMenuBar {

Q_OBJECT // Qt Library Macro

public:

    //=== Constructor/Destructor ===//

    /* Constructor
     * Builds the menus for the
     * main window of the GUI
     */
    MainMenuBar(MainWindow * mainWin);

    /* Destructor
     */
    ~MainMenuBar();

    /* Enables certain menu items that are only useful
     * after a GEDCOM file has been opened for editing
     */
    void enableLockedItems();

    /* Used to clear the checkmark on the
     * "Display Missing Pinyin" item
     */
    void clearFilterToggle();

    /* Should adoptive relations being used? */
    bool usingAdoptedRelations();

    /* Should add DECEASED annotation for people over 110 years old? */
    bool usingDeceasedOver110();

private:

    //=== Private Data Members ===//

    // File Menu
    QMenu * _fileMenu;
    QAction * _openAct;
    QAction * _saveAct;
    QAction * _exitAct;

    // Pinyin Menu
    QMenu * _pinyinMenu;
    QAction * _appendPinyinAct;
    QAction * _filterIncompleteAct;

    // Family Tree Menu
    QMenu * _toolsMenu;
    QAction * _viewTreeAct;
    QAction * _useAdoptionsToggleAct;
    QAction * _useDeceasedOver110ToggleAct;
    QAction * _estimateDatesAct;

    // Language Menu
    QMenu * _languageMenu;
    QActionGroup * _languageActionGroup;

    // Help Menu
    QMenu * _helpMenu;
    QAction * _websiteAct;
    QAction * _bugReportAct;
    QAction * _enableUpdateAct;
    QAction * _aboutAct;

};

#endif
