#ifndef V_GDATE_ESTIMATOR_H
#define V_GDATE_ESTIMATOR_H

#include "GFamilyTree.h"

/* GDateEstimator: Utility class for estimating missing
 * dates in a group of GFamilyTrees created from the
 * data in a GEDCOM file. Estimates birth, marriage and
 * death dates based on the methods set forth in
 * "Filling out the Pedigree Chart"
 * (Hong Kong Family History Service Center, July 2007)
 * Optionally appends automatic PLACE values as well
 */
class GDateEstimator {
public:

    //=== Constructor/Destructor ===//

    /* Constructor */
    GDateEstimator(GFTList & trees, const QString & defaultPlace = QString());

    /* Destructor */
    ~GDateEstimator();

    //=== Mutators ===//

    /* Estimates missing dates for births, marriages
     * and deaths and fills in the estimated values
     * (along with optional locations) in the GNodes
     * Returns the number of dates appeneded
     */
    int estimateMissingDates();

private:

    //=== Private Data Members ===//

    // List of GFamilyTrees to work with
    GFTList & _trees;

    // Default location of birth, marriage and death
    const QString & _defaultPlace;

    // Current date (used to check if an individual is dead)
    QDate _currentYear;

    //=== Private Helper Methods ===//

    //--- Individals ---//

    /* Estimates birth, marriage, and death dates for a
     * couple if relevent data is available and needed.
     * famNode->complete is set true if no more dates are needed
     * @return number of dates added
     */
    int updateCouple(GFTNode * famNode);

    /* Estimates marriage date for a
     * couple if relevent data is available
     * @return number of dates added
     */
    int updateMarriage(GFTNode * famNode);

    /* Estimates birth and death dates for an
     * individual if relevent data is available and needed
     * @return number of dates added to this node
     */
    int updateIndividual(GIndiEntry * indi, GFamily * fam, GIndiEntry * spouse);

    //--- Siblings ---//

    /* Recurse through all children in the tree
     * and update them with respect to siblings
     * @return number of dates added
     */
    int updateChildren(GFTNode * n);

    /* Estimates birth dates for siblings in a family
     * Assums that famNode->childFams is not null
     * @return number of dates added
     */
    int updateSiblings(GFTNode * famNode);

    /* Find references for upper (sibA) and lower (sibB)
     * siblings in childFams with non-null birth years
     */
    void findSiblingRefs(int & sibA, int & sibB, const QList<GFTNode *> & childFams);

    /* Estimate birth dates for all siblings in childFams between sibA and sibB
     * @return number of dates added
     */
    int estimateSiblingsBetween(int & sibA, int & sibB, const QList<GFTNode *> & childFams);

    /* Estimate birth dates for all siblings in childFams below sibA
     * @return number of dates added
     */
    int estimateSiblingsDown(int & sibA, const QList<GFTNode *> & childFams);

    /* Estimate birth dates for all siblings in childFams above sibB
     * @return number of dates added
     */
    int estimateSiblingsUp(int & sibB, const QList<GFTNode *> & childFams);

    //--- Paired References ---//

    /* Recursively estimate birth dates for family heads found between two reference
     * families with dates in the tree, and update individals' other dates along the way
     * @return number of dates added
     */
    int updateBranchPairs(GFTNode * n, GFTNode * famA = 0,  bool passedIncomplete = false);

    /* Estimate birth dates for all heads of families between famA and famB
     * @return number of dates added
     */
    int estimateBranchBetween(GFTNode * famA, GFTNode * famB);

    //--- Single References ---//

    /* Recursively estimate birth dates for family heads found above a
     * reference family with a date in the tree, projecting 25 years per generation
     * @return number of dates added
     */
    int updateBranchUpProjection(GFTNode * n, bool incompleteRoot = false);

    /* Recursively estimate marriage dates for couples
     * whose eldest child has a blank birth date.
     * @return number of dates added
     */
    int updateMarriages(GFTNode * n);

    /* Estimate birth dates for this node from parent
     * Assumes that this node is an eldest child
     * @return number of dates added
     */
    int estimateBranchDown(GFTNode * n);

    /* Estimate birth dates for parent of famB
     * Assumes that this node has a parent
     * @return number of dates added
     */
    int estimateBranchUp(GFTNode * famB);

};

#endif
