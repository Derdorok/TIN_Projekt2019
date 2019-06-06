#include <iostream>
#include <fstream>
#include <string>
#include "LogThreat.h"
using namespace std;


LogThreat::LogThreat(){
    root = new threatNode;
    end  = new threatNode;
    root->next = end;
    tail = root;

    savedLogsFromClientRoot = new threatNode;
    savedLogsFromClientEnd  = new threatNode;
    savedLogsFromClientRoot->next = savedLogsFromClientEnd;
    savedLogsFromClientTail = savedLogsFromClientRoot;

    savedLogsFromServerRoot = new threatNode;
    savedLogsFromServerEnd  = new threatNode;
    savedLogsFromServerRoot->next = savedLogsFromServerEnd;
    savedLogsFromServerTail = savedLogsFromServerRoot;

    cout<<"utworzono obiekt klasy"<<endl;
}

LogThreat::~LogThreat(){
    while(root->next!=end){
        deleteThreat(root->next->threatId);
    }
    delete root;
    delete end;

    while(savedLogsFromClientRoot->next!=savedLogsFromClientEnd){
        deleteSavedThreat(savedLogsFromClientRoot, savedLogsFromClientRoot->next->threatId);
    }
    delete savedLogsFromClientRoot;
    delete savedLogsFromClientEnd;

    while(savedLogsFromServerRoot->next!=savedLogsFromServerEnd){
        deleteSavedThreat(savedLogsFromServerRoot, savedLogsFromServerRoot->next->threatId);
    }
    delete savedLogsFromServerRoot;
    delete savedLogsFromServerEnd;
    cout<<"zniszczono obiekt klasy"<<endl;
}

//======================PUBLIC===============================

LogThreat::logNode *LogThreat::addThreat(int threatId, int port){
    threatNode*  newThreat = new threatNode;
    newThreat->threatId = threatId;
    newThreat->log = new logNode;
    newThreat->log->logId = 0;
    newThreat->log->port = port;
    newThreat->port=port;
    newThreat->log->threatId = threatId;

    newThreat->next = end;
    tail->next = newThreat;
    tail = newThreat;

    cout<<"ROOT:     Dodano wątek: "<<threatId<<endl;

    return newThreat->log;
}

void LogThreat::deleteThreat(int threatId){
    threatNode* pointer = searchPrev(root, threatId);
    if(pointer!=nullptr){
        if(pointer->next == tail){
        tail=pointer;
        }
        threatNode* del = pointer->next;
        pointer->next = del->next;
        del->next = nullptr;

        deleteLog(del->log);
        delete del;
        cout<<"ROOT:     Usunięto wątek: "<<threatId<<endl;
        return;
    }

    cout<<"ROOT:     NIE Usunięto wątku: "<<threatId<<endl;
}

LogThreat::logNode *LogThreat::addLog(logNode* lastLog, int logSize, int type){
    logNode* newLog = new logNode;
    newLog->logId = (lastLog->logId)+1;
    newLog->threatId = lastLog->threatId;
    newLog->logSize = logSize;
    if(type==1) {
        newLog->arriveTime = time(0);
        newLog->type = 1;
    }
    else {
        newLog->sendTime = time(0);
        newLog->type = 2;
    }
    newLog->port = lastLog->port;

    lastLog->next = newLog;
    cout<<"     ROOT:     Dodano Log: "<<newLog->logId<<" do wątku: "<<newLog->threatId<<endl;
    return newLog;
}

