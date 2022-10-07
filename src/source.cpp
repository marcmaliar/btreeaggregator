
#include <random>
#include <iostream>
#include "source.h"
#include "config.h"
#include <vector>

// Function generateRecord
// malloc some bits
// malloc key
// return it

std::random_device rd;
std::mt19937 mt(rd());
std::uniform_int_distribution<int> dist(0, UINT32_MAX);

bool first = true;
bool second = false;
uint32_t left = -1;
uint32_t counter = 0;
uint32_t one = 1;

uint32_t nextID() {
    return dist(mt);
    /*if (first) {first = false; second = true; return 0; }
    if (second) {second = false; left = 30; return one << 31; }
    uint64_t id = (one << (left)) + (counter << (left+1));
    counter += 1;
    if (counter == (one << (31 - left))) {left -= 1; counter = 0;}
    if (left == -1) {first = true; } 
    return id;*/
}

void fillRecord(Record* r) {
    r->key = nextID();
    for (int i = 0; i < N; ++i) {
        (r->payload)[i] = dist(mt);
    }
}
void fillPacket(Packet* packet) {
    packet->records.clear();
    Record r;
    for (int i = 0; i < P; ++i) {
        fillRecord(&r);
        packet->records.push_back(r);
        
    }
}


// Create record
// Send it to queue
// Wait until queue has space

void source(SourceConfig* p_sourceConfig) {
    SourceConfig sourceConfig = *p_sourceConfig;

    //p_sourceConfig->start = Clock::now();

    int packetsProcessed = 0;
    Packet p;
    fillPacket(&p);

    if (sourceConfig.runtype == timeLimit) {
        
        fillPacket(&p);
        while (diffInNs(sourceConfig.start, Clock::now()) < sourceConfig.nsLimit) {
            if (diffInNs(sourceConfig.start, Clock::now())-packetsProcessed*sourceConfig.nsInterval > sourceConfig.nsInterval) { 
                
                while (!sourceConfig.packetQueue->try_enqueue(p));
                while (!sourceConfig.idQueue->try_enqueue(p));
                fillPacket(&p);
                packetsProcessed++;
            }
            //else { std::this_thread::yield(); }
        }
    }
    else {
        throw std::domain_error("Invalid runtype");
    }
}