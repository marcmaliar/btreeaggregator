
#include "config.h"
#include "readserver.h"

void readServer(ReadServerConfig* p_readServerConfig) {

    ReadServerConfig readServerConfig = *p_readServerConfig;

    //p_readServerConfig->start = Clock::now();

    if (readServerConfig.runtype == timeLimit) {
        
        int commands = 0;
        ReadCommand rc;
        while (diffInNs(readServerConfig.start, Clock::now()) < readServerConfig.nsLimit) {
            if (readServerConfig.commandQueue->try_dequeue(rc)) { 
                if (readServerConfig.btreewrapper->read(rc.key)) commands++;
                //continue; 
            }
            //else std::this_thread::yield();
        }
        p_readServerConfig->commandsProcessed = commands;
    }
    else {
        throw std::domain_error("Invalid runtype");
    }   
}