// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>


#include "LogThreat.h"


using namespace std;

int main() {
    LogThreat* system = new LogThreat();

    LogThreat::logNode* a;
    LogThreat::logNode* b;
    LogThreat::logNode* c;

    a = system->addThreat(1);
    b = system->addThreat(2);
    c = system->addThreat(3);

    a = system->addLog(a, 15);
    a = system->addLog(a, 12);

    b = system->addLog(b, 12);
    c = system->addLog(c, 12);
    a = system->addLog(a, 12);

    b = system->addLog(b, 12);
    c = system->addLog(c, 12);
    a = system->addLog(a, 12);

    b = system->addLog(b, 12);
    c = system->addLog(c, 12);
    a = system->addLog(a, 12);

    c = system->addLog(c, 12);
    a = system->addLog(a, 12);

    system->printStructure(system->root);
    system->saveServerLogs(3);
    system->saveServerLogs(1);
    system->saveServerLogs(2);

    system->printStructure(system->root);
    system->printStructure(system->savedLogsFromClientRoot);

    b = system->addLog(b, 12);
    c = system->addLog(c, 12);
    a = system->addLog(a, 12);

    b = system->addLog(b, 12);
    c = system->addLog(c, 12);
    a = system->addLog(a, 12);

    b = system->addLog(b, 12);
    c = system->addLog(c, 12);
    a = system->addLog(a, 12);

    system->printStructure(system->root);
    system->saveServerLogs(3);
    system->saveServerLogs(1);
    system->saveServerLogs(2);

    system->printStructure(system->root);
    system->printStructure(system->savedLogsFromClientRoot);

    return 0;
}
