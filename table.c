#include <stdlib.h>
#include <string.h>

#include "table.h"
#include "scan.h"
#include "error.h"


static tObj *Top;
static tObj *Bottom;
static tObj *CurrObj;


void InitNameTable(){   //  Инициализация таблицы имён
    Top = NULL;
}


void Enter(char *N, tCat C, tType T, int V){    //  Добавление записи в таблицу имён
    tObj *P = (tObj*) malloc(sizeof(*P));
    strcpy(P->Name, N);
    P->Cat = C;
    P->Typ = T;
    P->Val = V;
    P->Prev = Top;
    Top = P;
}


void OpenScope(){
    Enter("", catGuard, typNone, 0);
    if(Top->Prev == NULL){
        Bottom = Top;
    }
}


void CloseScope(){
    tObj* P;
    while( (P = Top)->Cat != catGuard ){
        Top = Top->Prev;
        free(P);
    }
    Top = Top->Prev;
    free(P);
}


tObj *NewName(char *Name, tCat Cat){   //  Добавление нового имени
    tObj *Obj = Top;
    while(Obj->Cat != catGuard && strcmp(Obj->Name, Name) ){
        Obj = Obj->Prev;
    }
    if(Obj->Cat == catGuard){
        Obj = (tObj*) malloc(sizeof(*Obj));
        strcpy(Obj->Name, Name);
        Obj->Cat = Cat;
        Obj->Val = 0;
        Obj->Prev = Top;
        Top = Obj;
    }
    else{
        Error("double name declaration.");
    }
    return Obj;
}


tObj* Find(char* Name) {
    tObj* Obj;
    strcpy(Bottom->Name, Name);
    for( Obj = Top; strcmp(Obj->Name, Name); Obj = Obj->Prev );
    if( Obj == Bottom ){
        Error("undeclaired name.");
    }
    return Obj;
}


tObj* FirstVar(){
    CurrObj = Top;
    return NextVar();
}


tObj* NextVar() {
    tObj* VRef;
    while( CurrObj != Bottom && CurrObj->Cat != catVar ){
        CurrObj = CurrObj->Prev;
    }
    if( CurrObj == Bottom ){
        return NULL;
    }
    else {
        VRef = CurrObj;
        CurrObj = CurrObj->Prev;
        return VRef;
    }
}
