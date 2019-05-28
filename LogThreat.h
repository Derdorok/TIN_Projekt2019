#include <cstddef>
#include <iostream>
#include <ctime>

class LogThreat{
    public:

    struct logNode {
        int logId = -1;
        int threatId = -1;

        time_t arriveTime = 0;  //kiedy przybył na wątek serwera
        int logSize = -1;
        struct logNode *next = nullptr;
    };

    struct threatNode {
        int threatId = -1;
        struct threatNode  *next = nullptr;
        struct logNode     *log  = nullptr;
        struct logNode *logtail  = nullptr;
    };


    public:
        //struct threatNode *listOfThreats = new threatNode(0);


        struct threatNode *root;
        struct threatNode *end;
        struct threatNode *tail;

        struct threatNode *savedLogsFromClientRoot;
        struct threatNode *savedLogsFromClientTail;
        struct threatNode *savedLogsFromClientEnd;

        struct threatNode *savedLogsFromServerRoot;
        struct threatNode *savedLogsFromServerTail;
        struct threatNode *savedLogsFromServerEnd;

        LogThreat();
        ~LogThreat();   //załkowite pozamiatanie po sobie


        /** Wywoływane zawsze przy tworzeniu nowego wątku
            zwraca wskaźnik na ostatni obiekt logów.
            Kolejne logi dodawane są na końcu aktualnego*/
            logNode* addThreat(int threatId);

        /** Zapisywanie logów odbieranych przez wątki serwera*/
            void saveServerLogs(int threatId);

        /** Zapisywanie logów odbieranych przez wątki klientów*/
            void saveClientLogs(int threatId);

        /** Wywoływane zawsze przy niszczeniu wątku */
            void deleteThreat(int threatId);


        /** wątek serwera wykonuje zawsze kiedy otrzyma
            kolejny komunikat. Wymagany jest wskaźnik na
            ostatni zarejestrowany Log, oraz jakieś tam
            dane które znajdują się w komunikacie Logu.
            Takie danych jak logId, threatId, arriveTime
            obliczane są automatycznie*/
            logNode* addLog(logNode* lastLog, int logSize);


        /** wyświetlenie powstałej struktury*/
            void printStructure(threatNode* start);


    private:

        threatNode* searchPrev(threatNode* start, int threatId);
        void deleteSavedThreat(threatNode* start, int threatId);
        void deleteLog(logNode* log);

        void printThreat(threatNode* start);
        void printLog(logNode* log);

};

