#include <stdio.h>
#include <stdlib.h>

#include "location.h"
#include "pars.h"
#include "text.h"
#include "scan.h"
#include "table.h"
#include "ovm.h"
#include "gen.h"


void Init(){
    ResetText();
    if(ResetError){
        Error(Message);
    }
    InitScan();
    InitGen();
}

void Done(){
    CloseText();
}

int main(int argc, char *argv[]){
    printf("\nO-compiler...\n");
    if(argc <= 1){
        Path = NULL;
    }
    else{
        Path = argv[1];
    }
    Init();
    Compile();
    Run();
    Done();
    return 0;
}
