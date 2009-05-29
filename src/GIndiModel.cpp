
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
                data = getColData(_indiList->at(index.row()), index.column());
            }
        }
    }
    return data;
}

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
                data = tr("Birth");
                break;
            case DEATH_COL:
                data = tr("Death");
                break;
        }
    }
    return data;
}

/* Returns the number of data entries in the model */
int GIndiModel::rowCount(const QModelIndex &parent) const {
    return _indiList->size();
}

//=== Mutators ===//

/* Provides an interface to update the internal data behind the model */
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

/* Sort column data */
void GIndiModel::sort(int column, Qt::SortOrder order) {
    // Only sort valid columns
    if (column >= 0 && column < COL_COUNT) {
        ColumnComparer cmp(column, order == Qt::AscendingOrder);
        qSort(_indiList->begin(), _indiList->end(), cmp);
        reset(); // Notify views that data has changed
    }
}

/* Notify all views that internal data has been changed */
void GIndiModel::resetViews() {
    reset();
}

//=== Private Helper Methods ===//

/* Retrieve the data from the Individual
 * object corresponding to this column
 */
QString GIndiModel::getColData(const GIndiEntry * indi, int col) {
    QString data;
    QDate year;
    // Todo: Change the cases to constants instead of literals
    switch (col) {
        case NAME_COL:
            data = indi->name();
            break;
        case ROMAN_COL:
            data = indi->romanizedName();
            break;
        case BIRTH_COL: // Write "Birth Place (Birth Year)"
            year = indi->birthYear();
            data = indi->birthPlace();
            if (data.isNull()) data = "--";
            data.append(year.isNull() ? " (--)" : year.toString(" (yyyy)"));
            break;
        case DEATH_COL: // Write "Death Place (Death Year)"
            year = indi->deathYear();
            data = indi->deathPlace();
            if (data.isNull()) data = "--";
            // Write the date if there is one
            if (year.isValid()) {
                data.append(year.toString(" (yyyy)"));
            }
            // Write "Deceased" if marked as such
            else if (indi->deceased()) {
                data.append(" (").append(tr("Deceased")).append(")");
            }
            // Otherwise...
            else {
                data.append(" (--)");
            }
            break;
    }
    return data;
}

//=== ColumnComparer Class ===//

/* Constructor */
GIndiModel::ColumnComparer::ColumnComparer(int column, bool ascending)
 : _col(column), _asc(ascending) {}

/* Comparision Operation */
bool GIndiModel::ColumnComparer::operator()(GIndiEntry * a, GIndiEntry * b) {
    QString dataA = getColData(a, _col);
    QString dataB = getColData(b, _col);
    return _asc ? dataA < dataB : dataA > dataB;
}
