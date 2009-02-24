#ifndef V_GINDI_MAP_H
#define V_GINDI_MAP_H

#include <QMap>
#include <QString>
#include "GNode.h"
#include "GIndiEntry.h"

typedef QMap<QString,GIndiEntry*> QGMap;

class GIndiMap : public QGMap {
public:

    //=== Destructor ===//

    /* Destructor
     * Frees all map entries
     */
    ~GIndiMap();

    //=== Mutators ===//

    /* Inserts a person into the map
     * Pass in the pointer to the INDI node
     * Returns false if this person already exists
     */
    bool insert(GNode * person);

};

#endif
