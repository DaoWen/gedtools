#ifndef V_GINDI_ENTRY_H
#define V_GINDI_ENTRY_H

#include "GNode.h"

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

private:

    //=== Private Data Members ===//

    // Links to GNode entries to corresponding data
    GNode * _indiNode, * _nameNode, * _romanNode, * _birthNode, * _deathNode;
    // Actual data for this individual
    QString _id, _name, _romanName, _birthDate, _deathDate;

    //=== Private Helper Methods ===//

    /* Parses the name data from the GNode tree */
    void parseNames(GNode * n);

    /* Parses the birth data from the GNode tree */
    void parseBirth(GNode * n);

    /* Parses the death data from the GNode tree */
    void parseDeath(GNode * n);

};

#endif
