#include <iostream>
#include "LogThreat.h"

LogThreat::LogThreat(){
    root = new threatNode;
    end = new threatNode;
    root->next = end;
    tail = root;
}

LogThreat::~LogThreat(){
    while(root->next!=end){
        deleteThreat(root->next->threatId);
    }
    delete root;
    delete end;
}

//======================PUBLIC===============================

LogThreat::logNode *LogThreat::addThreat(int threatId){
    threatNode*  newThreat = new threatNode;
    newThreat->threatId = threatId;
    newThreat->log = new logNode;

    newThreat->next = end;
    tail->next = newThreat;

    return newThreat->log;
}

void LogThreat::deleteThreat(int threatId){
    threatNode* pointer = searchPrev(threatId);
    if(pointer!=nullptr){
        threatNode* del = pointer->next;
        pointer->next = del->next;

        deleteLog(del->log);
        delete del;
    }
}

LogThreat::logNode *LogThreat::addLog(logNode* lastLog, int logId, int logSize){
    logNode* newLog = new logNode;
    newLog->logId = logId;
    newLog->logSize = logSize;
    lastLog->next = newLog;
    return newLog;
}

void LogThreat::saveLogs(int threatId){


}

//======================PRIVATE===============================
LogThreat::threatNode *LogThreat::searchPrev(int threatId){
    for(threatNode* pointer = root; pointer->next->next!=nullptr; pointer=pointer->next){
        if(pointer->threatId == threatId)
            return pointer;
    }
    return nullptr;
}


void LogThreat::deleteLog(logNode* log){
    if(log->next != nullptr)
        deleteLog(log->next);
    delete log;
}
