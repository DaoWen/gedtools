
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
    // Bit values for updateStatuses in estimateMissingDates
    const int NONE = 0;
    const int PAIR = 1;
    const int SIBLING = 2;
    const int PROJECTION = 4;
    //
    bool updateStatus;
    int totalUpdated = 0, newUpdates;
    GFamilyTree * t;
    do {
        // Clear all flags
        updateStatus = NONE;
        do {
            // Clear PAIR and SIBLING flags
            updateStatus &= PROJECTION;
            do {
                // Clear PAIR flag
                updateStatus &= PROJECTION | SIBLING;
                // Branch Pairs / Individuals
                foreach (t, _trees) {
                    newUpdates = updateBranchPairs(t->root());
                    if (newUpdates > 0) {
                        totalUpdated += newUpdates;
                        updateStatus = PAIR | SIBLING | PROJECTION;
                    }
                }
            } while (updateStatus & PAIR > NONE);
            // Siblings
            foreach (t, _trees) {
                newUpdates = updateChildren(t->root());
                if (newUpdates > 0) {
                    totalUpdated += newUpdates;
                    updateStatus = SIBLING | PROJECTION;
                }
            }
        } while (updateStatus & SIBLING > NONE);
        // Projecting Branches
        foreach (t, _trees) {
            newUpdates = updateBranchProjection(t->root());
            if (newUpdates > 0) {
                totalUpdated += newUpdates;
                updateStatus = PROJECTION;
            }
        }
    } while (updateStatus & PROJECTION > NONE);
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
    // Tells the caller how many updates were made
    int updated = 0;
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
        updateIndividual(head, fam, spouse);
        if (spouse) updateIndividual(spouse, fam, head);
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
        birthYear = spouse->birthYear();
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
        fam->setMarriageYear(marriageYear, _defaultPlace);
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
        indi->setBirthYear(birthYear, _defaultPlace);
        // Tell the caller that updates have been made
        ++updated;
    }
    // Calculate whether or not the individual is dead
    if (indi->deathDate().isNull() && birthYear.isValid()) {
        // Over 110 years old => they're dead
        if (birthYear.addYears(110) < _currentYear) {
            indi->setDeceased(_defaultPlace);
            // Tell the caller that updates have been made
            ++updated;
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
    if (n->childFams) {
        // Update siblings
        updated += updateSiblings(n);
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
        childFams[i]->famHead->setBirthYear(startDate.addYears((int)birthGap), _defaultPlace);
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
        childFams[i]->famHead->setBirthYear(birthDate, _defaultPlace);
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
        childFams[i]->famHead->setBirthYear(birthDate, _defaultPlace);
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
    // Update this node
    updated += updateCouple(n);
    // Only find FamilyB if an incomplete was passed
    if (passedIncomplete) {
        // Find FamilyB with a reference birth date
        if (n->headComplete) {
            // If FamilyA was found then fill in the gaps
            if (famA) updated += estimateBranchBetween(famA, n);
            // Make this node the new FamilyA
            famA = n;
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
    // Fill in the gaps
    famB = famB->parentFam;
    while (famB != famA) {
        yearGap += avgGap;
        //famB->famHead->setBirthYear(startYear.addYears((int)yearGap), _defaultPlace);
        famB->famHead->setBirthYear(startYear.addYears((int)yearGap), QString("%1 %2").arg(yearGap).arg(avgGap));
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
        if (n->parentFam->childFams->at(0) == n) {
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
            if (n->parentFam->childFams->at(0) == n) {
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
    GIndiEntry * parent = n->parentFam->famHead;
    // Get year from parent
    QDate birthYear = parent->birthYear();
    // 25 years between father and first child
    // 21 years between mother and first child
    birthYear = parent->sex() == GIndiEntry::MALE ? birthYear.addYears(25) : birthYear.addYears(21);
    // Set eldest child's birth year
    n->famHead->setBirthYear(birthYear, _defaultPlace);
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
    GIndiEntry * parent = n->famHead;
    // Get year from eldest child
    QDate birthYear = famB->famHead->birthYear();
    // 25 years between father and first child
    // 21 years between mother and first child
    birthYear = parent->sex() == GIndiEntry::MALE ? birthYear.addYears(-25) : birthYear.addYears(-21);
    // Set parent's birth year
    parent->setBirthYear(birthYear, _defaultPlace);
    ++updated;
    return updated;
}
