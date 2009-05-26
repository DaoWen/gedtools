#ifndef V_GNODE_H
#define V_GNODE_H

#include <QString>

/* GNode: Single node in the GEDCOM list-tree
 * GEDCOM files have a complex structure resembling
 * a tree of linked lists. Each line in the file
 * corresponds to a single node in our list-tree.
 *
 * The GEDCOM Standard Release 5.5 can be found at:
 * http://homepages.rootsweb.ancestry.com/~pmcbride/gedcom/
 */
class GNode {
public:

    //=== Constructor/Destructor ===//

    /* Constructor
     * Pass in a line from the GEDCOM file
     * It will be split into a level, type, and data string.
     */
    GNode(const QString & dataStr);

    /* Destructor
     * Deletes this node, its children,
     * and the next node in the tree
     */
    ~GNode();

    //=== Accessors ===//

    /* Gets the next node in
     * this level of the tree
     */
    GNode * next() const;

    /* Gets the first node in the
     * next deepest level of the tree
     */
    GNode * firstChild() const;

    /* Gets the level number of this line
     */
    int level() const;

    /* Gets the type of this data line
     */
    QString type() const;

    /* Gets the data string of this line
     */
    QString data() const;

    //=== Mutators ===//

    /* Sets the next node in
     * this level of the tree
     */
    void setNext(GNode * n);

    /* Inserts a new node between
     * this node and its next sibling
     */
    GNode * insertNext(GNode * n);

    /* Sets the first node in the
     * next level of the tree
     */
    void setFirstChild(GNode * n);

    /* Sets the level number of this line
     */
    void setLevel(int lvl);

    /* Sets the type of this data line
     */
    void setType(const QString & tStr);

    /* Sets the data string of this line
     */
    void setData(const QString & dStr);

private:

    //=== Private Data Members ===//

    // Pointers to related nodes in this file
    GNode * _firstChild, * _next;
    // The "level" number from this line
    int _level;
    // The "type" lable from this line
    QString _type;
    // The data string on the end of this line
    QString _data;

};

#endif
