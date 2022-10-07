
#include <iostream>
#include "btreewrapper.h"
#include "record.h"
#include "source.h"


int main() {
    BTreeWrapper btreeWrapper;
    Packet p;
    fillPacket(&p);
    sort(p.records.begin(), p.records.end());
    
    for (Record r: p.records) {
        std::cout << "Keys? " << r.key << std::endl;
    }
    btreeWrapper.addPacket(p);
    for (Record r: p.records) {
        std::cout << "Success? " << btreeWrapper.read(r.key) << std::endl;
    }
    #if SIMPLE == 1
    for (auto it = btreeWrapper.btree.begin(); it != btreeWrapper.btree.end(); ++it) {
        std::cout << "Keys? " << (*it).key << std::endl;
    }
    #else
    for (auto it = btreeWrapper.btrees[0].begin(); it != btreeWrapper.btrees[0].end(); ++it) {
        std::cout << "Keys? " << (*it).key << std::endl;
    }
    #endif

}