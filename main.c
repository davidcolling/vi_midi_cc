#include <stdio.h>

#include <ncurses.h>
#include "print.c"

#include <stdlib.h>
#include <string.h>
#include "list.c"

#include <pthread.h>
#include <math.h>
#include "sendmidi.c"

// shell io
void setUp();
void initWindow();
void handleKey(char input);
int quit();

// midi io
void *midiThread();

// internal io
void takeCommand();
void takeRecording();
void playRecording();

// for determining where each fader goes
int getX(int index);
int getY(int index);

// for updating controller data. these call functions defined in print.c
void selectFaderRel(int offset);
void selectFaderAbs(int index);
void activateFader(int index);
void moveFaderRelative(int index, int difference);
void moveFaderAbsolute(int index, int value);
void moveActive(int difference);

// utilities
int isInt(char const* input);
char* itoa(int val, int base);

static short keepRunning; // controls the main loop. program ends when set to 0
static int *values; // first element array of faders represented by their current continuous control value 
static int selectedFader = 0; // contains the index of the fader that is currently active
static int controllerSize; // the number of faders == the number of ints stored in values array
int rows; // the number of rows of characters on the screen
int cols; // the number of cols of characters on the screen
static int rowsOfFaders; // the number of rows of faders that will fit on the screen
static int colsOfFaders; // the number of columns of faders that will fit on the screen
static int resolution; // number of positions on each fader
static node* activeFaders = NULL; // head of the linked list of active faders
static int useActive = 0; // if 0, only selectedFader is used; if 1, the fader at each index contained in activeFaders is used
static char* recordingRegister = NULL; // string holding the current key recording

char *sendMidiPath;

int main(int argc, char **argv) {
    if (argc == 2) {
        resolution = 8;
    } else if (argc == 3) {
        resolution = atoi(argv[2]);
    } else {
        printf("wrong number or arguments\n");
        return 1;
    }
    sendMidiPath = argv[1];

    setUp();

    keepRunning = getch();
    while (keepRunning) {
        keepRunning = getch();
        handleKey(keepRunning);
    }

    return quit();
}

/*
 * starts ncurses window, calculates controllerSize, allocates values, prints origional state
 */
void setUp() {
    initWindow();

    rows = getmaxy(stdscr);
    cols = getmaxx(stdscr);
    rowsOfFaders = (rows - 1) / (resolution + 2); // subtract one from screen height for the command line
    colsOfFaders = cols / 5;
    controllerSize = rowsOfFaders * colsOfFaders;

    values = malloc(controllerSize * sizeof(int));

    char ch;
    FILE *fp;

    fp = fopen("./values.txt", "r"); 

    if (fp != NULL) {
        for (int i = 0; i < controllerSize; i++) {
            char valueString[3];
            int charCount = 0;
            while ((ch = fgetc(fp)) != EOF && ch != ' ') {
                valueString[charCount] = ch;
                charCount++;
            }
            if (ch == ' ') {
                while(charCount < 3) {
                    valueString[charCount] = ' ';
                    charCount++;
                }
            }
            values[i] = atoi(valueString);
        }
    }
    fclose(fp);

    char faderNumberOutputString[128];

    for (int i = 0; i < controllerSize; i++) {
        snprintf(faderNumberOutputString, 128, "%d", i);
        printFader(getX(i), getY(i), resolution, faderNumberOutputString);
        printKnob(getY(i) + (resolution - values[i]), getX(i));
    }
}

/*
 * intializes ncurses window, sets options
 */
void initWindow() {
    // first three commands recommended in ncurses(3X)
    initscr();
    cbreak(); // pressing return is not necessary for program to receive chars
    noecho();

    // additional recommendations from ncurses(3X)
    nonl(); // pressing return does not insert new line
    intrflush(stdscr, FALSE); // if set true ncurses may confuse what's on screen
    keypad(stdscr, TRUE); // enables function keys such as an arrow key
}

