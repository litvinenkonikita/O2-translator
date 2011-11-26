#ifndef TABLE
#define TABLE

#include "scan.h"

typedef enum{   //  ��������� ���
    catConst,
    catVar,
    catType,
    catStProc,
    catModule,
    catGuard
} tCat;

typedef enum{   //  ����
    typNone,
    typInt,
    typBool
} tType;

typedef struct tObjDesc{    //  ��� �������� ������� ���
    char Name[NAMELEN + 1];
    tCat Cat;
    tType Typ;
    int Val;
    struct tObjDesc* Prev;
} tObj;


void InitNameTable();   //  ������������� �������
void Enter(char *N, tCat C, tType T, int V);    //  ���������� ��������
tObj* NewName(char *N, tCat C); // ����� ���
tObj* Find(char *N);    //  ����� �����
void OpenScope();   //  �������� �����
void CloseScope();  //  �������� �����
tObj* FirstVar();   //  ����� ������ ���������� � �����
tObj* NextVar();    //  ����� ��������� ���������� � �����

#endif