void LogThreat::saveServerLogs(int threatId){
    threatNode* pointer = searchPrev(root, threatId);
    if(pointer!=nullptr){pointer = pointer->next;}
    else return;
    threatNode* savePointer = searchPrev(savedLogsFromClientRoot, threatId);

    if(savePointer == nullptr){
        savePointer = new threatNode;
        savePointer->threatId = threatId;
        savePointer->log = new logNode;
        savePointer->log->logId = 0;
        savePointer->log->threatId = threatId;
        savePointer->logtail = savePointer->log;
        savePointer->next = savedLogsFromClientEnd;

        cout<<"SAVEROOT: Dodano wątek: "<<threatId<<endl;

        savedLogsFromClientTail->next = savePointer;
        savedLogsFromClientTail = savePointer;
    }
    else{savePointer = savePointer->next;}
    savePointer->logtail->next = pointer->log->next;

    logNode* pom;
    for(pom=pointer->log->next; pom->next->next!=nullptr; pom=pom->next){}
    savePointer->logtail = pom;

    pointer->log->next = pom->next;
    pom->next=nullptr;

    cout<<"     SAVEROOT: Dodano logi: "<<endl;
}

void LogThreat::saveClientLogs(int threatId, int port, int sizeLog, int type){

    threatNode* savePointer = searchPrev(savedLogsFromServerRoot, threatId);

    if(savePointer == nullptr){
        savePointer = new threatNode;
        savePointer->threatId = threatId;
        savePointer->log = new logNode;
        savePointer->log->logId = 0;
        savePointer->log->threatId = threatId;
        savePointer->logtail = savePointer->log;
        savePointer->next = savedLogsFromServerEnd;

        cout<<"SAVEROOT: Dodano wątek: "<<threatId<<endl;

        savedLogsFromServerTail->next = savePointer;
        savedLogsFromServerTail = savePointer;
    }
    else{savePointer = savePointer->next;}


    logNode* newLog = new logNode;
    newLog->threatId = threatId;
    newLog->logSize = sizeLog;
    newLog->logId = (savePointer->logtail->logId)+1;
    if(type==1) {
        newLog->arriveTime = 0;
        newLog->type = 1;
    }
    else {
        newLog->sendTime = 0;
        newLog->type = 2;
    }
    newLog->port = port;

    savePointer->logtail->next = newLog;
    savePointer->logtail = newLog;


    cout<<"     SAVEROOT: Dodano logi Klienta: "<<endl;

}

void LogThreat::printStructure(threatNode* start){
    cout<<"===========KONIEC==========="<<endl;
    if(start->next->next!=nullptr) printThreat(start->next);
    cout<<"==========POCZĄTEK=========="<<endl;
}

