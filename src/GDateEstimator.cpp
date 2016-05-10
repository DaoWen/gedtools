
#include "GDateEstimator.h"

//=== Constructor/Destructor ===//

/* Constructor */
GDateEstimator::GDateEstimator(GFTList & trees, const QString & defaultPlace)
 : _trees(trees), _defaultPlace(defaultPlace), _currentYear(QDate::currentDate()) {}

/* Destructor */
GDateEstimator::~GDateEstimator() {}

//=== Mutators ===//

/* Estimates missing dates for births, marriages
 * and deaths and fills in the estimated values
 * (along with optional locations) in the GNodes
 * Returns the number of dates appeneded
 */
int GDateEstimator::estimateMissingDates() {
    // Bit values for updateStatuses in estimateMissingDates
    const int NONE = 0;
    const int SIBLING = 1;
    const int PAIR = 2;
    const int UP_PROJECTION = 4;
    const int DOWN_PROJECTION = 8;
    int updateStatus;
    int totalUpdated = 0, newUpdates;
    GFamilyTree * t;
    do {
        // Clear all flags
        updateStatus = NONE;
        do {
            // Clear UP_PROJECTION, SIBLING and PAIR flags
            updateStatus &= DOWN_PROJECTION;
            do {
                // Clear SIBLING and PAIR flags
                updateStatus &= DOWN_PROJECTION | UP_PROJECTION;
                do {
                    // Clear SIBLING flag
                    updateStatus &= DOWN_PROJECTION | UP_PROJECTION | PAIR;
                    // Siblings / Individuals
                    foreach (t, _trees) {
                        newUpdates = updateChildren(t->root());
                        if (newUpdates > 0) {
                            totalUpdated += newUpdates;
                            updateStatus = DOWN_PROJECTION | SIBLING | PAIR | UP_PROJECTION;
                        }
                    }
                } while ((updateStatus & SIBLING) > NONE);
                // Branch Pairs
                foreach (t, _trees) {
                    newUpdates = updateBranchPairs(t->root());
                    if (newUpdates > 0) {
                        totalUpdated += newUpdates;
                        updateStatus = DOWN_PROJECTION | PAIR | UP_PROJECTION;
                    }
                }
            } while ((updateStatus & PAIR) > NONE);
            // Projecting Branches Upward
            foreach (t, _trees) {
                newUpdates = updateBranchUpProjection(t->root());
                if (newUpdates > 0) {
                    totalUpdated += newUpdates;
                    updateStatus = DOWN_PROJECTION | UP_PROJECTION;
                }
            }
        } while ((updateStatus & UP_PROJECTION) > NONE);
        // Projecting Branches Downward
        foreach (t, _trees) {
            newUpdates = updateMarriages(t->root());
            if (newUpdates > 0) {
                totalUpdated += newUpdates;
                updateStatus = DOWN_PROJECTION;
            }
        }
    } while ((updateStatus & DOWN_PROJECTION) > NONE);
    return totalUpdated;
}

//=== Private Helper Methods ===//

    //----------------------//
    //---   Individals   ---//
    //----------------------//

/* Estimates birth, marriage, and death dates for a
 * couple if relevent data is available and needed.
 * famNode->complete is set true if no more dates are needed
 * @return number of dates added
 */
int GDateEstimator::updateCouple(GFTNode * famNode) {
    // Todo: Single-parent families with no marriage date will automatically
    // have a marriage date appened anyway... Is there a way to fix that?
    int updated = 0; // Tells the caller how many updates were made
    // Families without children don't need to update children
    if (!famNode->kidsComplete) {
        if (!famNode->naturalChildFams || famNode->naturalChildFams->size() == 0) {
            famNode->kidsComplete = true;
        }
    }
    // Only check incomplete nodes with a head of family
    if (famNode->famHead && !famNode->headComplete) {
        // Get relevent values from the famNode
        GIndiEntry * head = famNode->famHead;
        GIndiEntry * spouse = famNode->spouse;
        GFamily * fam = famNode->thisFam;
        // Estimate marriage year if null
        //QDate marriageYear = fam ? fam->marriageYear() : QDate();
        // Only estimate once children's birthdates are done
        if (fam && (famNode->kidsComplete || famNode->naturalChildFams->at(0)->headComplete)) {
            updated += updateMarriage(famNode);
        }
        // Update individuals' data
        updated += updateIndividual(head, fam, spouse);
        if (spouse) updated += updateIndividual(spouse, fam, head);
        // Check if this family is complete or not
        // (if the marriage year is set then so is everything else,
        //  or if there is no family then the head's birth date)
        if ((fam && fam->marriageYear().isValid()) || (!fam && head->birthYear().isValid())) {
            famNode->headComplete = true;
        }
    }
    return updated;
}

