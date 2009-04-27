#ifndef V_GINDI_ENTRY_H
#define V_GINDI_ENTRY_H

#include "GNode.h"
#include <QDate>

/* GIndiEntry: Representation of an individual in a GEDCOM file
 * This class contains all relevant data found in a GEDCOM entry
 * for an individual (INDI). Not only does it hold local copies
 * of the relevant data strings (for convenience when editing),
 * but it also holds references to the GNodes containing the
 * relevant data for alteration of the actual data file.
 */
class GIndiEntry {
public:

    //=== Constructors ===//

    /* Constructor
     * Builds an Individual based on the data
     * parsed from a GEDCOM file, and links it
     * to the proper GNodes, starting with the GNode
     * pointing to this individual's 0-level record.
     */
    GIndiEntry(GNode * n);

    //=== Accessors ===//

    /* Get a copy of the ID string
     * value in the GNode data tree
     */
    QString id() const;

    /* Get a copy of the name string
     * value in the GNode data tree
     */
    QString name() const;

    /* Get a copy of the romanized name
     * string value in the GNode data tree
     */
    QString romanizedName() const;

    /* Get a copy of the birth date string
     * value in the GNode data tree
     */
    QString birthDate() const;

    /* Get a copy of the death date string
     * value in the GNode data tree
     */
    QString deathDate() const;

    //=== Mutators ===//

    /* Set a new value for the romanized name
     * string value in the GNode data tree, and
     * create a node in the tree if needed.
     */
    void setRomanizedName(const QString & romanName);

private:

    //=== Private Data Members ===//

    // Links GNode entries to corresponding data
    GNode * _indiNode, * _nameNode, * _romanNode, * _birthNode, * _deathNode;

    // Date objects for performing missing date estimates
    QDate _birthYear, _deathYear;

    //=== Private Helper Methods ===//

    /* Parses the name data from the GNode tree */
    void parseNames(GNode * n);

    /* Parses the birth data from the GNode tree */
    void parseBirth(GNode * n);

    /* Parses the death data from the GNode tree */
    void parseDeath(GNode * n);

};

#endif
