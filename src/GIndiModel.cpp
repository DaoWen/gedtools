
#include <climits>
#include "GIndiModel.h"

//=== Constants ===//

static const char EMPTY_VALUE[] = "--";

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
int GIndiModel::columnCount(const QModelIndex &/*parent*/) const {
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
            case BIRTH_DATE_COL:
                data = tr("Birth Date");
                break;
            case BIRTH_PLACE_COL:
                data = tr("Birth Place");
                break;
            case DEATH_DATE_COL:
                data = tr("Death Date");
                break;
            case DEATH_PLACE_COL:
                data = tr("Death Place");
                break;
            case ID_COL:
                data = tr("Record #");
        }
    }
    return data;
}

/* Returns the number of data entries in the model */
int GIndiModel::rowCount(const QModelIndex &/*parent*/) const {
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
        invalidateViews();
        switch (column) {
            case ID_COL: // These columns are sorted as integers
            case BIRTH_DATE_COL:
            case DEATH_DATE_COL:
            {
                IntColumnComparer cmp(column, order == Qt::AscendingOrder);
                qSort(_indiList->begin(), _indiList->end(), cmp);
            }
                break;
            default: // Sort all other valid columns as strings
            {
                StrColumnComparer cmp(column, order == Qt::AscendingOrder);
                qSort(_indiList->begin(), _indiList->end(), cmp);
            }
        }
        revalidateViews();
    }
}

/* Notify all views that internal data is changing */
void GIndiModel::invalidateViews() {
    beginResetModel();
}

/* Notify all views that internal data has been changed */
void GIndiModel::revalidateViews() {
    endResetModel();
}

//=== Private Helper Methods ===//

/* Retrieve the data from the Individual
 * object corresponding to this column
 */
QString GIndiModel::getColData(const GIndiEntry * indi, int col) {
    QString data;
    QDate year;
    switch (col) {
        case NAME_COL:
            data = indi->name();
            break;
        case ROMAN_COL:
            data = indi->romanizedName();
            break;
        case BIRTH_DATE_COL:
            year = indi->birthYear();
            data = year.isNull() ? EMPTY_VALUE : year.toString("yyyy");
            break;
        case BIRTH_PLACE_COL:
            data = indi->birthPlace();
            if (data.isNull()) data = EMPTY_VALUE;
            break;
        case DEATH_DATE_COL: // Write "Death Place (Death Year)"
            year = indi->deathYear();
            data = indi->deathPlace();
            if (data.isNull()) data = EMPTY_VALUE;
            // Write the date if there is one
            if (year.isValid()) {
                data = year.toString("yyyy");
            }
            // Write "Deceased" if marked as such
            else if (indi->deceased()) {
                data = tr("Deceased");
            }
            // Otherwise...
            else {
                data = EMPTY_VALUE;
            }
            break;
        case DEATH_PLACE_COL:
            data = indi->deathPlace();
            if (data.isNull()) data = EMPTY_VALUE;
            break;
        case ID_COL:
            data = indi->id();
            // Remove "@I @"
            data = data.remove(data.length()-1, 1).remove(0, 2);
            break;
    }
    return data;
}

//=== ColumnComparer Classes ===//

/* Int Constructor */
GIndiModel::IntColumnComparer::IntColumnComparer(int column, bool ascending)
 : _col(column), _asc(ascending) {}

/* Int Comparision Operation */
bool GIndiModel::IntColumnComparer::operator()(GIndiEntry * a, GIndiEntry * b) {
    bool okA, okB;
    QString dataStrA = getColData(a, _col);
    QString dataStrB = getColData(b, _col);
    int dataA = dataStrA.toInt(&okA);
    int dataB = dataStrB.toInt(&okB);
    // Not all date values will parse as integers
    // Empty dates "--" should be put at the bottom, not at 0
    // (otherwise all empty dates will appear beteen BC and AD)
    // And we set non-empty strings to the top, otherwise
    // "Deceased" and "--" won't be separated
    if (!okA) dataA = dataStrA == EMPTY_VALUE ? INT_MIN : INT_MAX;
    if (!okB) dataB = dataStrB == EMPTY_VALUE ? INT_MIN : INT_MAX;
    return _asc ? dataA < dataB : dataA > dataB;
}

/* Str Constructor */
GIndiModel::StrColumnComparer::StrColumnComparer(int column, bool ascending)
 : _col(column), _asc(ascending) {}

/* Str Comparision Operation */
bool GIndiModel::StrColumnComparer::operator()(GIndiEntry * a, GIndiEntry * b) {
    QString dataA = getColData(a, _col);
    QString dataB = getColData(b, _col);
    return _asc ? dataA < dataB : dataA > dataB;
}
