#ifndef V_TREE_WINDOW_H
#define V_TREE_WINDOW_H

#include <QtGui>
#include <QTreeView>
#include "GFamilyTreeModel.h"

class TreeWindow : public QDialog {

Q_OBJECT // Qt Library Macro

public:
    TreeWindow(GFamilyTreeModel * treeModel, QWidget * parent);
    ~TreeWindow();

private:
    GFamilyTreeModel * _treeModel;
    QBoxLayout * _mainLayout, * _buttonsLayout;
    QTreeView * _treeView;
    QPushButton * _expandButton;
    QPushButton *  _collapseButton;

public slots:
    void expandTree();
    void collapseTree();

};

#endif
