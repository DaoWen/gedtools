
#include "GIndiEntry.h"

//=== Constants ===//

// Todo: Move all of these into a separate header file
// (move the ones in GFamily.cpp too)

// Level-0 individual records have this data entry
const char DATA_INDI[] = "INDI";
// Level-2 death dates sometimes have this data entry
const char DATA_DECEASED[] = "DECEASED";
// Level-1 name attributes have this type
const char TYPE_NAME[] = "NAME";
// Level-2 romanized name attributes have this type
const char TYPE_ROMANIZED_NAME[] = "ROMN";
// Level-1 sex attributes have this type
const char TYPE_SEX[] = "SEX";
// Level-1 birth entry (empty)
const char ENTRY_BIRTH[] = "1 BIRT";
// Level-1 birth attributes have this type
const char TYPE_BIRTH[] = "BIRT";
// Level-1 death entry (empty)
const char ENTRY_DEATH[] = "1 DEAT";
// Level-1 death attributes have this type
const char TYPE_DEATH[] = "DEAT";
// Level-2 date entry
const char ENTRY_DATE[] = "2 DATE";
// Level-2 place entry
const char ENTRY_PLACE[] = "2 PLAC";
// Level-2 date attributes have this type
const char TYPE_DATE[] = "DATE";
// Level-2 place attributes have this type
const char TYPE_PLACE[] = "PLAC";
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
 : _indiNode(0), _nameNode(0), _romanNode(0), _sexNode(0), _birthNode(0),
   _birthDateNode(0), _birthPlaceNode(0), _deathNode(0), _deathDateNode(0),
   _deathPlaceNode(0), _famsNode(0), _famcNode(0), _marriages(0) {
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
    return _birthDateNode ? _birthDateNode->data() : QString();
}

/* Get the year in which this individual was born
 * (helpful for doing date calculations)
 */
QDate GIndiEntry::birthYear() const {
    return _birthYear;
}

/* Get a copy of the birth place string
 * value in the GNode data tree
 */
QString GIndiEntry::birthPlace() const {
    // This node might not exist...
    return _birthPlaceNode ? _birthPlaceNode->data() : QString();
}

/* Get a copy of the death date string
 * value in the GNode data tree
 */
QString GIndiEntry::deathDate() const {
    // This node might not exist...
    return _deathDateNode ? _deathDateNode->data() : QString();
}

/* Get the year in which this individual died
 * (helpful for doing date calculations)
 */
QDate GIndiEntry::deathYear() const {
    return _deathYear;
}

/* Get a copy of the death place string
 * value in the GNode data tree
 */
QString GIndiEntry::deathPlace() const {
    // This node might not exist...
    return _deathPlaceNode ? _deathPlaceNode->data() : QString();
}

/* Returns true if the individual's
 * death date is set to "DECEASED"
 */