/* Estimates marriage date for a
 * couple if relevent data is available
 * @return number of dates added
 */
int GDateEstimator::updateMarriage(GFTNode * famNode) {
    // Tells the caller how many updates were made
    int updated = 0;
    GFamily * fam = famNode->thisFam;
    // Only update the marriage isn't already set
    if (fam && fam->marriageDate().isEmpty()) {
        QDate marriageYear;
        GIndiEntry * eldestChild = 0;
        // Check eldest child
        if (famNode->naturalChildFams && famNode->naturalChildFams->size() > 0) {
            eldestChild = famNode->naturalChildFams->at(0)->famHead;
        }
        // Check head for valid birth year
        GIndiEntry * indi = famNode->famHead;
        QDate birthYear = indi->birthYear();
        // Otherwise check spouse for valid birth year
        if (!birthYear.isValid() && famNode->spouse) {
            indi = famNode->spouse;
            birthYear = indi->birthYear();
        }
        // Update marriage date from eldest child
        // Use the eldest child's birthdate if possible
        if (eldestChild && eldestChild->birthYear().isValid()) {
            // Marriage year is 1 year before the oldest child is born
            marriageYear = eldestChild->birthYear().addYears(-1);
            fam->setMarriageYear(marriageYear);
            // Tell the caller that updates have been made
            ++updated;
        }
        // Otherwise use the couple's birthdates
        // If this individual has a valid birth year
        else if (birthYear.isValid()) {
            // Calculate offset for 2nd, 3rd, ... marriages
            int offset = 0;
            const QStringList * marriages = indi->marriages();
            if (marriages) {
                // Each marriage is 5 years after the previous
                // Todo: Figure out a good way to estimate these gaps relative
                // to any that are already defined. Otherwise, we could have problems
                // where the first marriage is estimated at 30 years after the husband's
                // birth date because of generation averaging, but then the second marriage
                // is set at 29 years because of this equation...
                offset = 5 * marriages->indexOf(fam->id());
            }
            // Man's marriage year = birth year + 24
            if (indi->sex() == GIndiEntry::MALE) {
                marriageYear = birthYear.addYears(24+offset);
            }
            // Woman's marriage year = birth year + 20
            else {
                marriageYear = birthYear.addYears(20+offset);
            }
            fam->setMarriageYear(marriageYear);
            // Tell the caller that updates have been made
            ++updated;
        }
    }
    return updated;
}

/* Estimates birth and death dates for an
 * individual if relevent data is available and needed
 * @return number of dates added to this node
 */
int GDateEstimator::updateIndividual(GIndiEntry * indi, GFamily * fam, GIndiEntry * /*spouse*/) {
    // Tells the caller how many updates were made
    int updated = 0;
    QDate birthYear = indi->birthYear();
    QDate marriageYear = fam ? fam->marriageYear() : QDate();
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
        indi->setBirthYear(birthYear);
        // Tell the caller that updates have been made
        ++updated;
    }
    // Calculate whether or not the individual is dead
    if (indi->deathDate().isNull() && birthYear.isValid()) {
        // Over 110 years old => they're dead
        if (birthYear.addYears(110) < _currentYear) {
            indi->setDeceased();
            // Tell the caller that updates have been made
            ++updated;
        }
    }
    // Fill in any empty PLAC nodes if default place is set
    if (!_defaultPlace.isEmpty() && birthYear.isValid()) {
        if (indi->birthPlace().isEmpty() && !indi->birthDate().isEmpty()) {
            indi->setBirthPlace(_defaultPlace);
        }
        if (indi->deathPlace().isEmpty() && !indi->deathDate().isEmpty()) {
            indi->setDeathPlace(_defaultPlace);
        }
        if (fam && fam->marriagePlace().isEmpty() && !fam->marriageDate().isEmpty()) {
            fam->setMarriagePlace(_defaultPlace);
        }
    }
    return updated;
}

    //--------------------//
    //---   Siblings   ---//
    //--------------------//

/* Recurse through all children in the tree
 * and update them with respect to siblings
 * @return number of dates added
 */
int GDateEstimator::updateChildren(GFTNode * n) {
    int updated = 0;
    // Update this node
    updated += updateCouple(n);
    if (n->naturalChildFams) {
        // Update siblings
        if (!n->kidsComplete) {
            updated += updateSiblings(n);
        }
        // Recursively update children
        QList<GFTNode *> & childFams = *(n->naturalChildFams);
        GFTNode * m;
        foreach (m, childFams) {
            updated += updateChildren(m);
        }
    }
    return updated;
}

