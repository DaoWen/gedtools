
#include "PinyinMap.h"

using namespace std;

//=== Constructor/Destructor ===//

/* Default Constructor
 * This object knows how to construct
 * itself, so there are no parameters.
 */
PinyinMap::PinyinMap() : _pinyinData(*new QList<MapPair*>){
    if (!loadData()) {
        throw QString("Error reading pinyin database.");
    }
}

/* Default Destructor
 * Frees all the memory allocated
 * for the hanzi/pinyin map data
 */
PinyinMap::~PinyinMap() {
    // Free all of the MapPair entries
    qDeleteAll(_pinyinData);
    // _pinyinData is a reference, not a pointer,
    // so we need to get the address to delete it
    delete &_pinyinData;
}

//=== Accessors ===//

/* Find the pinyin phonetic for this
 * Chinese character in the database
 * Returns a null string if not found
 */
QString PinyinMap::lookup(const QChar & hanzi) const {
    QString pinyin;
    MapPair * entry = binarySearch(hanzi);
    if (entry) {
        pinyin = entry->pinyin;
    }
    return pinyin;
}

//=== Private Helper Methods ===//

/* Loads the data from PinyinMap.dat into _pinyinData
 * Returns false if there is a problem loading the data
 */
bool PinyinMap::loadData() {
    // Open a stream to read data from PinyinMap.dat
    QFile inputFile("PinyinMap.dat");
    inputFile.open(QIODevice::ReadOnly);
    QTextStream input(&inputFile);
    input.setCodec("UTF-8");
    input.skipWhiteSpace(); // Skip any whitespace at the front
    // Read all the data from PinyinMap.dat into dataList
    // We need to read it to this temporary list first because
    // we don't know how many entries it contains ahead of time.
    QChar hanzi;
    QString pinyin;
    while (!input.atEnd()) {
        input >> hanzi >> pinyin;
        _pinyinData.append(new MapPair(hanzi,pinyin));
        // We could get a blank entry at the end of the file
        // if we don't skip the whitespace there.
        input.skipWhiteSpace();
    }
    inputFile.close();
    // If there are entries in _pinyinData then it probably worked
    return (_pinyinData.size() > 0);
}

/* Performs a binary search on _pinyinData to find
 * the entry corresponding to this hanzi. If the value
 * is not found then the method returns a null pointer.
 */
PinyinMap::MapPair * PinyinMap::binarySearch(const QChar & hanzi) const {
    MapPair * entry;
    int low = 0;
    int high = _pinyinData.size()-1;
    int mid;
    while (low <= high) {
        mid = (low + high) / 2;
        entry = _pinyinData[mid];
        if (hanzi == entry->hanzi) {
            break; // Found it!
        }
        if (hanzi < entry->hanzi) {
            high = mid - 1; // Look left
        }
        else {
            low = mid + 1; // Look right
        }
    }
    // Return a null pointer if no match was found
    if (low > high) {
        entry = 0;
    }
    return entry;
}

//=== Protected Data Members ===//

// Constructor for the MapChar struct
PinyinMap::MapPair::MapPair(QChar & h, QString & p) : hanzi(h), pinyin(p) {}
