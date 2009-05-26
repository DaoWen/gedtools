#ifndef V_GFAMILY_TREE_H
#define V_GFAMILY_TREE_H

#include "GFamilyMap.h"

/* GFamilyTree Node */
class GFTNode {
public:

    /* Empty Constructor */
    GFTNode();

    /* Constructor */
    GFTNode(GFamily * thisFamily, GIndiEntry * famHead, const QString & famName, GFTNode * parentFamily);

    /* Destructor */
    ~GFTNode();

    // GFamily object corresponding to this node
    GFamily * thisFam;

    // Head member of the family (the husband
    // if this node is a child of the husband's
    // family, the wife if it's a child of hers,
    // or an unmarried individual)
    GIndiEntry * famHead;

    // Names of both parents "famHead & spouse"
    // or the name of an unmarried individual
    QString famName;

    // Family where this individual is a child
    GFTNode * parentFam;

    // Pointers to all childrens' families
    QList<GFTNode *> * childFams;
};

class GFamilyTree {
public:

    //=== Constructor/Destructor ===//

    /* Constructor (Recursive)
     * Pass in the root family and this will
     * construct the tree down to the leaves
     */
    GFamilyTree(GFamily * rootFam, GFamilyMap & famMap, GIndiMap & indiMap);

    /* Destructor (Recursive)
     * Destroys the entire tree of GFTNodes
     */
    ~GFamilyTree();

    //=== Accessors ===//

    /* Get the root node of this family tree */
    GFTNode * root() const;

private:

    //=== Private Data Members ===//

    // Family Tree Root
    GFTNode * _root;

    // Map References
    GFamilyMap & _famMap;
    GIndiMap & _indiMap;

    //=== Private Helper Methods ===//

    /* Recursively build GFTNodes for
     * this branch of the family tree
     */
    GFTNode * buildBranch(GFamily * fam, GIndiEntry * head, GFTNode * parent);

    /* Construct a name to display for this family
     * Couple: "head & spouse"
     * Individual: "head"
     */
    QString getFamilyName(GFamily * fam, GIndiEntry * head);

    /* Recursively destroy a branch of the tree */
    void deleteBranch(GFTNode * n);

};

#endif
