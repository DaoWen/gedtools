#ifndef V_GFAMILY_H
#define V_GFAMILY_H

#include <QList>
#include "GNode.h"

/* GFamily: Representation of a family in a GEDCOM file
 * This class contains all relevant data found in a GEDCOM entry
 * for a family (FAM). Not only does it hold local copies of
 * the relevant data strings (for convenience when editing),
 * but it also holds references to the GNodes containing the
 * relevant data for alteration of the actual data file.
 */
class GFamily {
public:

    //=== Constructors ===//

    /* Constructor
     * Builds a family based on the data
     * parsed from a GEDCOM file, and links it
     * to the proper GNodes, starting with the GNode
     * pointing to this family's 0-level record.
     */
    GFamily(GNode * n);

    //=== Accessors ===//

    /* Get a copy of the ID string
     * value in the GNode data tree
     */
    QString id() const;

private:

    //=== Private Data Members ===//

    // Links GNode entries to corresponding data
    GNode * _familyNode;

    // Parents' xref_id data
    const QString * _husbandID, _wifeID;

    // Children's xref_id data
    QList<const QString*> _childrenIDs;

    //=== Private Helper Methods ===//

    /* Parses the family member
     * references for the parents and
     * children from the GNode tree
     */
    void parseMembers(GNode * n);

};

#endif
