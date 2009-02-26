
#include <QFile>
#include <QList>
#include <QStack>
#include <QTextStream>
#include "GFile.h"

//=== Constructor/Destructor ===//

/* Constructor
 * Pass in a GEDCOM file name to be read and parsed
 * The result is a linked list-tree combo (a linked
 * list with subtrees of linked lists)
 */
GFile::GFile(const QString & fileName) : _root(0), _indiMap(new GIndiMap()) {
    // Create an input stream for Unicode
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QTextStream input(&file);
    input.setCodec("UTF-8");
    // List of all the individuals to parse
    QList<GNode*> indiNodes;
    // Variables used for input loop
    QString line; // Stores the current line
    GNode * n;    // Stores the node made from line
    // Stores references to the last node in each level of the tree
    QStack<GNode*> headNodes;
    // Start parsing the tree
    if (!input.atEnd()) {
        // Parse the 0 HEAD "root" node
        line = input.readLine();
        n = new GNode(line);
        headNodes.push(n);
        _root = n;
        // Parse all other nodes
        while (!input.atEnd()) {
            line = input.readLine();
            n = new GNode(line);
            // If this level is greater than the current level
            // Then we need to create a sublevel in the list-tree
            if (n->level() > headNodes.top()->level()) {
                // Link in a new child tree to the list
                headNodes.top()->setFirstChild(n);
                headNodes.push(n);
            }
            else {
                // Pop pointers from the stack until we find one
                // with the same level as the node we want to add
                // (unless they're already at the same level)
                while (n->level() < headNodes.top()->level()) {
                    headNodes.pop();
                }
                // Link in the new node in the list
                headNodes.top()->setNext(n);
                headNodes.top() = n;
            }
            // Add this node to the list of individuals to parse
            if (n->data() == "INDI") {
                indiNodes.append(n);
            }
        }
    }
    file.close();
    QList<GNode*>::const_iterator i = indiNodes.begin();
    QList<GNode*>::const_iterator end = indiNodes.end();
    for (;i!=end;++i) {
        _indiMap->insert(*i);
    }
}

/* Destructor
 * Deletes the maps and GNode tree
 */
GFile::~GFile() {
    delete _root; // Recursively deletes the whole tree
    delete _indiMap;
}

//=== Accessors ===//

/* Returns the pointer to the
 * root of the parsed list-tree.
 */
GNode * GFile::parsedTree() const {
    return _root;
}

/* Returns a reference to the
 * map of individuals' IDs to the
 * corresponding GIndiEntry objects
 */
GIndiMap & GFile::indiMap() {
    return *_indiMap;
}

//=== Utility Functions ===//

/* Attempts to write the data stored in the
 * GNode tree to the specified file. If the
 * save fails then the method returns false.
 */
bool GFile::saveFile(const QString & fileName) const {
    // Create an output stream for Unicode
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    QTextStream output(&file);
    output.setCodec("UTF-8");
    // todo: write the utf-8 bom to the beginning of the file
    // Write the data out
    printGedcomFile(output,_root);
    file.close();
    return true;
}

//=== Private Helper Methods ===//

void GFile::printGedcomFile(QTextStream & s, GNode * n) const {
    if (n) {
        // First stream out the contents of this node in the proper format:
        // LEVEL# TYPE DATA
        s << QString::number(n->level()).append(' ').append(n->type()).append(n->data()).append('\n');
        // Print out this node's children
        printGedcomFile(s, n->firstChild());
        // Print out this node's next sibling
        printGedcomFile(s, n->next());
    }
}
