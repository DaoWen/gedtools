#ifndef V_GINDI_ENTRY_H
#define V_GINDI_ENTRY_H

#include "GNode.h"
#include <QDate>
#include <QStringList>

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
    static const char MALE[];
    static const char FEMALE[];

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
    QDate birthYear() const;

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
    QDate deathYear() const;

    /* Get a copy of the death place string
     * value in the GNode data tree
     */
    QString deathPlace() const;

    /* Returns true if the individual's
     * death date is set to "DECEASED"
     */
    bool deceased() const;

    /* Returns true if the individual has any death information
     */
    bool dead() const;

    /* Returns true if the individual's
     * family pedigree property is "adopted"
     */
    bool adopted() const;

    /* Get a copy of the family (child) ID
     *  string value in the GNode data tree
     */
    QString familyChild() const;

    /* Get a copy of the family (parent) ID
     *  string value in the GNode data tree
     */
    QString familyParent() const;

    /* Get the list of IDs of all families
     * in which this individual is a parent
     * (null if only 0-1 marriages exist)
     */
    const QStringList * marriages() const;

    //=== Mutators ===//

    /* Set a new value for the romanized name
     * string value in the GNode data tree, and
     * create a node in the tree if needed.
     */
    void setRomanizedName(const QString & romanName);

    /* Sets an individual's estimated birth
     * year and updates the BIRT DATE node value
     */
    void setBirthYear(const QDate & year, const QString & datePrefix);

    /* Sets the PLAC value for the BIRT node
     */
    void setBirthPlace(const QString & place);

    /* Sets an individual's death node,
     * and optionally the date value to "DECEASED"
     */
    void setDeceased(bool annotate);

    /* Sets the PLAC value for the DEAT node
     */
    void setDeathPlace(const QString & place);

private:

    //=== Private Data Members ===//

    // Links GNode entries to corresponding data
    GNode * _indiNode, * _nameNode, * _romanNode, * _sexNode;
    GNode * _birthNode, * _birthDateNode, * _birthPlaceNode;
    GNode * _deathNode, * _deathDateNode, * _deathPlaceNode;
    GNode * _famsNode, * _famcNode;

    QStringList * _marriages;

    // Date objects for performing missing date estimates
    QDate _birthYear, _deathYear;

    // Is this person adopted?
    bool _adopted;

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

    /* Appends an empty BIRT node just after
     * the last NAME or SEX node
     */
    void appendBirthNode();

    /* Appends an empty DEAT node just after
     * the last NAME, SEX or BIRT node
     */
    void appendDeathNode();

};

#endif
