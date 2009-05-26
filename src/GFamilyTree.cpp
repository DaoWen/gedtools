
#include "GFamilyTree.h"

//=== GFTNode ===//

/* Empty Constructor */
GFTNode::GFTNode() : thisFam(0), famHead(0), parentFam(0), childFams(0) {}

/* Constructor */
GFTNode::GFTNode(GFamily * thisFamily, GIndiEntry * familyHead, const QString & familyName, GFTNode * parentFamily) :
  thisFam(thisFamily), famHead(familyHead), famName(familyName), parentFam(parentFamily), childFams(0) {}

/* Destructor */
GFTNode::~GFTNode() {
    delete childFams;
}

//=== Constructor/Destructor ===//

/* Constructor (Recursive)
 * Pass in the root family and this will
 * construct the tree down to the leaves
 */
GFamilyTree::GFamilyTree(GFamily * rootFam, GFamilyMap & famMap, GIndiMap & indiMap) :
  _famMap(famMap), _indiMap(indiMap) {
    QString famHeadId = rootFam->husband().isNull() ? rootFam->wife() : rootFam->husband();
    _root = buildBranch(rootFam, *indiMap.find(famHeadId),0);
}

/* Destructor (Recursive)
 * Destroys the entire tree of GFTNodes
 */
GFamilyTree::~GFamilyTree() {
    deleteBranch(_root);
}

//=== Accessors ===//

/* Get the root node of this family tree */
GFTNode * GFamilyTree::root() const {
    return _root;
}

//== Private Helper Methods ===//

/* Recursively build GFTNodes for
 * this branch of the family tree
 */
GFTNode * GFamilyTree::buildBranch(GFamily * fam, GIndiEntry * head, GFTNode * parent) {
    GFTNode * n;
    if (!fam) { // No FAMS family entry
        n = new GFTNode(fam, head, head->name(), parent);
    }
    // Family with children
    else {
        n = new GFTNode(fam, head, getFamilyName(fam, head), parent);
        // "Head Individual" Child
        GIndiEntry * child;
        // Child Family List & Iterators
        QList<GFTNode *> * childFamilies = new QList<GFTNode *>();
        QStringList::ConstIterator i = fam->children().begin();
        QStringList::ConstIterator end = fam->children().end();
        // Child Family
        GFamilyMap::Iterator famIterator, famEnd = _famMap.end();
        GFamily * childFam;
        // Add all childrens' families to the list
        while (i != end) {
            // Find this child's GIndiEntry
            child = *(_indiMap.find(*i++));
            // Find family for which this child is a parent
            famIterator = _famMap.find(child->familyParent());
            // No GFamily with this individual as a head
            if (famIterator == famEnd) {
                childFam = 0;
            }
            // Individual heads a family
            else {
                childFam = *famIterator;
            }
            childFamilies->append(buildBranch(childFam,child,n));
        }
        n->childFams = childFamilies;
    }
    return n;
}

/* Construct a name to display for this family
 * Couple: "head & spouse"
 * Individual: "head"
 */
QString GFamilyTree::getFamilyName(GFamily * fam, GIndiEntry * head) {
    QString familyName;
    // If head is the husband, then append wife, otherwise append husband
    QString spouseID = head->id() == fam->husband() ? fam->wife() : fam->husband();
    familyName = head->name();
    GIndiMap::Iterator spouse = _indiMap.find(spouseID);
    if (spouse != _indiMap.end()) {
        // TODO: Should the " & " be translated?
        familyName.append(" & ").append((*spouse)->name());
    }
    return familyName;
}

/* Recursively destroy a branch of the tree */
void GFamilyTree::deleteBranch(GFTNode * n) {
    if (n->childFams) {
        foreach (GFTNode * child, *n->childFams) {
            deleteBranch(child);
        }
    }
    delete n;
}
