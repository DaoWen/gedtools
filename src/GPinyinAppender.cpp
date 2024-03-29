
#include "GPinyinAppender.h"

//=== Constants ===//

// First unicode point for Chinese characters
// Note that PinyinMap.dat does not contain
// any characters before this codepoint.
const int GPinyinAppender::CJK_CODEPOINT_A = 0x3400;

// Last unicode point for Chinese characters
// Note that PinyinMap.dat does not contain
// any characters after this codepoint.
const int GPinyinAppender::CJK_CODEPOINT_Z = 0x9FFF;

//=== Constructor/Destructor ===//

/* Constructor */
GPinyinAppender::GPinyinAppender() {
    _pinyinMap = new PinyinMap();
}

/* Destructor */
GPinyinAppender::~GPinyinAppender() {
    delete _pinyinMap;
}

//=== Methods ===//

/* Appends pinyin data to the specified
 * GIndiMap's individual entries
 * @return incomplete ("?") entry count
 */
int GPinyinAppender::appendTo(GIndiMap & indiMap, bool replaceOldEntries) {
    // Define loop variables
    QString name; // Chinese name of individual
    QString romanName; // Romanized name of individual
    QString pinyin; // Hanyu pinyin for an individual character in the name
    bool needSpace; // Don't append spaces between hanzi
    bool hasHanzi; // If there are no hanzi then we don't need a romanized name
    bool incomplete; // If this romanization has a "?" then it's incomplete
    int incompleteCount = 0; // Total number of incomplete ("?") entries
    GIndiEntry * individual;
    GIndiMap::Iterator i, end = indiMap.end();
    // Loop through every GIndiEntry in the GIndiMap
    for (i=indiMap.begin();i!=end;++i) {
        individual = i.value();
        // Don't replace old pinyin if that option is specified
        if (!replaceOldEntries && !individual->romanizedName().isEmpty()) continue;
        // Reset variables for this name entry
        name = individual->name();
        romanName = "";
        hasHanzi = false;
        needSpace = false;
        incomplete = false;
        // Loop through each character in their name to append Pinyin
        for (int j=0;j<name.length();++j) {
            // If it's a Hanzi then try to append pinyin for it
            if (CJK_CODEPOINT_A <= name[j] && name[j] <= CJK_CODEPOINT_Z) {
                // Append space if needed
                if (needSpace) romanName.append(' ');
                hasHanzi = true;
                // Find the pinyin entry for this hanzi
                pinyin = _pinyinMap->lookup(name[j]);
                // If pinyin data is found then append it
                if (!pinyin.isNull()) {
                    romanName.append(pinyin);
                    needSpace = true;
                }
                // If there is no pinyin found then append "?"
                else {
                    romanName.append('?');
                    needSpace = true;
                    incomplete = true;
                }
            }
            // Otherwise append whatever the character is
            else {
                romanName.append(name[j]);
                needSpace = false;
            }
        }
        // Update the romanized name entry if needed
        if (hasHanzi) individual->setRomanizedName(romanName);
        // If this entry was incomplete then increment the count
        if (incomplete) ++incompleteCount;
    }
    return incompleteCount;
}
