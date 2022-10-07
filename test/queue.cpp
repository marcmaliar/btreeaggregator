#include <iostream>
#include "config.h"
#include "record.h"
#include "readerwriterqueue.h"
#include "source.h"
#include "writeserver.h"
#include "readserver.h"
#include "client.h"
#include <barrier>
#include "btreewrapper.h"

using namespace moodycamel;

#define REPEAT 3

typedef struct Result {
    int packetsProcessed;
    int recordsWritten;
    int commandsProcessed;
} Result;

Result average(Result* results) {
    int packetsProcessed = 0;
    int recordsWritten = 0;
    int commandsProcessed = 0;
    for (int i = 0; i < REPEAT; ++i){
        packetsProcessed += results[i].packetsProcessed;
        recordsWritten += results[i].recordsWritten;
        commandsProcessed += results[i].commandsProcessed;
    }
    Result r;
    r.packetsProcessed = packetsProcessed / REPEAT;
    r.recordsWritten = recordsWritten / REPEAT;
    r.commandsProcessed = commandsProcessed / REPEAT;
    return r;
}

WriteServerConfig writeServerConfig;
ReadServerConfig readServerConfig;
SourceConfig sourceConfig;
ClientConfig clientConfig;

Result run(Config config) {

    Result r;

    ReaderWriterQueue<Packet> packetQueue(config.packetQueueCapacity);
    ReaderWriterQueue<ReadCommand> commandQueue(config.commandQueueCapacity);
    ReaderWriterQueue<Packet> idQueue(config.idQueueCapacity);
    BTreeWrapper* btreeWrapper = new BTreeWrapper;

    auto t0 = Clock::now();

    writeServerConfig = (WriteServerConfig) { 
        
        .runtype = config.runtype,
        .nsLimit = config.nsLimit, 
        .packetQueue = &packetQueue, 
        .btreewrapper = btreeWrapper,
        .start = t0,
        .packetsProcessed = 0,
    };
    readServerConfig = (ReadServerConfig) { 
        
        .runtype = config.runtype,
        .nsLimit = config.nsLimit, 
        .commandQueue = &commandQueue,
        .btreewrapper = btreeWrapper,
        .start = t0,
        .commandsProcessed = 0,
    };
    sourceConfig = (SourceConfig) { 
        .nsInterval = config.nsInterval, 

        .runtype = config.runtype,
        .nsLimit = config.nsLimit, 

        .packetQueue = &packetQueue,
        .idQueue = &idQueue,
        .start = t0,
    };

    clientConfig = (ClientConfig) {
        .runtype = config.runtype,
        
        .nsLimit = config.nsLimit, 
        .commandQueue = &commandQueue,
        .idQueue = &idQueue,
        .start = t0,
    };
    std::thread writeServerThread(writeServer, &writeServerConfig);
    std::thread readServerThread(readServer, &readServerConfig);
    std::thread sourceThread(source, &sourceConfig);
    std::thread clientThread(client, &clientConfig);
    
    writeServerThread.join();
    readServerThread.join();
    sourceThread.join();
    clientThread.join();

    r.packetsProcessed = writeServerConfig.packetsProcessed;
    r.recordsWritten = writeServerConfig.packetsProcessed*P;
    r.commandsProcessed = readServerConfig.commandsProcessed;
    return r;
}

Result runAverage(Config config) {
    Result results[REPEAT];
    for (int i = 0; i < REPEAT; ++i) {
        results[i] = run((Config) config);
    }
    return average(results);
}

int main() {
    std::cout << "Basic queue experiment" << std::endl;

    std::vector<int> nsIntervals = {500000, 1000000, 5000000, 10000000, 50000000, 100000000, 500000000};
    for (int nsInterval: nsIntervals) {
        Config config = {
            .packetQueueCapacity = 1024,
            .commandQueueCapacity = 1024,
            .idQueueCapacity = 1024,
            .nsInterval = nsInterval,
            .runtype = timeLimit,
            .nsLimit = 1*1000000000,
        };
        
        Result r = runAverage(config);

        int line;
        int b;
        #if USE_BTREE == 0
        line = 0;
        b = -1;
        #elif SIMPLE == 1
        line = 1;
        b = -1;
        #else
        line = 2;
        b = B;
        #endif


        
        //std::cout << "Average packets processed: " << r.packetsProcessed << std::endl;
        //std::cout << "Average records written: " << r.recordsWritten << std::endl;
        //std::cout << "Average commands processed: " << r.commandsProcessed << std::endl;
        std::cout << "(" << line << ", " << b << ", " << config.nsInterval << ", " << P << ", " << r.commandsProcessed << ")," << std::endl;
    }

    return 0;
}