#ifndef TABLE
#define TABLE

#include "scan.h"

typedef enum{   //  Категории имён
    catConst,
    catVar,
    catType,
    catStProc,
    catModule,
    catGuard
} tCat;

typedef enum{   //  Типы
    typNone,
    typInt,
    typBool
} tType;

typedef struct tObjDesc{    //  Тип элемента таблицы имён
    char Name[NAMELEN + 1];
    tCat Cat;
    tType Typ;
    int Val;
    struct tObjDesc* Prev;
} tObj;


void InitNameTable();   //  Инициализация таблицы
void Enter(char *N, tCat C, tType T, int V);    //  Добавление элемента
tObj* NewName(char *N, tCat C); // Новое имя
tObj* Find(char *N);    //  Поиск имени
void OpenScope();   //  Открытие блока
void CloseScope();  //  Закрытие блока
tObj* FirstVar();   //  Поиск первой переменной в блоке
tObj* NextVar();    //  Поиск следующей переменной в блоке

#endif
