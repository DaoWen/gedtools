#ifndef V_GFAMILY_H
#define V_GFAMILY_H

#include <QStringList>
#include "GNode.h"
#include "GIndiMap.h"

/* GFamily: Representation of a family in a GEDCOM file
 * This class contains all relevant data found in a GEDCOM entry
 * for a family (FAM). Not only does it hold local copies of
 * the relevant data strings (for convenience when editing),
 * but it also holds references to the GNodes containing the
 * relevant data for alteration of the actual data file.
 */
class GFamily {
public:

    //=== Constructor/Destructor ===//

    /* Constructor
     * Builds a family based on the data
     * parsed from a GEDCOM file, and links it
     * to the proper GNodes, starting with the GNode
     * pointing to this family's 0-level record.
     */
    GFamily(GNode * n);

    /* Destructor */
    ~GFamily();

    //=== Accessors ===//

    /* Get a copy of the ID string
     * value in the GNode data tree
     */
    QString id() const;

    /* Get a copy of the ID string
     * value of the husband
     */
    QString husband() const;

    /* Get a copy of the ID string
     * value of the wife
     */
    QString wife() const;

    /* Get the list of ID strings
     * for all of the children
     */
    const QStringList & children() const;

    /* Date of the marriage */
    QString marriageDate() const;

    /* Year of the marriage (useful in calculations) */
    QDate marriageYear() const;

    /* Location of the marriage */
    QString marriagePlace() const;

    /* If neither parent in this family is a child
     * in another family then this family is the root
     */
    bool isTreeRoot(GIndiMap & indiMap) const;

    /* If this family contains no children then
     * it must be a leaf in the family tree
     */
    bool isTreeLeaf(GIndiMap & indiMap) const;

    //=== Mutators ===//

    /* Sets a couple's estimated marriage
     * year and updates the MARR node value
     */
    void setMarriageYear(const QDate & year, const QString & place = QString());

private:

    //=== Private Data Members ===//

    // Links GNode entries to corresponding data
    GNode * _familyNode, * _husbandNode, * _wifeNode;
    GNode * _marriageNode, * _marriageDateNode, * _marriagePlaceNode;

    // Year of marriage as a QDate for calculations
    QDate _marriageYear;

    // Children's xref_id data
    QStringList _childrenIDs;

    //=== Private Helper Methods ===//

    /* Appends an empty MARR node just after
     * the last HUSB, WIFE, or CHIL node
     */
    void appendMarriageNode();

    /* Parses the family member
     * references for the parents and
     * children from this level of
     * the GNode tree
     */
    void parseMembers(GNode * n);

};

#endif
