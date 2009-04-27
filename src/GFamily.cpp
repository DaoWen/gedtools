
#include "GFamily.h"

//=== Constants ===//

// Level-0 family records have this data entry
const char DATA_FAM[] = "FAM";

//=== Constructors ===//

/* Constructor
 * Builds a family based on the data
 * parsed from a GEDCOM file, and links it
 * to the proper GNodes, starting with the GNode
 * pointing to this family's 0-level record.
 */
GFamily::GFamily(GNode * n) : _familyNode(0), _husbandID(0), _wifeID(0) {
    // Validate that this is a family record
    if (!n) {
        throw QString("Null Pointer to Family");
    }
    else if (n->level() != 0 || n->data() != DATA_FAM) {
        throw QString("Bad Family");
    }
    // Save this family's data
    _familyNode = n; // Save a link to this family's GNode
    n = n->firstChild();
    // Get family member data
    parseMembers(n);
}

//=== Accessors ===//

/* Get a copy of the ID string
 * value in the GNode data tree
 */
QString GFamily::id() const {
    // ID is stored in the type slot for some reason
    return _familyNode->type();
}

//=== Private Helper Methods ===//

/* Parses the family member
 * references for the parents and
 * children from the GNode tree
 */
void GFamily::parseMembers(GNode * n) {

}
