
#include "config.h"
#include "writeserver.h"
#include <vector>



void client(ClientConfig* p_clientConfig) {

    ClientConfig clientConfig = *p_clientConfig;

    //p_clientConfig->start = Clock::now();

    std::vector<uint32_t> ids;
    Packet p;
    ReadCommand rc;

    if (clientConfig.runtype == timeLimit) {

        while (!clientConfig.idQueue->try_dequeue(p)) {}
        for (int i = 0; i < 2; ++i) {
            ids.push_back(p.records[i].key);
        }

        while (diffInNs(clientConfig.start, Clock::now()) < clientConfig.nsLimit) {
            if (clientConfig.idQueue->try_dequeue(p)) {
                for (int i = 0; i < 4; ++i) {
                    ids.push_back(p.records[i].key);
                }
            }
            
            auto i = rand() % ids.size(); // not _really_ random
            rc = {.key = ids[i]};
            clientConfig.commandQueue->try_enqueue(rc);
            //else std::this_thread::yield();
        }
        
    }
    else {
        throw std::domain_error("Invalid runtype");
    }   
}