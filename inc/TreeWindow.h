#ifndef V_TREE_WINDOW_H
#define V_TREE_WINDOW_H

#include <QtGui>
#include <QTreeView>
#include "GFile.h"
#include "GFamilyTreeModel.h"

class TreeWindow : public QDialog {

Q_OBJECT // Qt Library Macro

public:

    TreeWindow(GFamilyTreeModel * treeModel, GFile * gedFile, QWidget * parent);
    ~TreeWindow();

private:

    GFamilyTreeModel * _treeModel;
    GFile * _gedFile;
    QBoxLayout * _mainLayout, * _buttonsLayout;
    QTreeView * _treeView;
    QPushButton * _expandButton;
    QPushButton *  _collapseButton;
    QPushButton *  _searchButton;

    QString findSpouseFromId(const QString searchId, int * famCount);

public slots:

    void expandTree();
    void collapseTree();
    void searchTree();

};

#endif
