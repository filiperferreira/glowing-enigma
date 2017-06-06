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

typedef struct _eventVector {
    int maxListSize, eventsInList, start;
    Event* eventList;
} EventVector;

typedef struct _processVector {
    int maxListSize, processesInList, start;
    Process* processList;
} ProcessVector;

Process processConstructor(int, int, int, int, int*);
Event eventConstructor(int, int, int);
EventVector eventVectorConstructor(int);
ProcessVector processVectorConstructor(int);
int compareEvents(const void*, const void*);
int compareProcesses(const void*, const void*);
void sortEventList(EventVector*);
void sortProcessList(ProcessVector*);
Process findProcessById(ProcessVector, int);
void pushToProcessVector(ProcessVector*, Process);
void pushToEventVector(EventVector*, Event);
Process popFromProcessVector(ProcessVector*);

int main() {
    int numberOfProcesses;
    int currentTime = 0;
    int currentProcess = -1;
    EventVector events;
    ProcessVector list;
    ProcessVector waiting;

    scanf("%d", &numberOfProcesses);
    list = processVectorConstructor(numberOfProcesses);

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

        pushToProcessVector(&list, processConstructor(newId, newStartTime, newEstTime, ioInterruptions, newIo));
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
    events = eventVectorConstructor(numberOfProcesses);
    for (int i = 0; i < numberOfProcesses; i++) {
        pushToEventVector(&events, eventConstructor(list.processList[i].id, 0, list.processList[i].startTime));
    }
    sortEventList(&events);

    /*CREATING THE WAITING QUEUE*/
    waiting = processVectorConstructor(numberOfProcesses);

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
        int i;
        printf("\nTIME %d:\n", currentTime);

        /*CHECK WHICH EVENT WILL HAPPEN NOW*/
        /*MORE THAN 1 EVENT CAN OCCUR AT ONCE, MUST BE A WHILE LOOP*/
        printf("\nEVENTS:\n");
        while (events.eventList[events.start].eventTime == currentTime) {
            switch (events.eventList[events.start].type) {
                case 0:                
                    printf("Process %d wants to enter the waiting queue.\n", events.eventList[events.start].id);
                    Process toEnter = findProcessById(list, events.eventList[events.start].id);
                    pushToProcessVector(&waiting, toEnter);
                    printf("Process %d entered the waiting queue.\n", events.eventList[events.start].id);
                    break;
                default:
                    printf("Something\'s not quite right...\n");
            }
            events.start++;
        }

        if (currentProcess == -1 && waiting.start < waiting.processesInList) {
            sortProcessList(&waiting);
            Process nextProcess = popFromProcessVector(&waiting);
            currentProcess = nextProcess.id;

            printf("Process %d has begun execution.\n", currentProcess);

            /*NEED TO CREATE EVENT OF PROCESS EXIT*/
        }

        /*PRINT CURRENT PROCESS*/
        printf("\nCURRENT PROCESS:\n");
        if (currentProcess != -1) {
            printf("Currently running process %d.\n", currentProcess);
        }
        else {
            printf("No process currently running.\n");
        }

        /*PRINT WAITING PROCESSES*/
        printf("\nPROCESSES WAITING:\n");
        for (i = waiting.start; i < waiting.processesInList; i++) {
            printf("%d ", waiting.processList[i].id);
        }
        printf("\n");

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

EventVector eventVectorConstructor(int _size) {
    EventVector temp;

    temp.eventsInList = 0;
    temp.start = 0;

    temp.maxListSize = _size;
    temp.eventList = (Event*) malloc(sizeof(Event) * _size);

    return temp;
}

ProcessVector processVectorConstructor(int _size) {
    ProcessVector temp;

    temp.processesInList = 0;
    temp.start = 0;

    temp.maxListSize = _size;
    temp.processList = (Process*) malloc(sizeof(Process) * _size);

    return temp;
}

int compareEvents(const void *e1, const void *e2) {
    Event event1 = *((Event*) e1);
    Event event2 = *((Event*) e2);

    return (event1.eventTime - event2.eventTime);
}

int compareProcesses(const void *p1, const void *p2) {
    Process process1 = *((Process*) p1);
    Process process2 = *((Process*) p2);

    return (process1.estTime - process2.estTime);
}

void sortEventList(EventVector *events) {
    qsort(events->eventList, events->eventsInList, sizeof(Event), compareEvents);
}

void sortProcessList(ProcessVector *processes) {
    qsort(processes->processList, processes->processesInList, sizeof(Process), compareProcesses);
}

Process findProcessById(ProcessVector list, int processId) {
    int i;
    for (i = list.start; i < list.processesInList; i++) {
        if (list.processList[i].id == processId) return list.processList[i];
    }
    return list.processList[list.start];
}

void pushToProcessVector(ProcessVector *vector, Process process) {
    if (vector->processesInList == vector->maxListSize) {
        vector->processList = (Process*) realloc(vector->processList, 2 * vector->maxListSize * sizeof(Process));
        vector->maxListSize *= 2;
    }
    vector->processList[vector->processesInList] = process;
    vector->processesInList++;
}

void pushToEventVector(EventVector *vector, Event event) {
    if (vector->eventsInList == vector->maxListSize) {
        vector->eventList = (Event*) realloc(vector->eventList, 2 * vector->maxListSize * sizeof(Event));
        vector->maxListSize *= 2;
    }
    vector->eventList[vector->eventsInList] = event;
    vector->eventsInList++;
}

Process popFromProcessVector(ProcessVector *vector) {
    Process temp = vector->processList[vector->start];
    vector->start++;
    return temp;
}