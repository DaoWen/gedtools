#ifndef V_GFAMILY_TREE_H
#define V_GFAMILY_TREE_H

#include "GFamilyMap.h"

/* GFamilyTree Node */
class GFTNode {
public:

    /* Empty Constructor */
    GFTNode();

    /* Constructor */
    GFTNode(GFamily * thisFamily, GIndiEntry * famHead, GIndiEntry * familySpouse,
      const QString & famName, GFTNode * parentFamily, int lvl);

    /* Destructor */
    ~GFTNode();

    // GFamily object corresponding to this node
    GFamily * thisFam;

    // Head member of the family (the husband
    // if this node is a child of the husband's
    // family, the wife if it's a child of hers,
    // or an unmarried individual)
    GIndiEntry * famHead;

    // Spouse of the family head
    // (null if famHead is single)
    GIndiEntry * spouse;

    // Names of both parents "famHead & spouse"
    // or the name of an unmarried individual
    QString famName;

    // Family where this individual is a child
    GFTNode * parentFam;

    // Pointers to natural childrens' families
    QList<GFTNode *> * naturalChildFams;

    // Pointers to all childrens' families (including adopted)
    QList<GFTNode *> * allChildFams;

    // How deep is this node in the tree (starts at 0)
    int level;

    // Stores references to the closest nodes up/down that
    // have a birth date set (for calculation purposes)
    // (upperDate is closer to root, lowerDate to leaves)
    GFTNode * upperDate, * lowerDate;

    // Tells whether any dates are missing for this node
    bool headComplete, kidsComplete;

    // Returns true if the head's birth date is set
    inline bool hasBirthDate() {
        return famHead && !famHead->birthDate().isEmpty();
    }

    // Returns true if the head's birth date is set
    inline bool hasMarriageDate() {
        return thisFam && !thisFam->marriageDate().isEmpty();
    }

    // Returns the oldest child of the parentFam
    inline GFTNode * eldestSibling() {
        return parentFam->naturalChildFams->at(0);
    }

};

class GFamilyTree : public QObject {

Q_OBJECT // Qt Library Macro

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
    GFTNode * buildBranch(GFamily * fam, GIndiEntry * head, GIndiEntry * spouse, GFTNode * parent, int level);

    /* Construct a name to display for this family
     * Couple: "head & spouse"
     * Individual: "head"
     */
    QString getFamilyName(GIndiEntry * head, GIndiEntry * spouse);

    /* Find the spouse of the head of a family
     */
    GIndiEntry * getSpouse(GFamily * fam, GIndiEntry * head);

    /* Recursively destroy a branch of the tree */
    void deleteBranch(GFTNode * n);

};

typedef QList<GFamilyTree *> GFTList;

#endif
