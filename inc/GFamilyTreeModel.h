#ifndef V_GFAMILY_TREE_MODEL_H
#define V_GFAMILY_TREE_MODEL_H

#include <QAbstractItemModel>
#include "GFamilyTree.h"

/* GFamilyTreeModel: Model linking families into a family tree
 * This is an interface for the QTreeView to access all of the
 * data on the families in the GEDCOM file for display to the
 * user in a tree format.
 */
class GFamilyTreeModel : public QAbstractItemModel {

Q_OBJECT // Qt Library Macro

public:

    //=== Constructor/Destructor ===//

    /* Constructor
     * Builds a model for displaying the
     * entries contained in a GFamilyMap.
     */
    GFamilyTreeModel(QList<GFamilyTree *> & familyTrees);

    /* Destructor
     * Frees internal data structor for referencing
     * the entries contained in the GFamilyMap.
     */
    ~GFamilyTreeModel();

    //=== Accessors ===//

    /* Returns the number fields per data entry in the model */
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    /* Returns the data at the specified index */
    QVariant data(const QModelIndex &index, int role) const;

    /* Provides descriptions of the data rows/columns */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    /* Calculate the index of a given entry */
    QModelIndex index(int row, int column, const QModelIndex &parent) const;

    /* Find the parent of a given index */
    QModelIndex GFamilyTreeModel::parent(const QModelIndex &index) const;

    /* Returns the number of data entries in the model */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

private:

    //=== Private Data Members ===//

    // Number of columns in the model (used for the columnCount() method)
    static const int COL_COUNT = 3;

    // Special node
    GFTNode * _root;

    //=== Private Helper Methods ===//

    /* Find the corresponding GFTNode to an index in the tree */
    GFTNode * nodeFromIndex(const QModelIndex &index) const;

};

#endif
