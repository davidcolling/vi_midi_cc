/*
 * makes system calls to builds/sendmidi_macos
 */
void *sendMidi(int number, int value) {
    char command[100];
    command[0] = '\0';

    char numberString[50];
    snprintf(numberString, 50, "%d", number);

    const char *program = "builds/sendmidi_macos dev out cc ";
    strcat(command, program);
    strcat(command, numberString);

    char *space = " ";
    strcat(command, space);

    char valueString[50];
    snprintf(valueString, 50, "%d", value);
    strcat(command, valueString);

    system(command);
}
