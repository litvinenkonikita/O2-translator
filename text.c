#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "text.h"
#include "location.h"


#define TABSIZE 3
#define TRUE 1
#define FALSE 0

char ResetError = TRUE;
char *Message = "Source code file not opened!";
char *bytes;
int Ch = chEOT, Index = -1, len;


static FILE *fInput/*, *fOutput*/;

void NextCh(){
    if( (Ch = bytes[++Index]) == EOF ){
        Ch = chEOT;
    }
    else if(Ch == '\n'){
        Line++;
        Pos = 0;
        Ch = chEOL;
    }
    else if(Ch == '\r'){
        NextCh();
    }
    else if(Ch == '\t'){
        while(++Pos % TABSIZE);
    }
    else{
        Pos++;
    }

}


void ResetText(){
    /*if(Path == NULL){
        fprintf(stderr, "Call format: O <source code path>\n");
        exit(1);
    }
    else */if((fInput = fopen(/*Path*/"Input.txt", "rb")) == NULL){
        ResetError = TRUE;
        Message = "Source code file not found!";
    }
    else{
        ResetError = FALSE;
        fseek(fInput, 0, SEEK_END);
        len = ftell(fInput);
        if(len == -1L){
            fprintf(stderr, "Input file fseek error.");
            exit(1);
        }
        bytes = malloc(len);
        fseek(fInput, 0, SEEK_SET);
        fread(bytes, 1, len, fInput);

        Message = "OK";
        Pos = 0;
        Line = 1;
        fclose(fInput);
        NextCh();
    }

}


void CloseText(){
    free(bytes);
}
