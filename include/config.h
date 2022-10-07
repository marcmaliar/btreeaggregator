#pragma once

#include <chrono>
#include "record.h"
#include "readerwriterqueue.h"
#include "btreewrapper.h"
using namespace moodycamel;

typedef std::chrono::high_resolution_clock Clock;
inline long long diffInNs(std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end) {
    return (std::chrono::duration_cast<std::chrono::nanoseconds>(end-start)).count();
}

enum RunType { timeLimit };

typedef struct Config {
    int packetQueueCapacity;
    int commandQueueCapacity;
    int idQueueCapacity;
    int nsInterval;
    
    RunType runtype;
    int nsLimit;
} Config;

typedef struct SourceConfig {
    int nsInterval;

    RunType runtype;
    int nsLimit;

    ReaderWriterQueue<Packet>* packetQueue;
    ReaderWriterQueue<Packet>* idQueue;
    std::chrono::steady_clock::time_point start;
} SourceConfig;

typedef struct WriteServerConfig {
    RunType runtype;
    int nsLimit;

    ReaderWriterQueue<Packet>* packetQueue;
    std::chrono::steady_clock::time_point start;
    BTreeWrapper* btreewrapper;
    
    int packetsProcessed;
} WriteServerConfig;

typedef struct ReadServerConfig {
    RunType runtype;
    int nsLimit;

    ReaderWriterQueue<ReadCommand>* commandQueue;
    std::chrono::steady_clock::time_point start;
    BTreeWrapper* btreewrapper;

    int commandsProcessed;
} ReadServerConfig;

typedef struct ClientConfig {
    RunType runtype;
    int nsLimit;

    ReaderWriterQueue<ReadCommand>* commandQueue;
    ReaderWriterQueue<Packet>* idQueue;
    std::chrono::steady_clock::time_point start;
} ClientConfig;
