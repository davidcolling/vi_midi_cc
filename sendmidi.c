/*
 * makes system calls to the specified sendmidi binary
 */
void sendMidi(int number, int value) {
    char command[200];
    command[0] = '\0';

    char numberString[50];
    snprintf(numberString, 50, "%d", number);

    char *program = sendMidiPath;
    strcat(command, program);

    char *options = " dev IAC\ Driver cc ";
    strcat(command, options);
    strcat(command, numberString);

    char *space = " ";
    strcat(command, space);

    char valueString[50];
    snprintf(valueString, 50, "%d", value);
    strcat(command, valueString);

    system(command);
}
