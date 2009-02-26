#ifndef V_MAINMENUBAR_H
#define V_MAINMENUBAR_H

#include <QtGui>
#include "MainWindow.h"

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

private:

    //=== Private Data Members ===//

    QMenu * _fileMenu;

    QAction * _openAct;

    QAction * _saveAct;

    QAction * _exitAct;

};

#endif
