
#include <limits.h>
#include <string.h>
#include <stdlib.h>

#include "scan.h"
#include "table.h"
#include "ovm.h"
#include "error.h"

int PC;

void InitGen(){
    PC = 0;
}

void Gen(int Cmd){
    M[PC++] = Cmd;
}

void Fixup(int A){  // ??
    while(A > 0){
        //printf("\n%i", A);
        int temp = M[A-2];
        M[A-2] = PC;
        A = temp;
    }
}


void GenAbs(){
    Gen(cmDup);
    Gen(0);
    Gen(PC+3);
    Gen(cmIfGE);
    Gen(cmNeg);
}


void GenMin(){
    Gen(INT_MAX);
    Gen(cmNeg);
    Gen(1);
    Gen(cmSub);
}


void GenOdd(){
    Gen(2);
    Gen(cmMod);
    Gen(0);
    Gen(0);     // Адрес перехода вперед
    Gen(cmIfEQ);
}


void GenConst(int C){
    Gen(abs(C));
    if(C < 0){
        Gen(cmNeg);
    }
}


void GenComp(tLex Lex){ //  ?
    Gen(0);     // Адрес перехода вперед
    switch (Lex) {

        case lexEQ :
            Gen(cmIfNE);
            break;

        case lexNE :
            Gen(cmIfEQ);
            break;

        case lexLE :
            Gen(cmIfGT);
            break;

        case lexLT :
            Gen(cmIfGE);
            break;

        case lexGE :
            Gen(cmIfLT);
            break;

        case lexGT :
            Gen(cmIfLE);
            break;
    }
}


void GenAddr(tObj *X){  //  ?
    Gen(X->Val);    //  В текущую ячейку адрес предыдущей + 2
    X->Val = PC+1;  //  Адрес+2 = PC+1 - ? //
}


void AllocateVariables(){
    char msg[80];
    tObj *VRef;
    VRef = FirstVar();
    while(VRef != NULL){
        if(VRef->Val == 0){
            strcpy(msg, "Variable '");
            strcat(msg, VRef->Name);
            strcat(msg, "' is declared but never used.");
            Warning(msg);
        }
        else{
            Fixup(VRef->Val);   //
            //printf("\n%s", VRef->Name);
            PC++;
        }
        VRef = NextVar();
    }
}
