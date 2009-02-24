#ifndef V_GNODE_H
#define V_GNODE_H

#include <QString>

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
     * and the next node in the list
     */
    ~GNode();

    //=== Accessors ===//

    /* Gets the next node in
     * this level of the file.
     */
    GNode * next() const;

    /* Gets the first node in the
     * next level of the file.
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
     * this level of the file.
     */
    void setNext(GNode * n);

    /* Sets the first node in the
     * next level of the file.
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