void LogThreat::saveLogsToFile(){

    cout<<"zapis do pliku"<<endl;

    for(threatNode * pointer = savedLogsFromClientRoot->next; pointer->next!=nullptr; pointer=pointer->next){
        fstream plikSerwer;
        string plik = "Dane_serwera/watek_";
        plik+= to_string(pointer->threatId);
        plik+=".txt";
        cout<<"zapis do: "<<plik<<endl;


        plikSerwer.open(plik, ios::out | ios::app);
        logNode* logPointer=pointer->log->next;
        if(logPointer!=pointer->logtail){
            for(logPointer; logPointer->next!=pointer->logtail; logPointer=logPointer->next){
                cout<<"Zapis logu: "<<logPointer->logId<<endl;
                if(logPointer->type==1)plikSerwer<<"Nr. "<<logPointer->logId<<"    Port: "<<logPointer->port<<"       Czas Przybycia: "<<logPointer->arriveTime<<"            Rozmiar: "<<logPointer->logSize<<endl;
                else plikSerwer<<"Nr. "<<logPointer->logId<<"    Port: "<<logPointer->port<<"        Czas Wysłania: "<<logPointer->sendTime<<"            Rozmiar: "<<logPointer->logSize<<endl;
            }
            if(logPointer->type==1)plikSerwer<<"Nr. "<<logPointer->logId<<"    Port: "<<logPointer->port<<"       Czas Przybycia: "<<logPointer->arriveTime<<"            Rozmiar: "<<logPointer->logSize<<endl;
            else plikSerwer<<"Nr. "<<logPointer->logId<<"    Port: "<<logPointer->port<<"        Czas Wysłania: "<<logPointer->sendTime<<"            Rozmiar: "<<logPointer->logSize<<endl;

            cout<<"Zapis logu: "<<logPointer->logId<<endl;
            cout<<" logTail "<<pointer->logtail->logId<<endl;
            logPointer->next=nullptr;
            deleteLog(pointer->log->next);
            pointer->log->next=pointer->logtail;
        }

        plikSerwer.close();
    }

    for(threatNode * pointer = savedLogsFromServerRoot->next; pointer->next!=nullptr; pointer=pointer->next){
        fstream plikSerwer;
        string plik = "Dane_klientow/watek_";
        plik+= to_string(pointer->threatId);
        plik+=".txt";
        cout<<"zapis do: "<<plik<<endl;


        plikSerwer.open(plik, ios::out | ios::app);
        logNode* logPointer=pointer->log->next;
        if(logPointer!=pointer->logtail){
            for(logPointer; logPointer->next!=pointer->logtail; logPointer=logPointer->next){
                cout<<"Zapis logu: "<<logPointer->logId<<endl;
                if(logPointer->type==1)plikSerwer<<"Nr. "<<logPointer->logId<<"    Port: "<<logPointer->port<<"       Czas Przybycia: "<<logPointer->arriveTime<<"            Rozmiar: "<<logPointer->logSize<<endl;
                else plikSerwer<<"Nr. "<<logPointer->logId<<"    Port: "<<logPointer->port<<"        Czas Wysłania: "<<logPointer->sendTime<<"            Rozmiar: "<<logPointer->logSize<<endl;
            }
            if(logPointer->type==1)plikSerwer<<"Nr. "<<logPointer->logId<<"    Port: "<<logPointer->port<<"       Czas Przybycia: "<<logPointer->arriveTime<<"            Rozmiar: "<<logPointer->logSize<<endl;
            else plikSerwer<<"Nr. "<<logPointer->logId<<"    Port: "<<logPointer->port<<"        Czas Wysłania: "<<logPointer->sendTime<<"            Rozmiar: "<<logPointer->logSize<<endl;

            cout<<"Zapis logu: "<<logPointer->logId<<endl;
            cout<<" logTail "<<pointer->logtail->logId<<endl;
            logPointer->next=nullptr;
            deleteLog(pointer->log->next);
            pointer->log->next=pointer->logtail;
        }

        plikSerwer.close();
    }
}
//======================PRIVATE===============================
LogThreat::threatNode *LogThreat::searchPrev(threatNode* start, int threatId){
    for(threatNode* pointer = start; pointer->next->next!=nullptr; pointer=pointer->next){
        if(pointer->next->threatId == threatId)
            return pointer;
    }
    return nullptr;
}

void LogThreat::deleteSavedThreat(threatNode* start, int threatId){
    threatNode* pointer = searchPrev(start, threatId);
    if(pointer!=nullptr){
        threatNode* del = pointer->next;
        pointer->next = del->next;

        deleteLog(del->log);
        delete del;
        cout<<"ROOT:     Usunięto wątek: "<<threatId<<endl;
        return;
    }

    cout<<"ROOT:     NIE Usunięto wątku: "<<threatId<<endl;
}

void LogThreat::deleteLog(logNode* log){
    if(log->next != nullptr)
        deleteLog(log->next);

    cout<<"ROOT:     Usunięto log: "<<log->logId<<"    wątku: "<<log->threatId<<endl;
    delete log;
}

void LogThreat::printThreat(threatNode* start){
    if(start->next->next != nullptr)
        printThreat(start->next);
    printLog(start->log);
    cout<<"     THREAT: "<<start->threatId<<endl;

}

void LogThreat::printLog(logNode* log){
    if(log->next != nullptr)
        printLog(log->next);
    cout<<"         LOG: "<<log->logId<<endl;
}