void handleKey(char input) {
    switch (input) {
        case ':' :
            takeCommand();
            break;
        case 'K' :
            moveFaderRelative(selectedFader, 1);

            if (useActive == 1) {
                moveActive(1);
            }

            break;
        case 'J' :
            moveFaderRelative(selectedFader, -1);

            if (useActive == 1) {
                moveActive(-1);
            }

            break;
        case 'H' :
            moveFaderAbsolute(selectedFader, resolution - 1);
            break;
        case 'M' :
            moveFaderAbsolute(selectedFader, resolution / 2);
            break;
        case 'L' :
            moveFaderAbsolute(selectedFader, 0);
            break;
        case 'j':
            selectFaderRel(colsOfFaders);
            break;
        case 'k':
            selectFaderRel(-1 * colsOfFaders);
            break;
        case 'h':
            selectFaderRel(-1);
            break;
        case 'l':
            selectFaderRel(1); 
            break;
        case 'G':
            selectFaderAbs(controllerSize - 1);
            break;
        case 'g':
            selectFaderAbs(0);
            break;
        case 'a':
            activateFader(selectedFader);
            break;
        case 'q':
            takeRecording();
            break;
        case '@':
            playRecording();
            break;
    }
    
    refresh();
}

/*
 * shuts down ncurses, frees memory, sets return values
 */
int quit() {
    endwin();

    FILE *fp = fopen("./values.txt","w");

    if (fp != NULL) {
        for (int i = 0; i < controllerSize; i++) {
            fprintf(fp, "%d ", values[i]);
        }
    }
    fclose(fp);

    freeList(activeFaders);

    free(values);
    free(recordingRegister);

    return 0;
}

/*
 * thread for a midi message
 */
void *midiThread() {
    sendMidi(selectedFader, floor(127 * ((double) values[selectedFader] / (double) resolution)));
    return NULL;
}

/*
 * opens command line and receives, executes command
 */
void takeCommand() {
    printCommandLine();

    nocbreak();
    echo();
 
    // TODO make this char buffer bigger
    char input[100];
    getstr(input);

    char *useActiveCommand = "useactive";
    char *quitCommand = "q";
    char *zero = "0";

    // TODO should call a function with input as argument that returns status of command, its output after performing all the required actions 
    if (strncmp(input, useActiveCommand, strlen(useActiveCommand)) == 0) {
        if (useActive == 0) {
            useActive = 1;
        } else {
            useActive = 0;
        }
    } else if (strncmp(input, quitCommand, strlen(quitCommand)) == 0) {
        keepRunning = 0;
    } else if (isInt(input)) {
        selectFaderAbs(atoi(input));
    } else if (strncmp(input, zero, strlen(zero)) == 0) {
        selectFaderAbs(0);
    } else {
        char *commandNotFoundMessage = "command not found";
        printMessage(commandNotFoundMessage);
    }

    cbreak();
    noecho();
}

/*
 * records to recordingRegister a series of keystrokes as it is performed
 */
void takeRecording() {
    if (recordingRegister != NULL) {
        free(recordingRegister);
        recordingRegister = NULL;
    }

    char *takeRecordingMessage = "recording";
    printMessage(takeRecordingMessage);

    int charCount = 0;
    char recordingInput = getchar();

    while (recordingInput != 'q') {
        charCount++;
        char *tmp = realloc(recordingRegister, charCount);

        handleKey(recordingInput);
        
        recordingRegister = tmp; // not the final recordingRegister, but it works to store everything until the next loop. after all, it isn't read in the scope of this function
        recordingRegister[charCount - 1] = recordingInput;

        recordingInput = getchar();
    }

    // null-terminate the string
    char *tmp = realloc(recordingRegister, charCount);
    recordingRegister = tmp;
    recordingRegister[charCount] = NULL;

    printCommandLine();
}

/*
 * executes the keystrodes stored in recordingRegister
 */
