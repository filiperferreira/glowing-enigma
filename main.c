#define IO_TIME 5

#include <stdio.h>
#include <stdlib.h>

typedef struct _ioVector {
    int maxListSize, ioInList, start;
    int* ioTimes;
} IOVector;

typedef struct _process {
    int id, startTime, estTime;
    IOVector io;
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
IOVector ioVectorConstructor(int);
int compareEvents(const void*, const void*);
int compareProcesses(const void*, const void*);
void sortEventList(EventVector*);
void sortProcessList(ProcessVector*);
Process findProcessById(ProcessVector, int);
void pushToProcessVector(ProcessVector*, Process);
void pushToEventVector(EventVector*, Event);
void pushToIOVector(IOVector*, int);
Process popFromProcessVector(ProcessVector*);
int popFromIOVector(IOVector*);

int main() {
    int numberOfProcesses;
    int currentTime = 0;
    Process currentProcess;
    EventVector events;
    ProcessVector list;
    ProcessVector waiting;
    ProcessVector ioWaiting;

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

    /*CREATING THE QUEUE*/
    events = eventVectorConstructor(numberOfProcesses);
    for (int i = 0; i < numberOfProcesses; i++) {
        Event newEvent = eventConstructor(list.processList[i].id, 0, list.processList[i].startTime);
        pushToEventVector(&events, newEvent);
    }
    sortEventList(&events);

    /*CREATING THE WAITING QUEUE*/
    waiting = processVectorConstructor(numberOfProcesses);
    ioWaiting = processVectorConstructor(numberOfProcesses);
    currentProcess.id = -1;

    while (events.eventsInList != events.start) {
        int i;
        printf("\nTIME %d:\n", currentTime);

        /*CHECK WHICH EVENT WILL HAPPEN NOW*/
        /*MORE THAN 1 EVENT CAN OCCUR AT ONCE, MUST BE A WHILE LOOP*/
        printf("\nEVENTS:\n");
        sortEventList(&events);
        while (events.eventList[events.start].eventTime == currentTime) {
            switch (events.eventList[events.start].type) {
                case 0:                
                    printf("Process %d entered the waiting queue.\n", events.eventList[events.start].id);
                    Process toEnter = findProcessById(list, events.eventList[events.start].id);
                    pushToProcessVector(&waiting, toEnter);
                    break;
                case 1:
                    printf("Process %d has finished execution.\n", currentProcess.id);
                    currentProcess.id = -1;
                    break;
                case 2:
                    printf("Process %d is waiting for an IO event to finish.\n", currentProcess.id);
                    pushToProcessVector(&ioWaiting, currentProcess);
                    Event newEvent = eventConstructor(currentProcess.id, 3, currentTime + IO_TIME);
                    pushToEventVector(&events, newEvent);
                    currentProcess.id = -1;
                    break;
                case 3:
                    printf("IO of process %d has finished. Process %d returned to waiting queue.\n", events.eventList[events.start].id, events.eventList[events.start].id);
                    Process returningProcess = popFromProcessVector(&ioWaiting);
                    pushToProcessVector(&waiting, returningProcess);
                    break;
                default:
                    printf("Something\'s not quite right...\n");
            }
            events.start++;
        }

        if (currentProcess.id == -1 && waiting.start < waiting.processesInList) {
            sortProcessList(&waiting);
            currentProcess = popFromProcessVector(&waiting);

            printf("Process %d has begun execution.\n", currentProcess.id);

            Event newEvent;
            if (currentProcess.io.start != currentProcess.io.ioInList) {
                int finishTime = popFromIOVector(&currentProcess.io);
                currentProcess.estTime -= finishTime;
                for (int i = currentProcess.io.start; i < currentProcess.io.ioInList; i++) {
                    currentProcess.io.ioTimes[i] -= finishTime;
                }
                newEvent = eventConstructor(currentProcess.id, 2, currentTime + finishTime);
            }
            else {
                int finishTime = currentProcess.estTime;
                newEvent = eventConstructor(currentProcess.id, 1, currentTime + finishTime);
            }
            pushToEventVector(&events, newEvent);
        }

        /*PRINT CURRENT PROCESS*/
        printf("\nCURRENT PROCESS:\n");
        if (currentProcess.id != -1) {
            printf("Currently running process %d.\n", currentProcess.id);
        }
        else {
            printf("No process currently running.\n");
        }

        /*PRINT WAITING PROCESSES*/
        printf("\nPROCESSES WAITING:\n");
        if (waiting.start == waiting.processesInList) {
            printf("No processes waiting.\n");
        }
        else {
            for (i = waiting.start; i < waiting.processesInList; i++) {
                printf("%d ", waiting.processList[i].id);
            }
            printf("\n");
        }

        /*PRINT PROCESSES ON I/O INTERRUPTION*/
        printf("\nPROCESSES WAITING ON IO:\n");
        if (ioWaiting.start == ioWaiting.processesInList) {
            printf("No processes waiting on IO.\n");
        }
        else {
            for (i = ioWaiting.start; i < ioWaiting.processesInList; i++) {
                printf("%d ", ioWaiting.processList[i].id);
            }
            printf("\n");
        }

        currentTime++;
    }
    
    return 0;
}

Process processConstructor(int _id, int _startTime, int _estTime, int _ioSize, int* _io) {
    int i;
    Process temp;

    temp.id = _id;
    temp.startTime = _startTime;
    temp.estTime = _estTime;
    temp.io = ioVectorConstructor(_ioSize);
    for (i = 0; i < _ioSize; i++) {
        pushToIOVector(&temp.io, _io[i]);
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

IOVector ioVectorConstructor(int _size) {
    IOVector temp;

    temp.ioInList = 0;
    temp.start = 0;

    temp.maxListSize = _size;
    temp.ioTimes = (int*) malloc(sizeof(int) * _size);

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
    qsort(events->eventList + events->start, 
          events->eventsInList - events->start,
          sizeof(Event),
          compareEvents);
}

void sortProcessList(ProcessVector *processes) {
    qsort(processes->processList + processes->start,
          processes->processesInList - processes->start,
          sizeof(Process),
          compareProcesses);
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

void pushToIOVector(IOVector *vector, int ioTime) {
    if (vector->ioInList == vector->maxListSize) {
        vector->ioTimes = (int*) realloc(vector->ioTimes, 2 * vector->maxListSize * sizeof(int));
        vector->maxListSize *= 2;
    }
    vector->ioTimes[vector->ioInList] = ioTime;
    vector->ioInList++;
}

Process popFromProcessVector(ProcessVector *vector) {
    Process temp = vector->processList[vector->start];
    vector->start++;
    return temp;
}

int popFromIOVector(IOVector *io) {
    int temp = io->ioTimes[io->start];
    io->start++;
    return temp;
}