
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
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
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
    QVariant data;
    // Verify this data is to be displayed in the horizontal header
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case NAME_COL:
                data = tr("Name");
                break;
            case ROMAN_COL:
                data = tr("Romanized Name");
                break;
            case BIRTH_COL:
                data = tr("Birth Date");
                break;
        }
    }
    return data;
}

//=== Mutators ===//

/* Retrieve flags for a table cell
 * (selectable, editable, etc.)
 */
Qt::ItemFlags GIndiModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags flag = Qt::ItemIsEnabled; // Default return value
    if (index.isValid()) {
        // Only "Romanized Name" cell is editable
        if (index.column() == ROMAN_COL) {
            flag = QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
        }
        // All other columns are selectable
        else {
            flag = QAbstractItemModel::flags(index) | Qt::ItemIsSelectable;
        }
    }
    return flag;
}

bool GIndiModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    bool success = false; // Default return value
    // Only change data on editable cells with a valid index
    if (index.isValid() && role == Qt::EditRole) {
        // Update the data in the GIndiEntry
        _indiList->at(index.row())->setRomanizedName(value.toString());
        emit dataChanged(index, index);
        success = true;
    }
    return success;
 }


//=== Private Helper Methods ===//

/* Retrieve the data from the Individual
 * object corresponding to this column
 */
QVariant GIndiModel::getColData(int row, int col) const {
    QVariant data;
    // Todo: Change the cases to constants instead of literals
    switch (col) {
        case NAME_COL:
            data = _indiList->at(row)->name();
            break;
        case ROMAN_COL:
            data = _indiList->at(row)->romanizedName();
            break;
        case BIRTH_COL:
            data = _indiList->at(row)->birthDate();
            break;
    }
    return data;
}
