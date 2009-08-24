
#include "GFamilyTree.h"

//=== GFTNode ===//

/* Empty Constructor */
GFTNode::GFTNode()
 : thisFam(0), famHead(0), spouse(0), parentFam(0), childFams(0), level(-1),
   upperDate(0), lowerDate(0), headComplete(false), kidsComplete(false) {}

/* Constructor */
GFTNode::GFTNode(GFamily * thisFamily, GIndiEntry * familyHead, GIndiEntry * familySpouse,
  const QString & familyName,GFTNode * parentFamily, int lvl)
 : thisFam(thisFamily), famHead(familyHead), spouse(familySpouse), famName(familyName), parentFam(parentFamily),
   childFams(0), level(lvl), upperDate(0), lowerDate(0), headComplete(false), kidsComplete(false) {}

/* Destructor */
GFTNode::~GFTNode() {
    delete childFams;
}

//=== Constructor/Destructor ===//

/* Constructor (Recursive)
 * Pass in the root family and this will
 * construct the tree down to the leaves
 */
GFamilyTree::GFamilyTree(GFamily * rootFam, GFamilyMap & famMap, GIndiMap & indiMap)
 : _famMap(famMap), _indiMap(indiMap) {
    QString famHeadId = rootFam->husband().isNull() ? rootFam->wife() : rootFam->husband();
    // The root node has a level of 0
    GIndiMap::Iterator i = indiMap.find(famHeadId);
    GIndiEntry * head = (i == indiMap.end()) ? 0 : i.value();
    _root = buildBranch(rootFam, head, getSpouse(rootFam, head), 0, 0);
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
GFTNode * GFamilyTree::buildBranch(GFamily * fam,
  GIndiEntry * head, GIndiEntry * spouse, GFTNode * parent, int level) {
    GFTNode * n;
    if (!fam) { // No FAMS family entry
        n = new GFTNode(fam, head, 0, head->name(), parent, level);
    }
    // Family with children
    else {
        n = new GFTNode(fam, head, spouse, getFamilyName(head, spouse), parent, level);
        // "Head Individual" Child & Spouse
        GIndiEntry * child, * childSpouse;
        // Child Family List & Iterators
        QList<GFTNode *> * childFamilies = new QList<GFTNode *>();
        QStringList::ConstIterator i = fam->children().begin();
        QStringList::ConstIterator end = fam->children().end();
        GIndiMap::ConstIterator foundChildIterator;
        // Child Family
        GFamilyMap::Iterator famIterator, famEnd = _famMap.end();
        GFamily * childFam;
        // Add all childrens' families to the list
        while (i != end) {
            // Find this child's GIndiEntry (if any)
            foundChildIterator = _indiMap.find(*i);
            child = (foundChildIterator == _indiMap.end()) ? 0 : foundChildIterator.value();
            // No GFamily with this individual as a head
            if (!child || (famIterator = _famMap.find(child->familyParent())) == famEnd) {
                childFam = 0;
                childSpouse = 0;
            }
            // Individual heads a family
            else {
                childFam = *famIterator;
                childSpouse = getSpouse(childFam, child);
            }
            childFamilies->append(buildBranch(childFam,child,childSpouse,n,level+1));
            ++i;
        }
        n->childFams = childFamilies;
    }
    return n;
}

/* Construct a name to display for this family
 * Couple: "head & spouse"
 * Individual: "head"
 */
QString GFamilyTree::getFamilyName(GIndiEntry * head, GIndiEntry * spouse) {
    QString familyName;
    if (head) {
        familyName = head->name();
        if (spouse) familyName.append(" & ").append(spouse->name());
    }
    else { // Parentless family
        familyName = tr("Unknown");
    }
    return familyName;
}

/* Find the spouse of the head of a family
 */
GIndiEntry * GFamilyTree::getSpouse(GFamily * fam, GIndiEntry * head) {
    if (fam && head) {
        // If head is the husband, then find the wife, otherwise find the husband
        QString spouseID = head->id() == fam->husband() ? fam->wife() : fam->husband();
        GIndiMap::Iterator spouse = _indiMap.find(spouseID);
        return (spouse == _indiMap.end()) ? 0 : *spouse;
    }
    else {
        return 0;
    }
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
