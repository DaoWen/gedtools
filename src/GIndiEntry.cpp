
#include "GIndiEntry.h"

//=== Constants ===//

// Level-0 individual records have this data entry
const char DATA_INDI[] = "INDI";
// Level-1 name attributes have this type
const char TYPE_NAME[] = "NAME";
// Level-2 romanized name attributes have this type
const char TYPE_ROMANIZED_NAME[] = "ROMN";
// Level-1 sex attributes have this type
const char TYPE_SEX[] = "SEX";
// Level-1 birth attributes have this type
const char TYPE_BIRTH[] = "BIRT";
// Level-1 death attributes have this type
const char TYPE_DEATH[] = "DEAT";
// Level-2 date attributes have this type
const char TYPE_DATE[] = "DATE";
// Level-1 family (spouse) attributes have this type
const char TYPE_FAMS[] = "FAMS";
// Level-1 family (child) attributes have this type
const char TYPE_FAMC[] = "FAMC";

//=== Static Constants ===//

// Male and Female values corresponding to sex
const char * GIndiEntry::MALE = "M";
const char * GIndiEntry::FEMALE = "F";

//=== Constructors ===//

/* Constructor
 * Builds an Individual based on the data
 * parsed from a GEDCOM file, and links it
 * to the proper GNodes, starting with the GNode
 * pointing to this individual's 0-level record.
 */
GIndiEntry::GIndiEntry(GNode * n)
 : _indiNode(0), _nameNode(0), _romanNode(0), _birthNode(0), _deathNode(0), _famsNode(0), _famcNode(0) {
    // Validate that this is an individual record
    if (!n) {
        throw QString("Null Pointer to Individual");
    }
    else if (n->level() != 0 || n->data() != DATA_INDI) {
        throw QString("Bad Individual");
    }
    // Save this individual's data
    _indiNode = n; // Save a link to this individual's GNode
    // Find important data in this individual's tree
    parseIndiData(n->firstChild());
    // Individuals must have a name
    if (!_nameNode) {
        throw QString("Bad Individual: No Name");
    }
}

//=== Accessors ===//

/* Get a copy of the ID string
 * value in the GNode data tree
 */
QString GIndiEntry::id() const {
    // ID is stored in the type slot for some reason
    return _indiNode->type();
}

/* Get a copy of the name string
 * value in the GNode data tree
 */
QString GIndiEntry::name() const {
    return _nameNode->data();
}

/* Get a copy of the romanized name
 * string value in the GNode data tree
 */
QString GIndiEntry::romanizedName() const {
    // This node might not exist...
    return _romanNode ? _romanNode->data() : QString();
}

/* Get a copy of the sex string
 * value in the GNode data tree
 */
QString GIndiEntry::sex() const {
    return _sexNode->data();
}

/* Get a copy of the birth date string
 * value in the GNode data tree
 */
QString GIndiEntry::birthDate() const {
    // This node might not exist...
    return _birthNode ? _birthNode->data() : QString();
}

/* Get a copy of the death date string
 * value in the GNode data tree
 */
QString GIndiEntry::deathDate() const {
    // This node might not exist...
    return _deathNode ? _deathNode->data() : QString();
}

/* Get a copy of the family (child) ID
 *  string value in the GNode data tree
 */
QString GIndiEntry::familyChild() const {
    return _famcNode ? _famcNode->data() : QString();
}

/* Get a copy of the family (parent) ID
 *  string value in the GNode data tree
 */
QString GIndiEntry::familyParent() const {
    return _famsNode ? _famsNode->data() : QString();
}

//=== Mutators ===//

/* Set a new value for the romanized name
 * string value in the GNode data tree, and
 * create a node in the tree if needed.
 */
void GIndiEntry:: setRomanizedName(const QString & romanName) {
    // Append a new node to the tree if needed
    if (!_romanNode) {
        // Create the new node
        _romanNode = new GNode("2 ROMN " + romanName);
        // Append it to the end of _nameNode's children
        GNode * n = _nameNode->firstChild();
        if (!n) { // No existing first child
            _nameNode->setFirstChild(_romanNode);
        }
        else { // Pre-existing sub-list
            // Go to the end of the sub-list
            while (n->next()) n = n->next();
            // Append the new node there
            n->setNext(_romanNode);
        }
    }
    // Alter the existing node if needed
    else {
        _romanNode->setData(romanName);
    }
}

//=== Private Helper Methods ===//

/* Find all important data nodes in
 * this individual's GEDCOM sub-tree
 * @n = Individual's first child node
 */
void GIndiEntry::parseIndiData(GNode * n) {
    while (n) {
        // Name
        if (!_nameNode && n->type() == TYPE_NAME) {
            parseNames(n);
        }
        // Sex
        else if (!_sexNode && n->type() == TYPE_SEX) {
            parseSex(n);
        }
        // Birth
        else if (!_birthNode && n->type() == TYPE_BIRTH) {
            parseBirth(n);
        }
        // Death
        else if (!_deathNode && n->type() == TYPE_DEATH) {
            parseDeath(n);
        }
        // Family (Child)
        else if (!_famcNode && n->type() == TYPE_FAMC) {
            _famcNode = n;
        }
        // Family (Parent)
        else if (!_famsNode && n->type() == TYPE_FAMS) {
            _famsNode = n;
        }
        n = n->next();
    }
}

/* Parses the name data from the GNode tree
 * @n = Individual's the "1 NAME" node
 */
void GIndiEntry::parseNames(GNode * n) {
    // Extract Name Data
    _nameNode = n;
    // Check for Romanized Name Data
    n = n->firstChild();
    while (n && n->type() != TYPE_ROMANIZED_NAME) {
        n = n->next();
    }
    // Extract Romanized Name Data
    _romanNode = n;
}

/* Parses the sex data from the GNode tree
 * @n = Individual's the "1 SEX" node
 */
void GIndiEntry::parseSex(GNode * n) {
    // Extract sex node
    _sexNode = n;
}

/* Parses the birth data from the GNode tree
 * @n = Individual's the "1 BIRT" node
 */
void GIndiEntry::parseBirth(GNode * n) {
    // Find birth date
    if (n) {
        n = n->firstChild();
        while (n && n->type() != TYPE_DATE) {
            n = n->next();
        }
    }
    // Extract birth date
    _birthNode = n;
    // Convert date string to an object for calculation purposes
    _birthYear = _birthNode ? QDate::fromString(_birthNode->data().right(4),"yyyy") : QDate();
}

/* Parses the death data from the GNode tree
 * @n = Individual's the "1 DEAT" node
 */
void GIndiEntry::parseDeath(GNode * n) {
    // Find death date
    if (n) {
        n = n->firstChild();
        while (n && n->type() != TYPE_DATE) {
            n = n->next();
        }
    }
    // Extract death date
    _deathNode = n;
    // Convert date string to an object for calculation purposes
    _deathYear = _deathNode ? QDate::fromString(_deathNode->data().right(4),"yyyy") : QDate();
}
