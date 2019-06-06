#include <cstddef>
#include <iostream>
#include <fstream>
#include <ctime>

/*
	Od razu informuję że jest to tylko struktura danych. Nie tworzy ona żadnych
	nowych wątków, tylko oferuje metody które powinny być wykonywane przez inne wątki.

	Najpierw struktury:

	struct threatNode 	- 	to nas specjalnie nie powinno interesować;
	struct logNode		- 	struktura zawierająca dane Logu. to właśnie takie struktury będą zapisywane
							w systemie i dobrze by było, aby wątki wysyłały sobie właśnie takie struktury

	Podział metod na wątki:

	1.	Główny wątek (ten który tworzy nowe przy każdym połączeniu) wraz z tworzeniem
		nowego wątku powinien wykonać:

		            logNode* addThreat(int threatId);


						threatId = 	identyfikator, powinien być inny dla każdego wątku i każdy wątek powinien
									mieć w sobie pozycję określającą jego id;

						zwraca logNode* czyli wskaźnik na ostatni dodany Log. Każdy wątek powinien mieć pozycję
									logNode* actualLog;

	2. 	Każdy wątek powstały przy nawiązaniu nowego połączenia:

					logNode* addLog(logNode* lastLog, int logSize);

						lastLog	=	wskaźnik na ostatnio dodany Log

						logSize	=	jest to jedna z danych Logu. Jeżeli zdecydujemy aby przesyłać więcej informacji
									to będzie więcej argumentów.
									(to nie jest aktualnie zbyt eleganckie, gdyż zakłada że otrzymamy jakieś dane, a nie
									samą strukturę: logNode. W tym przypadku na podstawie argumentów ta metoda sama
									tworzy strukturę: logNode)

						zwraca logNode* czyli wskaźnik na ostatni dodany Log.

	3.	Każdy wątek serwera kiedy jest kończony powienien wykonać:

					void deleteThreat(int threatId);

	4.	Wątek archiwizacyjny co jakiś czas powinien wykonać:

					void saveServerLogs(int threatId);	=	Powoduje to zapis danych logów do specjlanej struktury,
															która nie jest już kasowana podczas kończenia wątku.

*/




class LogThreat{
    public:

    struct logNode {
        int logId = -1;
        int threatId = -1;
        int type = 0;

        time_t arriveTime = 0;  //kiedy przybył na wątek serwera
        time_t sendTime = 0;
        int logSize = -1;
        int port = 0;
        struct logNode *next = nullptr;
    };

    struct threatNode {
        int threatId = -1;
        int port = 0;
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
            logNode* addThreat(int threatId, int port);

		/** Wywoływane zawsze przy niszczeniu wątku */
            void deleteThreat(int threatId);

		/** wątek serwera wykonuje zawsze kiedy otrzyma
            kolejny komunikat. Wymagany jest wskaźnik na
            ostatni zarejestrowany Log, oraz jakieś tam
            dane które znajdują się w komunikacie Logu.
            Takie danych jak logId, threatId, arriveTime
            obliczane są automatycznie*/
            logNode* addLog(logNode* lastLog, int logSize, int type);


        /** Zapisywanie logów odbieranych przez wątki serwera.
			Co jakiś czas nasz specjalny wątek archiwizacji danych
			powinien wykonać tę metodę dla wszystkich wątków działających
			dla serwera*/
            void saveServerLogs(int threatId);

        /** Zapisywanie logów odbieranych przez wątki klientów*/
            void saveClientLogs(int threatId, int port, int sizeLog, int type);


            void saveLogsToFile();

        /** wyświetlenie powstałej struktury.
			Pomocne przy serwisowaniu*/
            void printStructure(threatNode* start);


    private:

        threatNode* searchPrev(threatNode* start, int threatId);
        void deleteSavedThreat(threatNode* start, int threatId);
        void deleteLog(logNode* log);

        void printThreat(threatNode* start);
        void printLog(logNode* log);

};

