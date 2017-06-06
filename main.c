#define DEBUG 1

#include <stdio.h>
#include <stdlib.h>

/*1 <= estTime <= 100*/
typedef struct _process {
    int id, startTime, estTime, ioSize;
    int* io;
} Process;

typedef struct _event {
    int id, type, eventTime;
} Event;

typedef struct _queue {
    int maxListSize, eventsInList, start;
    Event* eventList;
} Queue;

Process processConstructor(int, int, int, int, int*);
Event eventConstructor(int, int, int);
Queue queueConstructor(int);
int compareEvents(const void*, const void*);
void sortEventList(Queue*);

int main() {
    int numberOfProcesses;
    int currentTime = 0;
    Process* list;
    Queue events;
    //Queue waitingProcesses;

    scanf("%d", &numberOfProcesses);
    list = (Process*) malloc(numberOfProcesses * sizeof(Process));

    /*READING THE INPUT*/
    for (int i = 0; i < numberOfProcesses; i++) {
        int newId;
        int newStartTime;
        int newEstTime;
        int ioInterruptions;
        int* newIo;

        scanf("%d %d %d %d", &newId, &newStartTime, &newEstTime, &ioInterruptions);
        newIo = (int*) malloc(ioInterruptions * sizeof(int));

        for (int j = 0; j < ioInterruptions; j++) {
            int newIoTime;

            scanf("%d", &newIoTime);
            newIo[j] = newIoTime;
        }

        list[i] = processConstructor(newId, newStartTime, newEstTime, ioInterruptions, newIo);
    }

    /*CHECKING IF IT WAS ALL READ FINE*/
    /*if (DEBUG) {
        printf("Listing all processes:\n");
        for (int i = 0; i < numberOfProcesses; i++) {
            printf("Process number %d:\n", list[i].id);
            printf("Start Time: %d\n", list[i].startTime);
            printf("Duration: %d\n", list[i].estTime);
            for (int j = 0; j < list[i].ioSize; j++) {
                printf("IO interruption at time %d.\n", list[i].io[j]);
            }
        }
        printf("\n");
    }*/

    /*CREATING THE QUEUE*/
    events = queueConstructor(numberOfProcesses);
    for (int i = 0; i < numberOfProcesses; i++) {
        events.eventList[i] = eventConstructor(list[i].id, 0, list[i].startTime);
        events.eventsInList++;
    }

    sortEventList(&events);

    /*CHECKING IF EVENT QUEUE WAS CORRECTLY CREATED*/
    /*if (DEBUG) {
        printf("Listing all events in queue:\n");
        for (int i = 0; i < events.start + events.eventsInList; i++) {
            printf("Process number %d:\n", events.eventList[i].id);
            switch(events.eventList[i].type) {
                case 0:
                    printf("Type: Arrival\n");
                    break;
                default:
                    printf("Something\'s not quite right...\n");
            }
            printf("Will happen at time %d.\n", events.eventList[i].eventTime);
        }
        printf("\n");
    }*/

    while (events.eventsInList != events.start) {
        printf("Time %d:\n", currentTime);

        /*CHECK WHICH EVENT WILL HAPPEN NOW*/
        /*MORE THAN 1 EVENT CAN OCCUR AT ONCE, MUST BE A WHILE LOOP*/
        while (events.eventList[events.start].eventTime == currentTime) {
            switch (events.eventList[events.start].type) {
                case 0:
                    printf("Process %d entered the waiting queue.\n", events.eventList[events.start].id);
                    break;
                default:
                    printf("Something\'s not quite right...\n");
            }
            events.start++;
        }

        /*PRINT CURRENT PROCESS*/
        /*PRINT WAITING PROCESSES*/
        /*PRINT PROCESSES ON I/O INTERRUPTION*/

        currentTime++;
    }
    
    return 0;
}

Process processConstructor(int _id, int _startTime, int _estTime, int _ioSize, int* _io) {
    int i;
    int size = sizeof(_io) / sizeof(int);
    Process temp;

    temp.id = _id;
    temp.startTime = _startTime;
    temp.estTime = _estTime;
    temp.ioSize = _ioSize;
    temp.io = (int*) malloc(sizeof(_io));
    for (i = 0; i < size; i++) {
        temp.io[i] = _io[i];
    }

    return temp;
}

Event eventConstructor(int _id, int _type, int _eventTime) {
    Event temp;

    temp.id = _id;
    temp.type = _type;
    temp.eventTime = _eventTime;

    return temp;
}

Queue queueConstructor(int _size) {
    Queue temp;

    temp.eventsInList = 0;
    temp.start = 0;

    temp.maxListSize = _size;
    temp.eventList = (Event*) malloc(sizeof(Event) * _size);

    return temp;
}

int compareEvents(const void *e1, const void *e2) {
    Event event1 = *((Event*) e1);
    Event event2 = *((Event*) e2);

    return (event1.eventTime - event2.eventTime);
}

void sortEventList(Queue *events) {
    qsort(events->eventList, events->eventsInList, sizeof(Event), compareEvents);
}