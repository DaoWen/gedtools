
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
