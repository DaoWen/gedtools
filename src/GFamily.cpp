
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
const char ENTRY_MARRIAGE[] = "1 MARR";
const char TYPE_DATE[] = "DATE";
const char ENTRY_DATE[] = "2 DATE";
const char TYPE_PLACE[] = "PLAC";
const char ENTRY_PLACE[] = "2 PLAC";

//=== Constructor/Destructor ===//

/* Constructor
 * Builds a family based on the data
 * parsed from a GEDCOM file, and links it
 * to the proper GNodes, starting with the GNode
 * pointing to this family's 0-level record.
 */
GFamily::GFamily(GNode * n) : _familyNode(0), _husbandNode(0), _wifeNode(0),
  _marriageNode(0), _marriageDateNode(0), _marriagePlaceNode(0) {
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
    return _husbandNode ? _husbandNode->data() : QString();
}

/* Get a copy of the ID string
 * value of the wife
 */
QString GFamily::wife() const {
    return _wifeNode ? _wifeNode->data() : QString();
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
 * in another family then this family is the root,
 * and if this family will not show up in the
 * direct line of the tree because it's not the
 * first marriage then it is a tree root
 */
bool GFamily::isTreeRoot(GIndiMap & indiMap) const {
    GIndiMap::iterator iHusband, iWife;
    // Assume husband/wife is not the child of another family
    bool husbandRoot = true, wifeRoot = true;
    // Assume this is the husband/wife's first marriage
    bool husbandRemarried = false, wifeRemarried = false;
    // Husband
    if (_husbandNode) {
        iHusband = indiMap.find(husband());
        // Verify that this person exists
        if (iHusband == indiMap.end()) {
            throw (QString("Husband not found: ").append(husband()));
        }
        // Husband is not the child of another family
        husbandRoot = iHusband.value()->familyChild().isNull();
        // This is not his first marriage
        husbandRemarried = id() != iHusband.value()->familyParent();
    }
    // Wife
    if (_wifeNode) {
        iWife = indiMap.find(wife());
        // Verify that this person exists
        if (iWife == indiMap.end()) {
            throw (QString("Wife not found: ").append(wife()));
        }
        // Wife is the child of another family
        wifeRoot = iWife.value()->familyChild().isNull();
        // This is not her first marriage
        wifeRemarried = id() != iWife.value()->familyParent();
    }
    // Both roots, or one remarried and the other a root
    return (husbandRoot && wifeRoot) || (husbandRemarried && wifeRoot) || (wifeRemarried && husbandRoot);
}

/* If this family contains no children then
 * it must be a leaf in the family tree
 */
bool GFamily::isTreeLeaf(GIndiMap & indiMap) const {
    return _childrenIDs.size() == 0;
}

//=== Mutators ===//

/* Sets a couple's estimated marriage
 * year and updates the MARR node value
 */
void GFamily::setMarriageYear(const QDate & year, const QString & place) {
    if (!year.isValid()) {
        throw QString("Attempted to set an invalid marriage date.");
    }
    // Update the internal date
    _marriageYear = year;
    // Append MARR node if null
    if (!_marriageNode) appendMarriageNode();
    // Create the DATE node if needed
    if (!_marriageDateNode) {
        _marriageDateNode = new GNode(ENTRY_DATE);
    }
    // Build the date entry string
    QString dateString("EST ");
    dateString.append(year.toString("yyyy"));
    // Update nodes
    _marriageDateNode->setData(dateString);
    _marriageNode->setFirstChild(_marriageDateNode);
    // Append the marriage place if specified
    if (!place.isNull()) {
        // Create the place node if needed
        if (!_marriagePlaceNode) {
            _marriagePlaceNode = new GNode(ENTRY_PLACE);
        }
        if (_marriagePlaceNode->data().isEmpty()) {
            _marriagePlaceNode->setData(place);
        }
    }
    _marriageDateNode->setNext(_marriagePlaceNode);
}


//=== Private Helper Methods ===//

/* Appends an empty MARR node just after
 * the last HUSB, WIFE, or CHIL node
 */
void GFamily::appendMarriageNode() {
    GNode * m, * n = _familyNode->firstChild();
    QString nextType;
    // The MARR node should be inserted after the last
    // HUSB, WIFE, or CHIL node, so find the last one
    do {
        m = n; // Next node
        n = n->next(); // Next-next node
        nextType = n ? n->type() : QString();
    } while (!nextType.isNull() && (nextType == TYPE_CHILD || nextType == TYPE_HUSBAND || nextType == TYPE_WIFE));
    // Append MARR node here
    _marriageNode = m->insertNext(new GNode(ENTRY_MARRIAGE));
}

/* Parses the family member
 * references for the parents and
 * children from this level of
 * the GNode tree
 */
void GFamily::parseMembers(GNode * n) {
    while (n) {
        if (n->type() == TYPE_HUSBAND) {
            _husbandNode = n;
        }
        else if (n->type() == TYPE_WIFE) {
            _wifeNode = n;
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
            _marriageNode = n;
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
