
#include <climits>
#include "TreeWindow.h"

TreeWindow::TreeWindow(GFamilyTreeModel * treeModel, GFile * gedFile, QWidget * parent)
 : QDialog(parent, Qt::WindowMinMaxButtonsHint), _treeModel(treeModel), _gedFile(gedFile) {
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
    // Search
    _searchButton = new QPushButton(tr("Find Individual"));
    QObject::connect(_searchButton, SIGNAL(clicked()), this, SLOT(searchTree()));
    _buttonsLayout->addWidget(_searchButton);
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

void TreeWindow::searchTree() {
    bool okPressed;
    int searchId = QInputDialog::getInt(this, tr("Search for an individual"),
      tr("Enter the record number of the\nindividual whom you wish to locate in the tree:"
      "\n(Note: individuals may be found with their spouses)"), 0, 0, INT_MAX, 1, &okPressed
    );
    // Continue only if the user pressed OK
    if (okPressed) {
        // Search from the root's ID column
        QModelIndex start = _treeModel->index(0, GFamilyTreeModel::ID_COL);
        QModelIndexList matches = _treeModel->match(start, Qt::DisplayRole,
          QString::number(searchId), 1, Qt::MatchExactly | Qt::MatchRecursive
        );
        // If no match is found, try searching for the spouse instead
        if (matches.size() == 0) {
            QString spouseId = findSpouseFromId(QString("@I%1@").arg(searchId));
            // No spouse found in indiMap
            if (spouseId.isEmpty()) {
                QMessageBox::information(this, tr("Individual not found"),
                  tr("No individuals were found with record #%1").arg(searchId)
                );
            }
            // Spouse not found in family tree
            else if ((matches = _treeModel->match(start, Qt::DisplayRole,spouseId, 1,
              Qt::MatchExactly | Qt::MatchRecursive)).size() == 0) {
                QMessageBox::information(this, tr("Individual not found"),
                  tr("No individuals were found in the tree with record #%1").arg(searchId)
                );
            }
            else { // Found spouse
                _treeView->setCurrentIndex(matches.first());
            }
        }
        else { // Found individual
            _treeView->setCurrentIndex(matches.first());
        }
    }
}

QString TreeWindow::findSpouseFromId(const QString searchId) {
    QString spouseId;
    GIndiMap & indiMap = _gedFile->indiMap();
    // Find the individual
    GIndiMap::ConstIterator i = indiMap.find(searchId);
    if (i != indiMap.end()) {
        // Find the family in which individual is a parent
        QString famId = i.value()->familyParent();
        GFamilyMap & famMap = _gedFile->familyMap();
        GFamilyMap::ConstIterator f = famMap.find(famId);
        if (f != famMap.end()) {
            const GFamily * fam = f.value();
            // Find individual's spouse
            spouseId = fam->husband() == searchId ? fam->wife() : fam->husband();
            // Remove "@I @"
            spouseId = spouseId.remove(spouseId.length()-1, 1).remove(0, 2);
        }
    }
    return spouseId;
}
