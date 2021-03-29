#include "main.h"
/*
 * functions for printing ui to the screen
 */
void printFader(int x, int y, int resolution, char *id);
void printCommandLine();
void printMessage(char *message);
void markSelected(int y, int x);
void unMark(int y, int x);
void printKnob(int y, int x);
void rmKnob(int y, int x);
void markActive(int y, int x);

/*
 * prints one fader at the coordinate x, y
 */
void printFader(int x, int y, int resolution, char *id) {
    move(y, x);
    for (int i = 0; i < 5; i++) {
        addch('-');
    }

    for (int i = 0; i < resolution; i++) {
        move(y + i + 1, x);
        addch(' ');
        addch(' ');
        addch('|');
        addch(' ');
        addch(' ');
    }
    
    move(y + resolution + 1, x);
    addnstr(id, 5);
}
 
/*
 * prints prompt, clears all text on the command line
 */
void printCommandLine() {
    move(rows - 1, 0);
    addch(':');

    for (int i = 0; i < (cols - 1) - 2; i ++)
        addch(' ');

    move(rows - 1, 1);
}

/*
 * prints message on the command line
 */
void printMessage(char *message) {
    printCommandLine();
    addstr(message);
    refresh();
}

/*
 * visually indicates the fader at index is selected with ':::::' at the top of the fader
 */
void markSelected(int y, int x) {
    move(y, x);
    for (int i = 0; i < 5; i++)
        addch(':');
}

/*
 * visually indicates the fader at index is selected with ':::::' at the top of the fader
 */
void markVisual(int y, int x) {
    move(y, x);
    for (int i = 0; i < 5; i++)
        addch('.');
}

/*
 * visually indicates the fader at index is not selected or active
 */
void unMark(int y, int x) {
    move(y, x);
    for (int i = 0; i < 5; i++)
        addch('-');
}

/*
 * prints knob at given coordinate with ':::::' at the fader's current level
 */
void printKnob(int y, int x) {
    move(y, x);
    addch(':');
    addch(':');
    addch(':');
    addch(':');
    addch(':');
}

/*
 * removes knob at given coordinate
 */
void rmKnob(int y, int x) {
    move(y, x);
    addch(' ');
    addch(' ');
    addch('|');
    addch(' ');
    addch(' ');
}

/*
 * visually indicates the fader at index is active with ':::::' at the top of the fader
 */
void markActive(int y, int x) {
    move(y, x);
    for (int i = 0; i < 5; i++)
        addch(':');
}

