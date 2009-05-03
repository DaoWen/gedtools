
#include "TreeWindow.h"

TreeWindow::TreeWindow(GFamilyTreeModel * treeModel, QWidget * parent)
 : QDialog(parent), _treeModel(treeModel) {
    _mainLayout = new QVBoxLayout();
    // Tree
    _treeView = new QTreeView();
    _treeView->setModel(treeModel);
    _mainLayout->addWidget(_treeView);
    // Buttons
    _buttonsLayout = new QHBoxLayout();
    // Expand
    _expandButton = new QPushButton(tr("Expand All"));
    QObject::connect(_expandButton, SIGNAL(clicked()), this, SLOT(expandTree()));
    _buttonsLayout->addWidget(_expandButton);
    // Collapse
    _collapseButton = new QPushButton(tr("Collapse All"));
    QObject::connect(_collapseButton, SIGNAL(clicked()), this, SLOT(collapseTree()));
    _buttonsLayout->addWidget(_collapseButton);
    // Main
    _mainLayout->addLayout(_buttonsLayout);
    setLayout(_mainLayout);
    setWindowTitle(tr("Family Tree"));
}

TreeWindow::~TreeWindow() {
    delete _treeView;
    delete _expandButton;
    delete _collapseButton;
    delete _buttonsLayout;
    delete _mainLayout;
}

void TreeWindow::expandTree() {
    _treeView->expandAll();
}

void TreeWindow::collapseTree() {
    _treeView->collapseAll();
}
