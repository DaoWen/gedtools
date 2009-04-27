
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
    bool success;
    GIndiEntry * entry;
    try { // GIndiEntry might throw an exception
        entry = new GIndiEntry(person);
        // Returns false if this key already exists
        success = QGIMap::insert(person->type(),entry).value();
    }
    catch (...) {
        // Return false if the entry creation failed
        success = false;
    }
    return success;
}
