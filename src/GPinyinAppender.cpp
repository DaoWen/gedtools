
#include "GPinyinAppender.h"

//=== Constants ===//

// First unicode point for Chinese characters
const int GPinyinAppender::CJK_CODEPOINT = 0x3400;

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
int GPinyinAppender::appendTo(GIndiMap & indiMap) {
    // Define loop variables
    QString name; // Chinese name of individual
    QString romanName; // Romanized name of individual
    QString pinyin; // Hanyu pinyin for an individual character in the name
    bool needSpace; // Don't append spaces between hanzi
    bool hasHanzi; // If there are no hanzi then we don't need a romanized name
    bool incomplete; // If this romanization has a "?" then it's incomplete
    int incompleteCount = 0; // Total number of incomplete ("?") entries
    GIndiMap::Iterator i, end = indiMap.end();
    // Loop through every GIndiEntry in the GIndiMap
    for (i=indiMap.begin();i!=end;++i) {
        // Reset variables for this name entry
        name = i.value()->name();
        romanName = "";
        hasHanzi = false;
        needSpace = false;
        incomplete = false;
        // Loop through each character in their name to append Pinyin
        for (int j=0;j<name.length();++j) {
            // If it's a Hanzi then try to append pinyin for it
            if (name[j] >= CJK_CODEPOINT) {
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
        if (hasHanzi) i.value()->setRomanizedName(romanName);
        // If this entry was incomplete then increment the count
        if (incomplete) ++incompleteCount;
    }
    return incompleteCount;
}
