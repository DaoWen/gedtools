
#include "GFamily.h"
#include "GIndiEntry.h"

//=== Constants ===//

// Todo: Move all of these into a separate header file
// (move the ones in GIndiEntry.cpp too)

// Level-0 family records have this data entry
const char DATA_FAM[] = "FAM";
// Level-1 parent and children attribute types
const char TYPE_HUSBAND[] = "HUSB";
const char TYPE_WIFE[] = "WIFE";
const char TYPE_CHILD[] = "CHIL";
const char TYPE_MARRIAGE[] = "MARR";
const char TYPE_DATE[] = "DATE";
const char TYPE_PLACE[] = "PLAC";

//=== Constructor/Destructor ===//

/* Constructor
 * Builds a family based on the data
 * parsed from a GEDCOM file, and links it
 * to the proper GNodes, starting with the GNode
 * pointing to this family's 0-level record.
 */
GFamily::GFamily(GNode * n) : _familyNode(0), _marriageDateNode(0), _marriagePlaceNode(0) {
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

/* Destructor */
GFamily::~GFamily() {}

//=== Accessors ===//

/* Get a copy of the ID string
 * value in the GNode data tree
 */
QString GFamily::id() const {
    // ID is stored in the type slot for some reason
    return _familyNode->type();
}

/* Get a copy of the ID string
 * value of the husband
 */
QString GFamily::husband() const {
    return _husbandID;
}

/* Get a copy of the ID string
 * value of the wife
 */
QString GFamily::wife() const {
    return _wifeID;
}

/* Get the list of ID strings
 * for all of the children
 */
const QStringList & GFamily::children() const {
    return _childrenIDs;
}

/* Date of the marriage */
QString GFamily::marriageDate() const {
    return _marriageDateNode ? _marriageDateNode->data() : QString();
}

/* Year of the marriage (useful in calculations) */
QDate GFamily::marriageYear() const {
    return _marriageYear;
}

/* Location of the marriage */
QString GFamily::marriagePlace() const {
    return _marriagePlaceNode ? _marriagePlaceNode->data() : QString();
}

/* If neither parent in this family is a child
 * in another family then this family is the root
 */
bool GFamily::isTreeRoot(GIndiMap & indiMap) const {
    GIndiMap::iterator i;
    return !(
      // Husband or Wife is the child of another family
      (!_husbandID.isNull() && // This family contains a husband
      ((i = indiMap.find(_husbandID)) != indiMap.end()) && // Husband is found in the map
      !i.value()->familyChild().isNull()) || // Husband is the child of another family
      (!_wifeID.isNull() && // This family contains a wife
      ((i = indiMap.find(_wifeID)) != indiMap.end()) && // Wife is found in the map
      !i.value()->familyChild().isNull()) // Wife is the child of another family
    );
}

/* If this family contains no children then
 * it must be a leaf in the family tree
 */
bool GFamily::isTreeLeaf(GIndiMap & indiMap) const {
    return _childrenIDs.size() == 0;
}

//=== Private Helper Methods ===//

/* Parses the family member
 * references for the parents and
 * children from this level of
 * the GNode tree
 */
void GFamily::parseMembers(GNode * n) {
    while (n) {
        if (n->type() == TYPE_HUSBAND) {
            _husbandID = n->data();
        }
        else if (n->type() == TYPE_WIFE) {
            _wifeID = n->data();
        }
        else if (n->type() == TYPE_CHILD) {
            // Children should all be clumped together,
            // so just loop through all of them here
            while (n && n->type() == TYPE_CHILD) {
                _childrenIDs.append(n->data());
                n = n->next();
            }
            // Children are the last item we need to
            // parse (they're specified as being after
            // the parents in the GEDCOM standard), so
            // break out of the loop now.
            break;
        }
        else if (n->type() == TYPE_MARRIAGE) {
            GNode * m = n->firstChild();
            while (m) {
                if (m->type() == TYPE_DATE) {
                    _marriageDateNode = m;
                    // Convert date string to an object for calculation purposes
                    _marriageYear = _marriageDateNode ?
                      QDate::fromString(_marriageDateNode->data().right(4),"yyyy") : QDate();
                }
                else if (m->type() == TYPE_PLACE) {
                    _marriagePlaceNode = m;
                }
                m = m->next();
            }
        }
        n = n->next();
    }
}
