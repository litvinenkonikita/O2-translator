// ����������� ����������

#ifndef SCAN
#define SCAN

#define NAMELEN 31  // ����������� ����� �����

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


extern tLex Lex;    //  ������� �������
extern char Name[]; //  ��������� �������� �������� �����
extern int Num;     //  �������� �������� ���������
extern int LexPos;  //  ������� ������ �������


void InitScan();
void NextLex();

#endif
