#include <cstddef>
#include <iostream>


class LogThreat{
    struct logNode {
        int logId = -1;
        int logSize = -1;
        struct logNode *next = nullptr;
    };

    struct threatNode {
        int threatId = -1;
        struct threatNode *next = nullptr;
        struct logNode    *log  = nullptr;
    };


    public:
        //struct threatNode *listOfThreats = new threatNode(0);
        struct threatNode *root;
        struct threatNode *end;
        struct threatNode *tail;

        LogThreat();
        ~LogThreat();   //załkowite pozamiatanie po sobie


        /** Wywoływane zawsze przy tworzeniu nowego wątku
            zwraca wskaźnik na ostatni obiekt logów.
            Kolejne logi dodawane są na końcu aktualnego
        */
        logNode* addThreat(int threatId);

        /** Zapisywanie logów */
        void saveLogs(int threatId);

        /** Wywoływane zawsze przy niszczeniu wątku */
        void deleteThreat(int threatId);

        /** lastLog:    dotychczasowy wskaźnik na łańcuch logów,
                        który posiada wątek;
            return:     zwraca nowy wskaźnik na łańcuch logów
        */
        logNode* addLog(logNode* lastLog, int logId, int logSize);

    private:

        threatNode* searchPrev(int threatId);
        void deleteLog(logNode* log);

};

