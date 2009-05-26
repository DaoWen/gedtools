#ifndef V_GINDI_ENTRY_H
#define V_GINDI_ENTRY_H

#include "GNode.h"
#include <QDate>

/* GIndiEntry: Representation of an individual in a GEDCOM file
 * This class contains all relevant data found in a GEDCOM entry
 * for an individual (INDI). Not only does it hold local copies
 * of the relevant data strings (for convenience when editing),
 * but it also holds references to the GNodes containing the
 * relevant data for alteration of the actual data file.
 */
class GIndiEntry {
public:

    //=== Static Constants ===//

    // Male and Female values corresponding to sex
    static const char * MALE;
    static const char * FEMALE;

    //=== Constructors ===//

    /* Constructor
     * Builds an Individual based on the data
     * parsed from a GEDCOM file, and links it
     * to the proper GNodes, starting with the GNode
     * pointing to this individual's 0-level record.
     */
    GIndiEntry(GNode * n);

    //=== Accessors ===//

    /* Get a copy of the ID string
     * value in the GNode data tree
     */
    QString id() const;

    /* Get a copy of the name string
     * value in the GNode data tree
     */
    QString name() const;

    /* Get a copy of the romanized name
     * string value in the GNode data tree
     */
    QString romanizedName() const;

    /* Get a copy of the sex string
     * value in the GNode data tree
     */
    QString sex() const;

    /* Get a copy of the birth date string
     * value in the GNode data tree
     */
    QString birthDate() const;

    /* Get the year in which this individual was born
     * (helpful for doing date calculations)
     */
    QDate birthYear();

    /* Get a copy of the birth place string
     * value in the GNode data tree
     */
    QString birthPlace() const;

    /* Get a copy of the death date string
     * value in the GNode data tree
     */
    QString deathDate() const;

    /* Get the year in which this individual died
     * (helpful for doing date calculations)
     */
    QDate deathYear();

    /* Get a copy of the death place string
     * value in the GNode data tree
     */
    QString deathPlace() const;

    /* Returns true if the individual's
     * death date is set to "DECEASED"
     */
    bool deceased();

    /* Get a copy of the family (child) ID
     *  string value in the GNode data tree
     */
    QString familyChild() const;

    /* Get a copy of the family (parent) ID
     *  string value in the GNode data tree
     */
    QString familyParent() const;

    //=== Mutators ===//

    /* Set a new value for the romanized name
     * string value in the GNode data tree, and
     * create a node in the tree if needed.
     */
    void setRomanizedName(const QString & romanName);

    /* Sets an individual's estimated birth
     * year and updates the BIRT node value
     */
    void setBirthYear(const QDate & year, const QString & place = QString());

    /* Sets an individual's death
     * date value to "DECEASED"
     */
    void setDeceased(const QString & place = QString());

private:

    //=== Private Data Members ===//

    // Links GNode entries to corresponding data
    GNode * _indiNode, * _nameNode, * _romanNode, * _sexNode;
    GNode * _birthDateNode, * _birthPlaceNode;
    GNode * _deathDateNode, * _deathPlaceNode;
    GNode * _famsNode, * _famcNode;

    // Date objects for performing missing date estimates
    QDate _birthYear, _deathYear;

    //=== Private Helper Methods ===//

    /* Find all important data nodes in
     * this individual's GEDCOM sub-tree
     * @n = Individual's first child node
     */
    void parseIndiData(GNode * n);

    /* Parses the name data from the GNode tree
     * @n = Individual's the "1 NAME" node
     */
    void parseNames(GNode * n);

    /* Parses the birth data from the GNode tree
     * @n = Individual's the "1 BIRT" node
     */
    void parseBirth(GNode * n);

    /* Parses the death data from the GNode tree
     * @n = Individual's the "1 DEAT" node
     */
    void parseDeath(GNode * n);

};

#endif
