
#include "GFamilyMap.h"

//=== Destructor ===//

/* Destructor
 */
GFamilyMap::~GFamilyMap() { }

//=== Mutators ===//

/* Inserts a family into the map
 * Pass in the pointer to the FAM node
 * Returns false if this family already exists
 */
bool GFamilyMap::insert(GNode * family) {
    // Use the family's ID as the key, which is stored
    // in the type of the node, and have it map to
    // the actual GFamily
    bool success;
    GFamily * entry;
    try { // GFamily might throw an exception
        entry = new GFamily(family);
        // Returns false if this key already exists
        success = QGFMap::insert(family->type(),entry).value();
    }
    catch (...) {
        // Return false if the entry creation failed
        success = false;
    }
    return success;
}
