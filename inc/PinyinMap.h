#ifndef V_PINYINMAP_H
#define V_PINYINMAP_H

#include <QList>
#include <QtCore>

/* PinyinMap: Mapping of a Chinese character to its pronounciation
 * Each Hanzi is mapped to its phonetic representation, written in
 * Hanyu Pinyin. The database is read from a file PinyinMap.dat,
 * which is a file in UTF-8 encoding containing thousands of lines
 * of the form "[Hanzi] [Pinyin]\n". The file is garaunteed to be
 * sorted according to the Unicode value of the Hanzi (low to high).
 * By convention the Pinyin values are given without tones. The
 * database was generated from the Unihan Project database:
 * http://unicode.org/charts/unihan.html
 */
class PinyinMap {
public:
    //=== Constructor/Destructor ===//

    /* Default Constructor
     * This object knows how to construct
     * itself, so there are no parameters.
     */
    PinyinMap();

    /* Default Destructor
     * Frees all the memory allocated
     * for the hanzi/pinyin map data
     */
    ~PinyinMap();

    //=== Accessors ===//

    /* Find the pinyin phonetic for this
     * Chinese character in the database
     * Returns a null string if not found
     */
    QString lookup(const QChar & hanzi) const;

protected:

    //=== Protected Data Members ===//

    // Structure for storing data pairs in pinyinData
    struct MapPair {
        MapPair(QChar & h, QString & p); // Constructor
        QChar hanzi;    // Hanzi (Chinese character)
        QString pinyin; // Pinyin phonetic representation
    };

private:

    //=== Private Helper Methods ===//

    /* Loads the data from PinyinMap.dat into _pinyinData
     * Returns false if there is a problem loading the data
     */
    bool loadData();

    /* Performs a binary search on _pinyinData to find
     * the entry corresponding to this hanzi. If the value
     * is not found then the method returns a null string.
     */
    MapPair * binarySearch(const QChar & hanzi) const;

    //=== Private Data Members ===//

    // Binary searchable array containing mappings of
    // Chinese characters to pinyin phonetic representations
    QList<MapPair*> & _pinyinData;

};

#endif
