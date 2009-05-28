#ifndef V_GDATE_ESTIMATOR_H
#define V_GDATE_ESTIMATOR_H

#include "GFamilyTree.h"

/* GDateEstimator: Utility class for estimating missing
 * dates in a group of GFamilyTrees created from the
 * data in a GEDCOM file. Estimates Birth, Marriage and
 * Death dates based on the methods set forth in
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

    /* Estimates missing dates for Births, Marriages
     * and Deaths and fills in the estimated values
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

    /* Estimates birth, marriage, and death dates for an
     * individual if relavent data is available and needed.
     * famNode->complete set true if no more dates are needed
     * @return number of dates added to this node
     */
    int updateIndividual(GFTNode * famNode);

    //--- Siblings ---//

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

    //--- Branches ---//

    GFTNode * findFamilyA(GFTNode * n);

    GFTNode * findFamilyB(GFTNode * famA, GFTNode * n);

};

#endif
