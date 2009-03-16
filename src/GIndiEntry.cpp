
#include "GIndiEntry.h"

//=== Constants ===//

// Level-0 individual records have this data entry
const char DATA_INDI[] = "INDI";
// Level-1 name attributes have this type
const char TYPE_NAME[] = "NAME";
// Level-2 romanized name attributes have this type
const char TYPE_ROMANIZED_NAME[] = "ROMN";
// Level-1 birth attributes have this type
const char TYPE_BIRTH[] = "BIRT";
// Level-1 death attributes have this type
const char TYPE_DEATH[] = "DEAT";
// Level-2 name attributes have this type
const char TYPE_DATE[] = "DATE";

//=== Constructors ===//

/* Constructor
 * Builds an Individual based on the data
 * parsed from a GEDCOM file, and links it
 * to the proper GNodes, starting with the GNode
 * pointing to this individual's 0-level record.
 */
GIndiEntry::GIndiEntry(GNode * n) : _indiNode(0), _nameNode(0), _romanNode(0) {
    // Validate that this is an individual record
    if (!n) {
        throw QString("Null Pointer to Individual");
    }
    else if (n->level() != 0 || n->data() != DATA_INDI) {
        throw QString("Bad Individual");
    }
    // Save this individual's data
    _indiNode = n; // Save a link to this individual's GNode
    n = n->firstChild();
    // Get Name Data
    parseNames(n);
    // Get Birth & Death Dates
    parseBirth(n);
    parseDeath(n);
}

//=== Accessors ===//

/* Get a pointer to the ID string
 * value in the GNode data tree
 */
QString GIndiEntry::id() const {
    // ID is stored in the type slot for some reason
    return _indiNode->type();
}

/* Get a pointer to the name string
 * value in the GNode data tree
 */
QString GIndiEntry::name() const {
    return _nameNode->data();
}

/* Get a pointer to the romanized name
 * string value in the GNode data tree
 */
QString GIndiEntry::romanizedName() const {
    // This node might not exist...
    return _romanNode ? _romanNode->data() : QString();
}

/* Get a pointer to the birth date string
 * value in the GNode data tree
 */
QString GIndiEntry::birthDate() const {
    // This node might not exist...
    return _birthNode ? _birthNode->data() : QString();
}

/* Get a pointer to the death date string
 * value in the GNode data tree
 */
QString GIndiEntry::deathDate() const {
    // This node might not exist...
    return _deathNode ? _deathNode->data() : QString();
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

/* Parses the name data from the GNode tree */
void GIndiEntry::parseNames(GNode * n) {
    // Find Name Data Entry
    while (n && n->type() != TYPE_NAME) {
        n = n->next();
    }
    if (!n) {
        throw QString("Bad Individual: No Name");
    }
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

/* Parses the birth data from the GNode tree */
void GIndiEntry::parseBirth(GNode * n) {
    // Find birth data entry
    while (n && n->type() != TYPE_BIRTH) {
        n = n->next();
    }
    // Find birth date
    if (n) {
        n = n->firstChild();
        while (n && n->type() != TYPE_DATE) {
            n = n->next();
        }
    }
    // Extract birth date
    _birthNode = n;
}

/* Parses the death data from the GNode tree */
void GIndiEntry::parseDeath(GNode * n) {
    // Find death data entry
    while (n && n->type() != TYPE_DEATH) {
        n = n->next();
    }
    // Find death date
    if (n) {
        n = n->firstChild();
        while (n && n->type() != TYPE_DATE) {
            n = n->next();
        }
    }
    // Extract death date
    _deathNode = n;
}
