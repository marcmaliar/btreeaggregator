
#include "config.h"
#include "writeserver.h"

void writeServer(WriteServerConfig* p_writeServerConfig) {

    WriteServerConfig writeServerConfig = *p_writeServerConfig;

    //p_writeServerConfig->start = Clock::now();

    if (writeServerConfig.runtype == timeLimit) {
        
        int packets = 0;
        Packet p;
        while (diffInNs(writeServerConfig.start, Clock::now()) < writeServerConfig.nsLimit) {
            //std::cout << "Write server checking the time" << std::endl;
            if (writeServerConfig.packetQueue->try_dequeue(p)) {
                writeServerConfig.btreewrapper->addPacket(p);
                packets++; continue; 
            }
            else std::this_thread::yield();
        }
        p_writeServerConfig->packetsProcessed = packets;
    }
    else {
        throw std::domain_error("Invalid runtype");
    }   
}