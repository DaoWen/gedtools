#ifndef V_GFILE_H
#define V_GFILE_H

#include "GNode.h"
#include "GIndiMap.h"

/* GFile: GEDCOM File Parser
 * This class reads a GEDCOM file and builds a tree of
 * GNodes based on the internal structure of the file.
 *
 * The GEDCOM Standard Release 5.5 can be found at:
 * http://homepages.rootsweb.ancestry.com/~pmcbride/gedcom/
 */
class GFile {
public:

    //=== Constructor/Destructor ===//

    /* Constructor
     * Pass in a GEDCOM file name to be read and parsed
     * The result is a linked list-tree combo (a linked
     * list with subtrees of linked lists)
     */
    GFile(const QString & fileName);

    /* Destructor
     * Deletes the maps and GNode tree
     */
    ~GFile();

    //=== Accessors ===//

    /* Returns the pointer to the
     * root of the parsed list-tree.
     */
    GNode * parsedTree() const;

    /* Returns a reference to the
     * map of individuals' IDs to the
     * corresponding GIndiEntry objects
     */
    GIndiMap & indiMap();

    //=== Utility Functions ===//

    /* Attempts to write the data stored in the
     * GNode tree to the specified file. If the
     * save fails then the method returns false.
     */
    bool saveFile(const QString & fileName) const;

private:

    //=== Private Data Members ===//

    // Root node of the parsed file
    GNode * _root;

    // Map of individual's IDs to their node pointer
    GIndiMap * _indiMap;

    //=== Private Helper Methods ===//

    /* Recursively prints the contents of the GNode
     * list-tree as they would have appeared in the
     * original GEDCOM file.
     */
    void printGedcomFile(QTextStream & s, GNode * n) const;

};

#endif
