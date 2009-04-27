#ifndef V_GINDI_MAP_H
#define V_GINDI_MAP_H

#include <QMap>
#include <QString>
#include "GNode.h"
#include "GIndiEntry.h"

typedef QMap<QString,GIndiEntry*> QGIMap;

/* GIndiMap: Mapping of individual's xref_id onto that individual's GNode
 * Each individual in a GEDCOM file has a unique xref_id, similar to a pointer
 * or a label. This ID is used to link the individual to spouses and other
 * family members in the FAMILY construct. This data structure provides a fast
 * lookup for the individual based on their xref_id.
 */
class GIndiMap : public QGIMap {
public:

    //=== Destructor ===//

    /* Destructor
     */
    ~GIndiMap();

    //=== Mutators ===//

    /* Inserts a person into the map
     * Pass in the pointer to the INDI node
     * Returns false if this person already exists
     */
    bool insert(GNode * person);

};

#endif
