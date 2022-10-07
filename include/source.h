
#pragma once

#include <iostream>
#include <thread>
#include "config.h"

extern std::thread sourceThread;

uint32_t nextID();
void fillRecord(Record* r);
void fillPacket(Packet* packet);
void source(SourceConfig* p_sourceConfig);  /* An example function declaration */

