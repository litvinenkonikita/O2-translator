#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "location.h"
#include "text.h"


void Error(char *Msg){
    printf("Error at %i line: %s\n\n", Line, Msg);
    exit(EXIT_SUCCESS);
}

void Expected(char *Msg){
    char message[80];
    strcpy(message, "Expected ");
    Error(strcat(message, Msg));
}


void Warning(char *Msg){
    printf("\nWarning: %s\n", Msg);
}
