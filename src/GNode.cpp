
#include <QRegExp>
#include "GNode.h"

//=== Static Data ===//

// RegExp used to parse line data
QRegExp LINE_EXP("\\s*(\\d+) ([^ ]+)(.*)");

//=== Constructor/Destructor ===//

/* Constructor
 * Pass in a line from the GEDCOM file
 * It will be split into a level, type, and data string.
 */
GNode::GNode(const QString & dataStr) : _firstChild(0), _next(0) {
    // Parse the string with a RegExp
    if (LINE_EXP.exactMatch(dataStr)) {
        _level = LINE_EXP.cap(1).toInt();
        _type = LINE_EXP.cap(2);
        _data = LINE_EXP.cap(3).trimmed();
    }
    else {
        throw QString("Invalid Line!");
    }
}

/* Destructor
 * Deletes this node's children
 * and the next node in the tree
 */
GNode::~GNode() {
    delete _firstChild;
    delete _next;
}

//=== Accessors ===//

/* Gets the next node in
 * this level of the tree
 */
GNode * GNode::next() const {
    return _next;
}

/* Gets the first node in the
 * next level of the tree
 */
GNode * GNode::firstChild() const {
    return _firstChild;
}

/* Gets the level number of this line
 */
int GNode::level() const {
    return _level;
}

/* Gets the type of this data line
 */
QString GNode::type() const {
    return _type;
}

/* Gets the data string of this line
 */
QString GNode::data() const {
    return _data;
}

//=== Mutators ===//

/* Sets the next node in
 * this level of the tree
 */
void GNode::setNext(GNode * n) {
    _next = n;
}

/* Inserts a new node between
 * this node and its next sibling
 */
GNode * GNode::insertNext(GNode * n) {
    // Don't insert null nodes
    if (n) {
        // Link in existing siblings
        if (this->_next) {
            n->setNext(this->_next);
        }
        // Link in the new node
        this->setNext(n);
    }
    return n;
}

/* Sets the first node in the
 * next level of the tree
 */
void GNode::setFirstChild(GNode * n) {
    _firstChild = n;
}

/* Sets the level number of this line
 */
void GNode::setLevel(int lvl) {
    _level = lvl;
}

/* Sets the type of this data line
 */
void GNode::setType(const QString & tStr) {
    _type = tStr;
}

/* Sets the data string of this line
 */
void GNode::setData(const QString & dStr) {
    _data = dStr;
}

//=== Static Utility Methods ===//

/* Creates a QDate object to represent
 * the value of a DATE node (year only)
 */
QDate GNode::parseDateNode(GNode * dateNode) {
    QDate dateYear;
    static QRegExp YEAR_EXP("(\\d{4})( BC)?");
    if (dateNode) {
        if (YEAR_EXP.indexIn(dateNode->data()) > -1) {
            bool conversionOK;
            int intYear = YEAR_EXP.cap(1).toInt(&conversionOK);
            if (conversionOK) {
                // If "BC" is in the date then make it negative
                if (!YEAR_EXP.cap(2).isEmpty()) {
                    intYear *= -1;
                }
                // Use default value of January 1 for the month and year
                dateYear = QDate(intYear,1,1);
            }
        }
    }
    return dateYear;
}
