// Лексический анализатор

#ifndef SCAN
#define SCAN

#define NAMELEN 31  // Максималная длина имени

typedef char tName[NAMELEN + 1];

typedef enum {
    lexNone, lexName, lexNum,
    lexMODULE, lexIMPORT, lexBEGIN, lexEND,
    lexCONST, lexVAR, lexWHILE, lexDO,
    lexIF, lexTHEN, lexELSIF, lexELSE,
    lexMult, lexDIV, lexMOD, lexPlus, lexMinus,
    lexEQ, lexNE, lexLT, lexLE, lexGT, lexGE,
    lexDot, lexComma, lexColon, lexSemi, lexAss,
    lexLPar, lexRPar, lexQuote,
    lexEOT
}   tLex;


extern tLex Lex;    //  Текущая лексема
extern char Name[]; //  Строковое значение текущего имени
extern int Num;     //  Значение числовых литералов
extern int LexPos;  //  Позиция начала лексемы


void InitScan();
void NextLex();

#endif
