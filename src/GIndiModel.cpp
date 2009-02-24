
#include "GIndiModel.h"

//=== Constructor/Destructor ===//

/* Constructor
 * Builds a model for displaying the
 * entries contained in a GIndiMap.
 */
GIndiModel::GIndiModel(GIndiMap & indiMap) {
    _indiList = new QList<GIndiEntry*>;
    GIndiMap::iterator i = indiMap.begin();
    GIndiMap::iterator end = indiMap.end();
    for (; i!=end; ++i) {
        _indiList->append(i.value());
    }
}

/* Destructor
 * Frees internal data structor for referencing
 * the entries contained in the GIndiMap.
 */
GIndiModel::~GIndiModel() {
    delete _indiList;
}

//=== Accessors ===//

/* Returns the number of data entries in the model */
int GIndiModel::rowCount(const QModelIndex &parent) const {
    return _indiList->size();
}

/* Returns the number fields per data entry in the model */
int GIndiModel::columnCount(const QModelIndex &parent) const {
    return COL_COUNT;
}

/* Returns the data at the specified index */
QVariant GIndiModel::data(const QModelIndex &index, int role) const {
    QVariant data = QVariant();
    // Return a "null" value if this isn't a valid index
    if (index.isValid()) {
        // Return a "null" value if this isn't being displayed as text
        if (role == Qt::DisplayRole) {
            // Return a "null" value if this entry is outside the bounds
            // of our list of individuals (this GIndiEntry doesn't exist)
            if (index.row() < _indiList->size()) {
                // Otherwise return the data
                data = getColData(index.row(), index.column());
            }
        }
    }
    return data;
}

/* Provides descriptions of the data rows/columns */
QVariant GIndiModel::headerData(int section, Qt::Orientation orientation, int role) const {
    // Todo
    return QVariant();
}

//=== Private Helper Methods ===//

/* Retrieve the data from the Individual
 * object corresponding to this column
 */
QVariant GIndiModel::getColData(int row, int col) const {
    QVariant data;
    // Todo: Change the cases to constants instead of literals
    switch (col) {
        case 0:
            data = _indiList->at(row)->name();
            break;
        case 1:
            data = _indiList->at(row)->romanizedName();
            break;
        case 2:
            data = _indiList->at(row)->birthDate();
            break;
        default:
            data = QVariant();
    }
    return data;
}
