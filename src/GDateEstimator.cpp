
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
    const int PROJECTION = 4;
    int updateStatus;
    int totalUpdated = 0, newUpdates;
    GFamilyTree * t;
    do {
        // Clear all flags
        updateStatus = NONE;
        do {
            // Clear SIBLING and PAIR flags
            updateStatus &= PROJECTION;
            do {
                // Clear SIBLING flag
                updateStatus &= PROJECTION | PAIR;
                // Siblings / Individuals
                foreach (t, _trees) {
                    newUpdates = updateChildren(t->root());
                    if (newUpdates > 0) {
                        totalUpdated += newUpdates;
                        updateStatus = SIBLING | PAIR | PROJECTION;
                    }
                }
            } while ((updateStatus & SIBLING) > NONE);
            // Branch Pairs
            foreach (t, _trees) {
                newUpdates = updateBranchPairs(t->root());
                if (newUpdates > 0) {
                    totalUpdated += newUpdates;
                    updateStatus = PAIR | PROJECTION;
                }
            }
        } while ((updateStatus & PAIR) > NONE);
        // Projecting Branches
        foreach (t, _trees) {
            newUpdates = updateBranchProjection(t->root());
            if (newUpdates > 0) {
                totalUpdated += newUpdates;
                updateStatus = PROJECTION;
            }
        }
    } while ((updateStatus & PROJECTION) > NONE);
    return totalUpdated;
}

//=== Private Helper Methods ===//

    //----------------------//
    //---   Individals   ---//
    //----------------------//

/* Estimates birth, marriage, and death dates for a
 * couple if relavent data is available and needed.
 * famNode->complete is set true if no more dates are needed
 * @return number of dates added
 */
int GDateEstimator::updateCouple(GFTNode * famNode) {
    // Todo: Single-parent families with no marriage date will automatically
    // have a marriage date appened anyway... Is there a way to fix that?
    int updated = 0; // Tells the caller how many updates were made
    // Only check incomplete nodes
    if (!famNode->headComplete) {
        // Get relavent values from the famNode
        GIndiEntry * head = famNode->famHead;
        GIndiEntry * spouse = famNode->spouse;
        GFamily * fam = famNode->thisFam;
        QDate marriageYear = fam ? fam->marriageYear() : QDate();
        // Estimate marriage year if null
        if (fam && marriageYear.isNull()) {
            updated += updateMarriage(fam, head, spouse);
        }
        // Update individuals' data
        updated += updateIndividual(head, fam, spouse);
        if (spouse) updated += updateIndividual(spouse, fam, head);
        // Check if this family is complete or not
        // (if the head's birth year is set then so is everything else)
        if (head->birthYear().isValid()) {
            famNode->headComplete = true;
        }
    }
    return updated;
}

/* Estimates marriage date for a
 * couple if relavent data is available
 * @return number of dates added
 */
int GDateEstimator::updateMarriage(GFamily * fam, GIndiEntry * head, GIndiEntry * spouse) {
    // Tells the caller how many updates were made
    int updated = 0;
    QDate marriageYear;
    // Check head for valid birth year
    GIndiEntry * indi = head;
    QDate birthYear = head->birthYear();
    // Otherwise check spouse for valid birth year
    if (!birthYear.isValid() && spouse) {
        indi = spouse;
        birthYear = indi->birthYear();
    }
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
        fam->setMarriageYear(marriageYear);
        // Tell the caller that updates have been made
        ++updated;
    }
    return updated;
}

/* Estimates birth and death dates for an
 * individual if relavent data is available and needed
 * @return number of dates added to this node
 */