bool GIndiEntry::deceased() const {
    return _deathDateNode && _deathDateNode->data() == DATA_DECEASED;
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

/* Get the list of IDs of all families
 * in which this individual is a parent
 * (null if only 0-1 marriages exist)
 */
const QStringList * GIndiEntry::marriages() const {
    return _marriages;
}

//=== Mutators ===//

/* Set a new value for the romanized name
 * string value in the GNode data tree, and
 * create a node in the tree if needed.
 */
void GIndiEntry::setRomanizedName(const QString & romanName) {
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

/* Sets an individual's estimated birth
 * year and updates the BIRT DATE node value
 */
void GIndiEntry::setBirthYear(const QDate & year) {
    if (!year.isValid()) {
        throw QString("Attempted to set an invalid birth date.");
    }
    // Update internal date value
    _birthYear = year;
    // Create BIRT node if needed
    if (!_birthNode) appendBirthNode();
    // Create the DATE node if needed
    if (!_birthDateNode) {
        _birthDateNode = new GNode(ENTRY_DATE);
    }
    // Build the date entry string
    QString dateString("EST ");
    if (year.year() < 0) {
        // Remove the "-" from the front and add "BC"
        dateString.append(year.toString("yyyy BC").mid(1));
    }
    else {
        dateString.append(year.toString("yyyy"));
    }
    // Update nodes
    _birthDateNode->setData(dateString);
    _birthNode->setFirstChild(_birthDateNode);
    _birthDateNode->setNext(_birthPlaceNode);
}

/* Sets the PLAC value for the BIRT node
 */
void GIndiEntry::setBirthPlace(const QString & place) {
    // Create the place node if needed
    if (!_birthPlaceNode) {
        _birthPlaceNode = new GNode(ENTRY_PLACE);
        // Add the new node into the tree
        if (_birthDateNode) _birthDateNode->setNext(_birthPlaceNode);
        else _birthNode->setFirstChild(_birthPlaceNode);
    }
    _birthPlaceNode->setData(place);
}

/* Sets an individual's death
 * date value to "DECEASED"
 */
void GIndiEntry::setDeceased() {
    // Create DEAT node if needed
    if (!_deathNode) appendDeathNode();
    // Create the DATE node if needed
    if (!_deathDateNode) {
        _deathDateNode = new GNode(ENTRY_DATE);
    }
    _deathDateNode->setData(DATA_DECEASED);
    _deathNode->setFirstChild(_deathDateNode);
    _deathDateNode->setNext(_deathPlaceNode);
}

/* Sets the PLAC value for the DEAT node
 */
void GIndiEntry::setDeathPlace(const QString & place) {
    // Create the place node if needed
    if (!_deathPlaceNode) {
        _deathPlaceNode = new GNode(ENTRY_PLACE);
        // Add the new node into the tree
        if (_deathDateNode) _deathDateNode->setNext(_deathPlaceNode);
        else _deathNode->setFirstChild(_deathPlaceNode);
    }
    _deathPlaceNode->setData(place);
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
            _sexNode = n;
        }
        // Birth
        else if (!_birthDateNode && n->type() == TYPE_BIRTH) {
            parseBirth(n);
        }
        // Death
        else if (!_deathDateNode && n->type() == TYPE_DEATH) {
            parseDeath(n);
        }
        // Family (Child)
        else if (!_famcNode && n->type() == TYPE_FAMC) {
            _famcNode = n;
        }
        // Family (Parent)
        else if (n->type() == TYPE_FAMS) {
            // First marriage
            if (!_famsNode) {
                _famsNode = n;
            }
            // Second marriage
            else if (!_marriages) {
                _marriages = new QStringList();
                _marriages->append(_famsNode->data());
                _marriages->append(n->data());
            }
            // Third, fourth, ...
            else {
                _marriages->append(n->data());
            }
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

/* Parses the birth data from the GNode tree
 * @n = Individual's the "1 BIRT" node
 */
void GIndiEntry::parseBirth(GNode * n) {
    // Find birth date
    if (n) {
        _birthNode = n;
        n = n->firstChild();
        while (n) {
            // Extract birth date
            if (n->type() == TYPE_DATE) {
                _birthDateNode = n;
            }
            // Extract birth place
            else if (n->type() == TYPE_PLACE) {
                _birthPlaceNode = n;
            }
            n = n->next();
        }
    }
    // Convert date string to an object for calculation purposes
    _birthYear = GNode::parseDateNode(_birthDateNode);
}

/* Parses the death data from the GNode tree
 * @n = Individual's the "1 DEAT" node
 */
void GIndiEntry::parseDeath(GNode * n) {
    // Find death date
    if (n) {
        _deathNode = n;
        n = n->firstChild();
        while (n) {
            // Extract death date
            if (n->type() == TYPE_DATE) {
                _deathDateNode = n;
            }
            // Extract death place
            else if (n->type() == TYPE_PLACE) {
                _deathPlaceNode = n;
            }
            n = n->next();
        }
    }
    // Convert date string to an object for calculation purposes
    _deathYear = GNode::parseDateNode(_deathDateNode);
}

/* Appends an empty BIRT node just after
 * the last NAME or SEX node
 */
void GIndiEntry::appendBirthNode() {
    GNode * n = _sexNode ? _sexNode : _nameNode;
    // Append BIRT node here
    _birthNode = n->insertNext(new GNode(ENTRY_BIRTH));
}

/* Appends an empty DEAT node just after
 * the last NAME, SEX or BIRT node
 */
void GIndiEntry::appendDeathNode() {
    GNode * n = _birthNode ? _birthNode : _sexNode ? _sexNode : _nameNode;
    // Append BIRT node here
    _deathNode = n->insertNext(new GNode(ENTRY_DEATH));
}