/* Estimates birth dates for siblings in a family
 * Assums that famNode->childFams is not null
 * @return number of dates added
 */
int GDateEstimator::updateSiblings(GFTNode * famNode) {
    // Tells the caller how many updates were made
    int updated = 0;
    // Variables for the loop below
    int sibA, sibB;
    QList<GFTNode *> & childFams = *(famNode->naturalChildFams);
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
            // Break if the upper sibling is the last sibling in the list
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
    bool childHasBirthDate;
    // Find upper & lower siblings with birth dates
    for (int i=0; i<childFams.size(); ++i) {
        // Find out if this child's birth date is set
        childHasBirthDate = childFams[i]->famHead->birthYear().isValid();
        // Only find a lower sibling if an incomplete was passed
        if (passedIncomplete) {
            // Find a lower sibling with a reference birth date
            if (childHasBirthDate) {
                sibB = i;
                // Lower reference has been found so stop looping
                break;
            }
        }
        // If we've already passed an incomplete node then
        // we can't find a new upper sibling
        else {
            // Find an upper sibling with a reference birth date
            if (childHasBirthDate) {
                sibA = i;
                // famA can't be the last child
                if (i == childFams.size()-1) {
                    sibA = -1;
                }
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
    // Account for no year 0
    int yearA = startDate.year();;
    int yearB = childFams[sibB]->famHead->birthYear().year();
    if (yearA < 0 && yearB > 0) yearA += 1;
    // Average sibA & sibB's birth dates to find the gap in bewtween
    double avgGap =
      (double)(yearB - yearA) / (sibB - sibA);
    // Total year gap between sibA and childFams[i]
    double birthGap = 0;
    // Estimate birth years for all siblings between sibA & sibB
    for (int i=sibA+1; i<sibB; ++i) {
        birthGap += avgGap;
        childFams[i]->famHead->setBirthYear(startDate.addYears((int)birthGap));
        ++updated;
        // Update individual so that all dates are updated
        updated += updateCouple(childFams[i]);
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
    for (int i=sibA+1; i<childFams.size(); ++i) {
        // 2 years between sibling births
        birthDate = birthDate.addYears(2);
        childFams[i]->famHead->setBirthYear(birthDate);
        ++updated;
        // Update individual so that all dates are updated
        updated += updateCouple(childFams[i]);
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
    for (int i=sibB-1; i>-1; --i) {
        // 2 years between sibling births
        birthDate = birthDate.addYears(-2);
        childFams[i]->famHead->setBirthYear(birthDate);
        ++updated;
        // Update individual so that all dates are updated
        updated += updateCouple(childFams[i]);
    }
    return updated;
}

    //-----------------------------//
    //---   Paired References   ---//
    //-----------------------------//

/* Recursively estimate birth dates for family heads found between two reference
 * families with dates in the tree, and update individals' other dates along the way
 * @return number of dates added
 */
int GDateEstimator::updateBranchPairs(GFTNode * n, GFTNode * famA, bool passedIncomplete) {
    int updated = 0;
    // Only find FamilyB if an incomplete was passed
    if (passedIncomplete) {
        // Find FamilyB with a reference birth date
        if (n->hasBirthDate()) {
            // If FamilyA was found earlier, then fill in
            // the gaps, but only if famB is the eldest child
            if (famA && n->eldestSibling() == n) updated += estimateBranchBetween(famA, n);
            // Make this node the new FamilyA
            famA = n;
            // Update all siblings of famA so that there
            // aren't conflicts with date averages
            if (n->parentFam) updated += updateSiblings(n->parentFam);
            passedIncomplete = false;
        }
    }
    // If this node is incomplete then we've
    // found a gap between FamilyA and FamilyB
    else if (!n->hasBirthDate()) {
        passedIncomplete = true;
    }
    // Base case:
    // Only continue if this family has children
    if (n->naturalChildFams) {
        // Recursively check/update all children
        QList<GFTNode *> & childFams = *(n->naturalChildFams);
        GFTNode * m;
        foreach (m, childFams) {
            // Check every round to see if this has become
            // a complete node and can be used as FamilyA
            if (n != famA && n->hasBirthDate()) {
                famA = n;
                passedIncomplete = false;
                // Update all siblings of famA so that there
                // aren't conflicts with date averages
                if (n->parentFam) updated += updateSiblings(n->parentFam);
            }
            updated += updateBranchPairs(m, famA, passedIncomplete);
        }
    }
    return updated;
}

/* Estimate birth dates for all heads of families between famA and famB
 * @return number of dates added
 */
int GDateEstimator::estimateBranchBetween(GFTNode * famA, GFTNode * famB) {
    int updated = 0;
    QDate startYear = famB->famHead->birthYear();
    double yearGap = 0;
    int levelGap = famB->level - famA->level;
    // Check for division by 0
    if (levelGap < 1) throw QString("FamilyA and FamilyB have no gap! (Division by 0)");
    // Account for no year 0
    int yearA = famA->famHead->birthYear().year();
    int yearB = startYear.year();
    if (yearA < 0 && yearB > 0) yearA += 1;
    // Calculate the average gap between individuals
    double avgGap = (double)(yearA - yearB) / levelGap;
    // Offset for younger children since the generation
    // gaps should be based on the eldest child
    // Todo: Make this work correctly by keeping track of sibling offsets
    // in the updateBranchPairs() method so that we can actually make
    // each generation gap (parents to eldest child) the same
    int sibOffset;
    // Fill in the gaps
    famB = famB->parentFam;
    while (famB != famA) {
        yearGap += avgGap;
        sibOffset = famB->parentFam->naturalChildFams->indexOf(famB) * 2;
        famB->famHead->setBirthYear(startYear.addYears((int)yearGap+sibOffset));
        famB = famB->parentFam;
        updated += updateSiblings(famB);
        ++updated;
    }
    return updated;
}

    //-----------------------------//
    //---   Single References   ---//
    //-----------------------------//

/* Recursively estimate birth dates for family heads found above a
 * reference family with a date in the tree, projecting 25 years per generation
 * @return number of dates added
 */
int GDateEstimator::updateBranchUpProjection(GFTNode * n, bool incompleteRoot) {
    int updated = 0;
    // Found blank nodes below FamilyA
    if (!incompleteRoot && n->parentFam && !n->hasBirthDate()) {
        // Only project downward to eldest child
        if (n->eldestSibling() == n) {
            updated += estimateBranchDown(n);
        }
    }
    else {
        // Found parentless family without marriage date
        if (!n->famHead && !n->thisFam->marriageYear().isValid()) {
            incompleteRoot = true;
        }
        // Found a blank root (family has parents)
        else if (n->famHead && !n->hasMarriageDate()) {
            incompleteRoot = true;
        }
        // Base case:
        // Only continue if this family has children
        // and it wasn't a blank branch below FamilyA
        if (n->naturalChildFams) {
            // First make sure that all dates are set (not just birthday)
            if (!n->headComplete) {
                //if (n->famHead) updated += updateMarriage(n);
                updated += updateCouple(n);
            }
            // Recursively check/update all children
            QList<GFTNode *> & childFams = *(n->naturalChildFams);
            GFTNode * m;
            foreach (m, childFams) {
                updated += updateBranchUpProjection(m, incompleteRoot);
            }
        }
    }
    return updated;
}

/* Recursively estimate marriage dates for couples
 * whose eldest child has a blank birth date.
 * @return number of dates added
 */
int GDateEstimator::updateMarriages(GFTNode * n) {
    int updated = 0;
    if (n->famHead && n->thisFam) updated += updateMarriage(n);
    if (n->naturalChildFams) {
        // Recursively check/update all children
        QList<GFTNode *> & childFams = *(n->naturalChildFams);
        GFTNode * m;
        foreach (m, childFams) {
            updated += updateMarriages(m);
        }
    }
    return updated;
}

/* Estimate birth dates for this node from parent
 * Assumes that this node is an eldest child
 * @return number of dates added
 */
int GDateEstimator::estimateBranchDown(GFTNode * n) {
    if (n->hasBirthDate()) throw QString("Found complete node below FamilyA on projection");
    int updated = 0;
    GFamily * marriageFam = n->parentFam->thisFam;
    // Get year from parent
    QDate birthYear = marriageFam->marriageYear();
    // 1 year between marriage and first child
    // Set eldest child's birth year
    n->famHead->setBirthYear(birthYear.addYears(1));
    ++updated;
    // Update individual so that all dates are updated
    updated += updateCouple(n);
    return updated;
}

/* Estimate birth dates for parent of famB
 * Assumes that this node has a parent
 * @return number of dates added
 */
int GDateEstimator::estimateBranchUp(GFTNode * famB) {
    int updated = 0;
    GFTNode * n = famB->parentFam;
    if (n->hasBirthDate()) throw QString("Found complete node above FamilyB on projection: ").append(n->famHead->id());
    GFamily * marriageFam = n->thisFam;
    // Get year from eldest child
    QDate marriageYear = famB->famHead->birthYear();
    // 1 year between marriage and first child
    // Set parent's birth year
    marriageFam->setMarriageYear(marriageYear.addYears(-1));
    ++updated;
    // Update individual so that all dates are updated
    updated += updateCouple(n);
    return updated;
}
