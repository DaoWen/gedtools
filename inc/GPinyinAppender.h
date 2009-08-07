#ifndef V_G_PINYIN_APPENDER_H
#define V_G_PINYIN_APPENDER_H

#include "GIndiMap.h"
#include "PinyinMap.h"

/* GPinyinAppender: Adds pinyin data to all entries
 * in a GIndiMap based on the data contained in the
 * PinyinMap.dat file (accessed through PinyinMap).
 */
class GPinyinAppender {

public:

    //=== Constructor/Destructor ===//

    /* Constructor */
    GPinyinAppender();

    /* Destructor */
    ~GPinyinAppender();

    //=== Methods ===//

    /* Appends pinyin data to the specified
     * GIndiMap's individual entries
     * @return incomplete ("?") entry count
     */
    int appendTo(GIndiMap & indiMap);

private:

    //=== Constants ===//

    // First unicode point for Chinese characters
    static const int CJK_CODEPOINT;

    //=== Private Data Members ===//

    // Map containing pinyin data from PinyinMap.dat
    PinyinMap * _pinyinMap;

};

#endif
