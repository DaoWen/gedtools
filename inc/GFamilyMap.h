#ifndef V_GFAMILY_MAP_H
#define V_GFAMILY_MAP_H

#include <QMap>
#include <QString>
#include "GNode.h"
#include "GFamily.h"

typedef QMap<QString,GFamily*> QGFMap;

/* GFamilyMap: Mapping of families' xref_id onto that individual's GNode
 * Each family in a GEDCOM file has a unique xref_id, similar to a pointer
 * or a label. This ID is used to link the family to its individual members
 * family members in the FAM construct. This data structure provides a fast
 * lookup for the individual based on their xref_id.
 */
class GFamilyMap : public QGFMap {
public:

    //=== Destructor ===//

    /* Destructor
     */
    ~GFamilyMap();

    //=== Mutators ===//

    /* Inserts a family into the map
     * Pass in the pointer to the FAM node
     * Returns false if this person already exists
     */
    bool insert(GNode * family);

};

#endif