int GDateEstimator::updateIndividual(GIndiEntry * indi, GFamily * fam, GIndiEntry * spouse) {
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
            // Single parent or first wife
            if (!spouse || spouse->familyParent() == indi->familyParent()) {
                birthYear = marriageYear.addYears(-20);
            }
            // Second, third, ...
            else {
                const QStringList * marriages = spouse->marriages();
                if (!marriages) throw QString("Null marriages list");
                // Each wife is 5 years younger than the last
                int yearsYounger = marriages->indexOf(indi->familyParent()) * 5;
                birthYear = marriageYear.addYears(yearsYounger-20);
            }
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
    if (n->childFams) {
        // Update siblings
        if (!n->kidsComplete) {
            updated += updateSiblings(n);
        }
        // Recursively update children
        QList<GFTNode *> & childFams = *(n->childFams);
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
        // Update individual so that headComplete is set
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
        // Update individual so that headComplete is set
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
        // Update individual so that headComplete is set
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
        if (n->headComplete) {
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
    else if (!n->headComplete) {
        passedIncomplete = true;
    }
    // Base case:
    // Only continue if this family has children
    if (n->childFams) {
        // Recursively check/update all children
        QList<GFTNode *> & childFams = *(n->childFams);
        GFTNode * m;
        foreach (m, childFams) {
            // Check every round to see if this has become
            // a complete node and can be used as FamilyA
            if (n != famA && n->headComplete) {
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
    int sibOffset;
    // Fill in the gaps
    famB = famB->parentFam;
    while (famB != famA) {
        yearGap += avgGap;
        sibOffset = famB->parentFam->childFams->indexOf(famB) * 2;
        famB->famHead->setBirthYear(startYear.addYears((int)yearGap+sibOffset));
        famB = famB->parentFam;
        ++updated;
    }
    return updated;
}

    //-----------------------------//
    //---   Single References   ---//
    //-----------------------------//

/* Recursively estimate birth dates for family heads found above or below a
 * reference family with a date in the tree, projecting 25 years per generation
 * @return number of dates added
 */
int GDateEstimator::updateBranchProjection(GFTNode * n, bool incompleteRoot) {
    int updated = 0;
    // Found blank nodes below FamilyA
    if (!incompleteRoot && n->parentFam && !n->headComplete) {
        // Only project upward from eldest child
        if (n->eldestSibling() == n) {
            updated += estimateBranchDown(n);
        }
    }
    else {
        // Found a blank root
        if (!n->headComplete) {
            incompleteRoot = true;
        }
        // Found FamilyB below blank root
        else if (incompleteRoot) {
            // Only project upward from eldest child
            if (n->eldestSibling() == n) {
                // Project upward from this node
                updated += estimateBranchUp(n);
            }
            // This is not an empty node
            incompleteRoot = false;
        }
        // Base case:
        // Only continue if this family has children
        // and it wasn't a blank branch below FamilyA
        if (n->childFams) {
            // Recursively check/update all children
            QList<GFTNode *> & childFams = *(n->childFams);
            GFTNode * m;
            foreach (m, childFams) {
                updated += updateBranchProjection(m, incompleteRoot);
            }
        }
    }
    return updated;
}

/* Estimate birth dates for this node from parent
 * Assumes that this node is an eldest child
 * @return number of dates added
 */
int GDateEstimator::estimateBranchDown(GFTNode * n) {
    if (n->headComplete) throw QString("Found complete node below FamilyA on projection");
    int updated = 0;
    GFamily * marriageFam = n->parentFam->thisFam;
    // Get year from parent
    QDate birthYear = marriageFam->marriageYear();
    // 1 year between marriage and first child
    birthYear.addYears(1);
    // Set eldest child's birth year
    n->famHead->setBirthYear(birthYear);
    ++updated;
    return updated;
}

/* Estimate birth dates for parent of famB
 * Assumes that this node has a parent
 * @return number of dates added
 */
int GDateEstimator::estimateBranchUp(GFTNode * famB) {
    int updated = 0;
    GFTNode * n = famB->parentFam;
    if (n->headComplete) throw QString("Found complete node above FamilyB on projection");
    GFamily * marriageFam = n->thisFam;
    // Get year from eldest child
    QDate marriageYear = famB->famHead->birthYear();
    // 1 year between marriage and first child
    marriageYear.addYears(-1);
    // Set parent's birth year
    marriageFam->setMarriageYear(marriageYear);
    ++updated;
    return updated;
}
