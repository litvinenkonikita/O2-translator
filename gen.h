#ifndef GEN
#define GEN

#include "table.h"

extern int PC;

void InitGen();

void Gen(int Cmd);
void Fixup(int A);

void GenAbs();
void GenMin();
void GenOdd();
void GenConst(int C);
void GenComp(tLex Op);
void GenAddr(tObj *X);
void AllocateVariables();

#endif
