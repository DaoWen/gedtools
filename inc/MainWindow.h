#ifndef V_MAINWINDOW_H
#define V_MAINWINDOW_H

#include <QtGui>
#include <QtCore>
#include "GFile.h"
#include "GIndiModel.h"

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
     */
    ~MainWindow();

private:

    //=== Private Data Members ===//

    GFile * gedFile;

    QMenu * fileMenu;

    QAction * openAct;

    QAction * exitAct;

    QTableView * tableView;

    QAbstractItemModel * indiModel;

    //=== Private Helper Methods ===//

    void createMenus();

private slots:

    //=== Menu Action Methods ===//

    void openFile();

};

#endif