void playRecording() {
    if (recordingRegister == NULL) {
        char *noRecordingMessage = "there is no recording";
        printMessage(noRecordingMessage);
        return;
    }

    for (int i = 0; i < strlen(recordingRegister); i++) {
        handleKey(recordingRegister[i]);
    }
}

/*
 * returns x coordinate of upper left corner of fader at index
 */
int getX(int index) {
    return 5 * (index % colsOfFaders);
} 
/*
 * returns y coordinate of upper left corner of fader at index
 */
int getY(int index) {
    return (resolution + 2) * (index / colsOfFaders); // add 2 for top and bottom of fader
}

/*
 * changes selectedFader and prints the new selector position with functions from main.c
 * offset: the number of faders away from the current active fader to set the new active one relative to 0. eg, offset -1 would change the active fader 34 to 33
 */
void selectFaderRel(int offset) {
    if ( (0 <= selectedFader + offset) && (selectedFader + offset <= controllerSize - 1) ) {
        selectFaderAbs(selectedFader + offset);
    } else if (selectedFader == 0 && offset == -1) {
        selectFaderAbs(controllerSize - 1);
    } else if (selectedFader == controllerSize - 1 && offset == 1) {
        selectFaderAbs(0);
    }
}

/*
 * changes selectedFader and prints the new selector position with functions from main.c
 * index: the fader number to set as the new active one. eg, index 1 would change the active fader to 1
 */
void selectFaderAbs(int index) {
    void *faderPointer = selectedFader;
    if (!(activeFaders != NULL && contains(activeFaders, faderPointer)))
        unMarkSelected(getY(selectedFader), getX(selectedFader));
    selectedFader = index;
    markSelected(getY(selectedFader), getX(selectedFader));
}

/*
 * adds index to activeFaders if it isn't already active or removes index from activeFaders if it is already active
 */
void activateFader(int index) {
    void *faderPointer = index;
    if (activeFaders == NULL) {
        activeFaders = newList(faderPointer);
    } else {
        if (contains(activeFaders, faderPointer) == 0) {
            activeFaders = addNode(activeFaders, faderPointer);
            markActive(getY(index), getX(index));
        } else {
            activeFaders = rmNode(activeFaders, faderPointer);
            unMarkActive(getY(index), getX(index));
        }
    }
}

/*
 * changes the continuous control value at index by difference and prints the new fader position
 */
void moveFaderRelative(int index, int difference) {
    if (0 <= (values[index] + difference) && (values[index] + difference) <= resolution - 1) {
        rmKnob(getY(index) + (resolution - values[index]), getX(index));
        values[index] += difference;
        printKnob(getY(index) + (resolution - values[index]), getX(index));

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, midiThread, NULL);
    }
}

/*
 * changes the continuous control value at index to value
 */
void moveFaderAbsolute(int index, int value) {
    if (0 <= value && value <= resolution - 1) {
        rmKnob(getY(index) + (resolution - values[index]), getX(index));
        values[index] = value;
        printKnob(getY(index) + (resolution - value), getX(index));

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, midiThread, NULL);
    }
}

/* 
 * moves each fader whose index is stored in the list starting at head by difference
 */
void moveActive(int difference) {
    if (activeFaders != NULL) {
        node *current = activeFaders;
        if ((int) current->value != selectedFader) {
            moveFaderRelative((int) current->value, difference);
        }
    
        while (current->next != NULL) {
            current = current->next;

            if ((int) current->value != selectedFader) {
                moveFaderRelative((int) current->value, difference);
            }
        }
    }
}

int isInt(char const* input) {
    return strcmp(itoa(atoi(input), 10), input) == 0;
}

char* itoa(int val, int base) {
    static char buffer[32] = {0};
    int i = 30;

    for(; val && i ; --i, val /= base)
        buffer[i] = "0123456789abcdef"[val % base];

    return &buffer[i+1];
}

