
#include "GFamilyTreeModel.h"

//=== Constructor/Destructor ===//

/* Constructor
 * Builds a model for displaying the
 * entries contained in a GFamilyMap.
 */
GFamilyTreeModel::GFamilyTreeModel(QList<GFamilyTree *> & familyTrees) {
    // Create empty root node
    _root = new GFTNode();
    _root->childFams = new QList<GFTNode *>();
    // Add all family trees to the root node
    foreach (GFamilyTree * t, familyTrees) {
        _root->childFams->append(t->root());
    }
}

/* Destructor
 * Frees internal data structor for referencing
 * the entries contained in the GFamilyMap.
 */
GFamilyTreeModel::~GFamilyTreeModel() {
    delete _root;
}

//=== Accessors ===//

/* Returns the number fields per data entry in the model */
int GFamilyTreeModel::columnCount(const QModelIndex &parent) const {
    return COL_COUNT;
}

/* Returns the data at the specified index */
QVariant GFamilyTreeModel::data(const QModelIndex &index, int role) const {
    QVariant value;
    // Only using DisplayRole
    if (index.isValid() && role == Qt::DisplayRole) {
        GFTNode * n = static_cast<GFTNode *>(index.internalPointer());
        // Name
        if (index.column() == 0) {
            value = n->famName;
        }
        // Birth
        else if (index.column() == 1) {
            QString place = n->famHead->birthPlace();
            if (place.isNull()) place = "--";
            // Year
            QDate date = n->famHead->birthYear();
            QString year = date.isValid() ? date.toString("yyyy") : "--";
            // Value
            value = QString("%1 (%2)").arg(place).arg(year);
        }
        // Marriage
        else {
            // Not applicable if this is a single individual
            if (!n->thisFam) {
                value = tr("N/A");
            }
            // Write the place and year
            else {
                // Place
                QString place = n->thisFam->marriagePlace();
                if (place.isNull()) place = "--";
                // Year
                QDate date = n->thisFam->marriageYear();
                QString year = date.isValid() ? date.toString("yyyy") : "--";
                // Value
                value = QString("%1 (%2)").arg(place).arg(year);
            }
        }
    }
    return value;
}

/* Provides descriptions of the data rows/columns */
QVariant GFamilyTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    QVariant value;
    // Only horizontal DisplayRole headers
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
            case 0: // Name
                value = tr("Name");
                break;
            case 1: // Birth date
                value = tr("Birth");
                break;
            case 2: // Marriage date
                value = tr("Marriage");
        }
    }
    return value;
}

/* Calculate the index of a given entry */
QModelIndex GFamilyTreeModel::index(int row, int column, const QModelIndex &parent) const {
    QModelIndex index;
    if (hasIndex(row, column, parent)) { // Verify valid cooridinate
        // Get the parent and child GFTNodes
        GFTNode * parentNode;
        if (!parent.isValid()) { // Parent is root
            parentNode = _root;
        }
        else { // Parent is another node
            parentNode = nodeFromIndex(parent);
        }
        // Create the index
        index = createIndex(row, column, parentNode->childFams->at(row));
    }
    return index;
}

/* Find the parent of a given index */
QModelIndex GFamilyTreeModel::parent(const QModelIndex &index) const {
    QModelIndex pIndex;
    if (index.isValid()) { // _root has no parent, but still use QModelIndex()
        GFTNode * childNode = static_cast<GFTNode *>(index.internalPointer());
        GFTNode * parentNode = childNode->parentFam;
        // GFamilyTree root nodes have parentFam = 0, so if
        // parentNode doesn't exist then the parent is _root
        if (parentNode) {
            // Parent of family roots is null, so set it to _root if that's the case
            GFTNode * granparentNode = parentNode->parentFam ? parentNode->parentFam : _root;
            // Calculate the row number of the parent
            int row = granparentNode->childFams->indexOf(parentNode);
            // Only col=0 indecies have children
            pIndex = createIndex(row, 0, parentNode);
        }
    }
    return pIndex;
}

/* Returns the number of data entries in the model */
int GFamilyTreeModel::rowCount(const QModelIndex &parent) const {
    int count = 0;
    GFTNode * parentNode = nodeFromIndex(parent);
    // Tree leaves have no childFams list
    // because they have no children
    if (parentNode && parentNode->childFams) {
        count = parentNode->childFams->size();
    }
    return count;
}

//=== Private Helper Methods ===//

/* Find the corresponding GFTNode to an index in the tree */
GFTNode * GFamilyTreeModel::nodeFromIndex(const QModelIndex &index) const {
    if (index.isValid()) {
        return static_cast<GFTNode *>(index.internalPointer());
    }
    else {
        return _root;
    }
}
