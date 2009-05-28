
#include "GDateEstimator.h"

//=== Constructor/Destructor ===//

/* Constructor */
GDateEstimator::GDateEstimator(GFTList & trees, const QString & defaultPlace)
 : _trees(trees), _defaultPlace(defaultPlace), _currentYear(QDate::currentDate()) {}

/* Destructor */
GDateEstimator::~GDateEstimator() {}

//=== Mutators ===//

/* Estimates missing dates for Births, Marriages
 * and Deaths and fills in the estimated values
 * (along with optional locations) in the GNodes
 * Returns the number of dates appeneded
 */
int GDateEstimator::estimateMissingDates() {
    return 0;
}

//=== Private Helper Methods ===//

    //----------------------//
    //---   Individals   ---//
    //----------------------//

/* Estimates birth, marriage, and death dates for an
 * individual if relavent data is available and needed.
 * famNode->complete is set true if no more dates are needed
 * @return number of dates added to this node
 */
int GDateEstimator::updateIndividual(GFTNode * famNode) {
    // Tells the caller how many updates were made
    int updated = 0;
    // Get relavent values from the famNode
    GIndiEntry * indi = famNode->famHead;
    GFamily * fam = famNode->thisFam;
    QDate birthYear = indi->birthYear();
    QDate marriageYear = fam ? fam->marriageYear() : QDate();
    // Estimate marriage year if null
    if (fam && marriageYear.isNull()) {
        GIndiEntry * spouse = famNode->famHead;
        QDate spouseBirthYear = spouse->birthYear();
        // If this individual has a valid birth year
        if (birthYear.isValid()) {
            // Man's marriage year = birth year + 24
            if (indi->sex() == GIndiEntry::MALE) {
                marriageYear = birthYear.addYears(24);
            }
            // Woman's marriage year = birth year + 20
            else {
                marriageYear = birthYear.addYears(20);
            }
            fam->setMarriageYear(birthYear, _defaultPlace);
            // Tell the caller that updates have been made
            ++updated;
        }
        // If this individual's spouse has a valid birth year
        else if (spouseBirthYear.isValid()) {
            // Man's marriage year = birth year + 24
            if (spouse->sex() == GIndiEntry::MALE) {
                marriageYear = spouseBirthYear.addYears(24);
            }
            // Woman's marriage year = birth year + 20
            else {
                marriageYear = spouseBirthYear.addYears(20);
            }
            fam->setMarriageYear(spouseBirthYear, _defaultPlace);
            // Tell the caller that updates have been made
            ++updated;
        }
    }
    // Estimate birth year if null
    if (birthYear.isNull() && marriageYear.isValid()) {
        // Man's birth year = marriage year - 24
        if (indi->sex() == GIndiEntry::MALE) {
            birthYear = marriageYear.addYears(-24);
        }
        // Woman's birth year = marriage year - 20
        else {
            birthYear = marriageYear.addYears(-20);
        }
        indi->setBirthYear(birthYear, _defaultPlace);
        // Tell the caller that updates have been made
        ++updated;
    }
    // Calculate whether or not the individual is dead
    if (indi->deathDate().isNull() && birthYear.isValid()) {
        // Over 110 years old => they're dead
        if (birthYear.addYears(-110).year() > 0) {
            indi->setDeceased(_defaultPlace);
            // Tell the caller that updates have been made
            ++updated;
        }
    }
    // Check if this individual is complete or not
    // (if the birth year is set then so is everything else)
    if (!birthYear.isNull()) {
        famNode->headComplete = true;
    }
    return updated;
}

    //--------------------//
    //---   Siblings   ---//
    //--------------------//

/* Estimates birth dates for siblings in a family
 * Assums that famNode->childFams is not null
 * @return number of dates added
 */
int GDateEstimator::updateSiblings(GFTNode * famNode) {
    // Tells the caller how many updates were made
    int updated = 0;
    // Variables for the loop below
    int sibA, sibB;
    QList<GFTNode *> & childFams = *(famNode->childFams);
    for (;;) { // Loop until this hits one of the two break statements at bottom
        // Find upper & lower sibling references
        findSiblingRefs(sibA, sibB, childFams);
        // Estimate dates using found references
        if (sibB > -1) {
            // Upper & lower siblings were found
            if (sibA > -1) {
                updated += estimateSiblingsBetween(sibA, sibB, childFams);
            }
            // Only lower sibling was found
            else {
                updated += estimateSiblingsUp(sibB, childFams);
            }
        }
        // Only upper sibling was found
        else if (sibA > -1) {
            // Break the upper sibling is the last sibling in the list
            // (that means all siblings are complete)
            if (sibA == childFams.size()-1) {
                famNode->kidsComplete = true;
                break;
            }
            // Otherwise estimate birth dates for siblings below sibA
            updated += estimateSiblingsDown(sibA, childFams);
        }
        // Break if no references were found
        else break;
    }
    return updated;
}

