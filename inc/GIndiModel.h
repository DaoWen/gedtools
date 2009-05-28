#ifndef V_GINDIMODEL_H
#define V_GINDIMODEL_H

#include <QList>
#include <QAbstractTableModel>
#include "GIndiMap.h"

/* GIndiModel: Model of individuals' data for display in a table
 * This is an interface for the QTableView to access all of the
 * data on the individuals in the GEDCOM file for display to the
 * user in a table format.
 */
class GIndiModel : public QAbstractTableModel {

Q_OBJECT // Qt Library Macro

public:

    //=== Constants ===//

    // Column Indecies
    static const int NAME_COL = 0;
    static const int ROMAN_COL = 1;
    static const int BIRTH_COL = 2;
    static const int DEATH_COL = 3;

    //=== Constructor/Destructor ===//

    /* Constructor
     * Builds a model for displaying the
     * entries contained in a GIndiMap.
     */
    GIndiModel(GIndiMap & indiMap);

    /* Destructor
     * Frees internal data structor for referencing
     * the entries contained in the GIndiMap.
     */
    ~GIndiModel();

    //=== Accessors ===//

    /* Returns the number fields per data entry in the model */
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    /* Returns the data at the specified index */
    QVariant data(const QModelIndex &index, int role) const;

    /* Retrieve flags for a table cell
     * (selectable, editable, etc.)
     */
    Qt::ItemFlags flags(const QModelIndex &index) const;

    /* Provides descriptions of the data rows/columns */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    /* Returns the number of data entries in the model */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    //=== Mutators ===//

    /* Provides an interface to update the internal data behind the model */
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    /* Sort column data */
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

    /* Notify all views that internal data has been changed */
    void resetViews();

private:

    //=== Private Data Members ===//

    // Number of columns in the table (used for the columnCount() method)
    static const int COL_COUNT = 4;

    // Map of IDs to Individuals
    QList<GIndiEntry*> * _indiList;

    //=== Private Helper Methods ===//

    /* Retrieve the data from the Individual
     * object corresponding to this column
     */
    static QString getColData(const GIndiEntry * indi, int col);

    //=== ColumnComparer Class ===//

    /* ColumnComparer: Comparison operator for comparing
     * columns in the _indiList and used by sort()
     */
    class ColumnComparer {
        public:
            /* Constructor */
            ColumnComparer(int column, bool ascending);
            /* Comparision Operation */
            bool operator()(GIndiEntry * a, GIndiEntry * b);
        private:
            int _col; // Sort target column
            bool _asc; // Sort direction
    };

};

#endif
