
#include "GIndiMap.h"

//=== Destructor ===//

/* Destructor
 */
GIndiMap::~GIndiMap() { }

//=== Mutators ===//

/* Inserts a person into the map
 * Pass in the pointer to the INDI node
 * Returns false if this person already exists
 */
bool GIndiMap::insert(GNode * person) {
    // Use the INDI's ID as the key, which is stored
    // in the type of the node, and have it map to
    // the actual GIndiEntry
    // Returns false if this key already exists
    return QGMap::insert(person->type(),new GIndiEntry(person)).value();
}