/* Find references for upper (sibA) and lower (sibB)
 * siblings in childFams with non-null birth years
 */
void GDateEstimator::findSiblingRefs(int & sibA, int & sibB, const QList<GFTNode *> & childFams) {
    // Reset loop variables
    sibA = -1;
    sibB = -1;
    bool passedIncomplete = false;
    // Find upper & lower siblings with birth dates
    for (int i=0; i<childFams.size(); ++i) {
        // Only find a lower sibling if an incomplete was passed
        if (passedIncomplete) {
            // Find a lower sibling with a reference birth date
            if (childFams[i]->headComplete) {
                sibB = i;
                // Lower reference has been found so stop looping
                break;
            }
        }
        // If we've already passed an incomplete node then
        // we can't find a new upper sibling
        else {
            // Find an upper sibling with a reference birth date
            if (childFams[i]->headComplete) {
                sibA = i;
            }
            // Otherwise this is incomplete
            else {
                passedIncomplete = true;
            }
        }
    }
}

/* Estimate birth dates for all siblings in childFams between sibA and sibB
 * @return number of dates added
 */
int GDateEstimator::estimateSiblingsBetween(int & sibA, int & sibB, const QList<GFTNode *> & childFams) {
    int updated = 0;
    QDate startDate = childFams[sibA]->famHead->birthYear();
    // Average sibA & sibB's birth dates to find the gap in bewtween
    double avgGap =
      (double)(startDate.year() - childFams[sibB]->famHead->birthYear().year()) / (sibB - sibA - 1);
    // Total year gap between sibA and childFams[i]
    double birthGap = 0;
    // Estimate birth years for all siblings between sibA & sibB
    for (int i=sibA; i<sibB; ++i) {
        birthGap += avgGap;
        childFams[i]->famHead->setBirthYear(startDate.addYears((int)birthGap), _defaultPlace);
        ++updated;
    }
    return updated;
}

/* Estimate birth dates for all siblings in childFams below sibA
 * @return number of dates added
 */
int GDateEstimator::estimateSiblingsDown(int & sibA, const QList<GFTNode *> & childFams) {
    int updated = 0;
    QDate birthDate = childFams[sibA]->famHead->birthYear();
    // Loop back to the end (if there was another
    // complete node then sibB would have been set)
    for (int i=sibA; i<childFams.size(); ++i) {
        // 2 years between sibling births
        birthDate = birthDate.addYears(2);
        childFams[i]->famHead->setBirthYear(birthDate, _defaultPlace);
        ++updated;
    }
    return updated;
}

/* Estimate birth dates for all siblings in childFams above sibB
 * @return number of dates added
 */
int GDateEstimator::estimateSiblingsUp(int & sibB, const QList<GFTNode *> & childFams) {
    int updated = 0;
    QDate birthDate = childFams[sibB]->famHead->birthYear();
    // Loop back to the start (if there was another
    // complete node then sibA would have been set)
    for (int i=sibB; i>-1; --i) {
        // 2 years between sibling births
        birthDate = birthDate.addYears(-2);
        childFams[i]->famHead->setBirthYear(birthDate, _defaultPlace);
        ++updated;
    }
    return updated;
}

    //--------------------//
    //---   Branches   ---//
    //--------------------//

int GDateEstimator::updateBranchPairs(GFTNode * n) {
    return 0;
}

int GDateEstimator::findFamilyA(GFTNode * n) {
    int updated = 0;
    // This node is only interesting if it has children under it
    if (n && n->childFams) {
        QList<GFTNode *> & childFams = *(n->childFams);
        GFTNode * m;
        // Found FamilyA, so find FamilyB
        if (n->headComplete) {
            foreach (m, childFams) {
                findFamilyB(n, m);
            }
        }
        // Keep searching for FamilyA
        else {
            foreach (m, childFams) {
                findFamilyA(m);
            }
        }
    }
    return updated;
}

int GDateEstimator::findFamilyB(GFTNode * famA, GFTNode * n) {
    int updated = 0;
    if (n) {
        // Found FamilyB, so fill in the gaps
        if (n->headComplete) {

        }
        // Continue searching downward for FamilyB
        else if (n->childFams) {

        }
    }
    return updated;
}

int GDateEstimator::estimateBranchBetween(GFTNode * famA, GFTNode * famB) {
    QDate x;
    QDate startYear = famB->famHead->birthYear();
    double yearGap = 0;
    // Division by 0?
    double avgGap = (famA->famHead->birthYear().year() - birthYear) / (famB->level - famA->level - 1);
    famB = famB->parentFam;
    while (famB != famA) {

    }
}

//int GDateEstimator::estimateBranchDown() {}

//int GDateEstimator::estimateBranchUp() {}
