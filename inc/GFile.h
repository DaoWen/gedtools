#ifndef V_GFILE_H
#define V_GFILE_H

#include "GNode.h"
#include "GIndiMap.h"

class GFile {
public:

    //=== Constructor/Destructor ===//

    /* Constructor
     * Pass in a GEDCOM file name to be read and parsed
     * The result is a linked-list/tree combo (a linked-
     * list with subtrees of linked-lists)
     */
    GFile(const char * fileName);

    /* Destructor
     * Deletes the GNode tree and map
     */
    ~GFile();

    //=== Accessors ===//

    /* Returns the pointer to the
     * root of the parsed list/tree.
     */
    GNode * parsedTree() const;

    /* Returns a reference to the
     * map of individuals' IDs to the
     * corresponding GIndiEntry objects
     */
    GIndiMap & indiMap();

private:

    //=== Private Data Members ===//

    // Root node of the parsed file
    GNode * _root;

    // Map of individual's IDs to their node pointer
    GIndiMap * _indiMap;

    //=== Private Helper Methods ===//

    /* Checks if the current node is an INDI node,
     * and if it is it's added to the IndiMap
     */
    void checkNode(GNode * n);

};

#endif
