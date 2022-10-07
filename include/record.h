
#pragma once

#include <iostream>
#include <vector>

#define N 1
#define P 1024 // TOUCH
typedef struct Record {
    uint32_t key;
    uint32_t payload[N];

    bool operator < (const Record& r) const
    {
        return (key < r.key);
    }
} Record; 

typedef struct Packet {
    std::vector<Record> records;
} Packet;

typedef struct ReadCommand {
    uint32_t key;
} ReadCommand;

