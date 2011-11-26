// Лексический анализатор

#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "scan.h"
#include "text.h"
#include "error.h"
#include "location.h"


#define KWNUM 34    // Количество ключевых слов языка O

tLex Lex;   // Текущая лексема
tName Name; // Строковое значение имени
int Num;    //  Значение числовых литералов


typedef char tKeyWord[9];   // 9 - Длина слова PROCEDURE


static int nkw = 0; // Номер ключевого слова


static struct{
    tKeyWord Word;
    tLex Lex;
}   KWTable[KWNUM];     //  Таблица ключевых слов


static void EnterKW(tKeyWord Name, tLex Lex){   // Добавление ключевого слова в таблицу
    strcpy(KWTable[nkw].Word, Name);
    KWTable[nkw++].Lex = Lex;
}


static tLex TestKW(){   // Проверка текущего имени
    int i;
    for(i = nkw - 1; i >= 0; i--){
        if(strcmp(Name, KWTable[i].Word) == 0){
            return KWTable[i].Lex;
        }
    }
    return lexName;
}


static void Ident(){    // Считывание текущей лексемы, не являющейся числовым литералом
    int i = 0;
    do{
        if(i < NAMELEN){
            Name[i++] = Ch;
        }
        else{
            Error("Name is too long.");
        }
        NextCh();
    }
    while(isalnum(Ch));
    Name[i] = '\0';
    Lex = TestKW();
}

static void Number(){   //   Считывание текущего числа
    Lex = lexNum;
    Num = 0;
    do{
        int d = Ch - '0';
        if(((INT_MAX - d) / 10) >= Num){
            Num = Num*10 + d;
        }
        else{
            Error("Number is too big.");
        }
        NextCh();
    } while(isdigit(Ch));
}


static void Comment(){  //  Считывание текущего комментария
    int Level = 1;  // Уровень вложенности комментариев
    NextCh();
    do{
        if(Ch == '*'){
            NextCh();
            if(Ch == ')'){
                Level--;
                NextCh();
            }
        }
        else if(Ch == '('){
            NextCh();
            if(Ch == '*'){
                Level++;
                NextCh();
            }
        }
        else /*if(Ch != chEOT)*/{
            NextCh();
        }
    } while(Level > 0 && Ch != chEOT);
    if(Level > 0){
        LexPos = Pos;
        Error("Comment unfinished.");
    }
}


void NextLex(){ //  Следующая лексема
    while(Ch == chSpace || Ch == chTab || Ch == chEOL){
        NextCh();
    }
    LexPos = Pos;
    if(isalpha(Ch)){
        Ident();
    }
    else if(isdigit(Ch)){
        Number();
    }
    else{
        switch(Ch){
            case ';' :
                NextCh();
                Lex = lexSemi;
            break;

            case ':' :
                NextCh();
                if(Ch == '='){
                    NextCh();
                    Lex = lexAss;
                }
                else Lex = lexColon;
            break;

            case '.' :
                NextCh();
                Lex = lexDot;
            break;

            case ',' :
                NextCh();
                Lex = lexComma;
            break;

            case '=' :
                NextCh();
                Lex = lexEQ;
            break;

            case '#' :
                NextCh();
                Lex = lexNE;
            break;

            case '<' :
                NextCh();
                if(Ch == '='){
                    NextCh();
                    Lex = lexLE;
                }
                else{
                    Lex = lexLT;
                }
            break;

            case '>' :
                NextCh();
                if(Ch == '='){
                    NextCh();
                    Lex = lexGE;
                }
                else{
                    Lex = lexGT;
                }
            break;

            case '(' :
                NextCh();
                if(Ch == '*'){
                    Comment();
                    NextLex();
                }
                else{
                    Lex = lexLPar;
                }
            break;

            case ')' :
                NextCh();
                Lex = lexRPar;
            break;

            case '+' :
                NextCh();
                Lex = lexPlus;
            break;

            case '-' :
                NextCh();
                Lex = lexMinus;
            break;

            case '*' :
                NextCh();
                Lex = lexMult;
            break;

            case chEOT :
                Lex = lexEOT;
            break;

            default:
                Error("Invalid symbol.");
        }
    }
}


void InitScan(){                    //  Инициализация таблицы ключевых слов - lexNone соответствует тем ключ. словам,
    EnterKW("ARRAY",    lexNone);   //  которые отсутствуют в языке О, но присутствуют в языке Оберон (и в Оберон-2).
    EnterKW("BY",       lexNone);
    EnterKW("BEGIN",    lexBEGIN);  //+
    EnterKW("CASE",     lexNone);
    EnterKW("CONST",    lexCONST);  //+
    EnterKW("DIV",      lexDIV);    //+
    EnterKW("DO",       lexDO);     //+
    EnterKW("ELSE",     lexELSE);   //+
    EnterKW("ELSIF",    lexELSIF);  //+
    EnterKW("END",      lexEND);    //+
    EnterKW("EXIT",     lexNone);
    EnterKW("FOR",      lexNone);
    EnterKW("IF",       lexIF);     //+
    EnterKW("IMPORT",   lexIMPORT); //+
    EnterKW("IN",       lexNone);
    EnterKW("IS",       lexNone);
    EnterKW("LOOP",     lexNone);
    EnterKW("MOD",      lexMOD);    //+
    EnterKW("MODULE",   lexMODULE); //+
    EnterKW("NIL",      lexNone);
    EnterKW("OF",       lexNone);
    EnterKW("OR",       lexNone);
    EnterKW("POINTER",  lexNone);
    EnterKW("PROCEDURE",lexNone);
    EnterKW("RECORD",   lexNone);
    EnterKW("REPEAT",   lexNone);
    EnterKW("RETURN",   lexNone);
    EnterKW("THEN",     lexTHEN);   //+
    EnterKW("TO",       lexNone);
    EnterKW("TYPE",     lexNone);
    EnterKW("UNTIL",    lexNone);
    EnterKW("VAR",      lexVAR);    //+
    EnterKW("WHILE",    lexWHILE);  //+
    EnterKW("WITH",     lexNone);

    NextLex();
}
