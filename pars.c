#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "scan.h"
#include "table.h"
#include "gen.h"
#include "error.h"
#include "ovm.h"


#define spABS       1
#define spMAX       2
#define spMIN       3
#define spDEC       4
#define spODD       5
#define spHALT      6
#define spINC       7
#define spInOpen    8
#define spInInt     9
#define spOutInt    10
#define spOutLn     11


static void StatSeq();
static tType Expression();


//  Проверка лексемы
static void Check(tLex L, char *Message){
    if(Lex != L){
        Expected(Message);
    }
    else{
        NextLex();
    }
}


//  [ +|- ] (Number | CharLiteral | Name)
static int ConstExpr(){
    int v;
    tObj *X;
    tLex Op;

    Op = lexPlus;
    if(Lex == lexPlus || Lex == lexMinus){
        Op = Lex;
        NextLex();
    }
    if(Lex == lexNum){
        v = Num;
        NextLex();
    }
    else if(Lex == lexName){
        X = Find(Name);
        if(X->Cat == catGuard){
            Error("Constant self-definition.");
        }
        else if(X->Cat != catConst){
            Expected("constant name.");
        }
        else{
            v = X->Val;
            NextLex();
        }
    }
    else{
        Expected("constant expression.");
    }
    if(Op == lexMinus){
        v = -v;
    }
    return v;
}


//  Name = ConstExpr
//  Ссылка на имя в таблице
static void ConstDecl(){
    tObj *ConstRef;
    ConstRef = NewName(Name, catGuard/* ?? */);
    NextLex();
    Check(lexEQ, "\"=\"");
    ConstRef->Val = ConstExpr();
    ConstRef->Typ = typInt;
    ConstRef->Cat = catConst;
}


//  Распознавание типа
static void ParseType(){
    tObj *TypeRef;
    if(Lex != lexName){
        Expected("name.");
    }
    else{
        TypeRef = Find(Name);
        if(TypeRef->Cat != catType){
            Expected("type's name.");
        }
        else if(TypeRef->Typ != typInt){
            Expected("integer type.");
        }
        NextLex();
    }
}


// Объявление переменных : Name {"," Name} ":" Type
static void VarDecl(){

    tObj *NameRef;
    if(Lex != lexName){
        Expected("name.");
    }
    else{
        NameRef = NewName(Name, catVar);
        //printf("\n%s", Name);
        NameRef->Typ = typInt;
        NextLex();
    }
    while(Lex == lexComma){
        NextLex();
        if(Lex != lexName){
            Expected("name.");
        }
        else{
            NameRef = NewName(Name, catVar);
            //printf("\n%s", Name);
            NameRef->Typ = typInt;
            NextLex();
        }
    }
    Check(lexColon, "\":\"");
    ParseType();
}

//  Variables & constants declarations
//  Последовательность объявлений констант и переменных:
//  { CONST {Объявление константы ";"} | VAR {Объявление переменной ";"} }
static void DeclSeq(){
    while(Lex == lexVAR || Lex == lexCONST){
        if(Lex == lexCONST){
            NextLex();
            while(Lex == lexName){
                ConstDecl();
                Check(lexSemi, "\";\"");
            }
        }
        else{
            NextLex();
            while(Lex == lexName){
                VarDecl();
                Check(lexSemi, "\";\"");
            }
        }
    }
}


//  Integer expression (целочисленное выражение)
static void IntExpression(){
    if(Expression() != typInt){
        Expected("integer expression.");
    }
}


//  Standard function(Стандартная функция)
static tType StFunc(int F){
    switch(F){
        case spABS :
            IntExpression();
            GenAbs();
            return typInt;

        case spMAX :
            ParseType();
            Gen(INT_MAX);
            return typInt;

        case spMIN :
            ParseType();
            GenMin();
            return typInt;

        case spODD :
            IntExpression();
            GenOdd();
            return typBool;
    }
    return typNone;
}


//  Factor(Множитель)
static tType Factor(){
    tObj *X;
    tType T;

    if(Lex == lexName){
        if((X = Find(Name))->Cat == catVar){
            GenAddr(X); //  Адрес переменной
            Gen(cmLoad);
            NextLex();
            return X->Typ;
        }
        else if(X->Cat == catConst){
            GenConst(X);
            NextLex();
            return X->Typ;
        }
        else if(X->Cat == catStProc && X->Typ != typNone){
            NextLex();
            Check(lexLPar, "\"(\"");
            T = StFunc(X->Val);
            Check(lexRPar, "\")\"");
        }
        else{
            Expected("variable, constant, or standard function.");
        }
    }
    else if(Lex == lexNum){
        GenConst(Num);
        NextLex();
        return typInt;
    }
    else if(Lex == lexLPar){
        NextLex();
        T = Expression();
        Check(lexRPar, "\")\"");
    }
    else{
        Expected("name, number or \"(\".");
    }
    return T;
}


//  Term (слагаемое) : Factor operation Factor
static tType Term(){
    tLex Op;
    tType T = Factor();
    if(Lex == lexMult || Lex == lexDIV || Lex == lexMOD){
        if(T != typInt){
            Error("Invalid operation for non-int operand.");
        }
        do{
            Op = Lex;
            NextLex();
            if((T = Factor()) != typInt){
                Expected("integer expression.");
            }
            switch(Op){
                case lexMult : Gen(cmMult); break;
                case lexDIV : Gen(cmDiv); break;
                case lexMOD : Gen(cmMod); break;
            }
        }
        while(Lex == lexMult || Lex == lexDIV || Lex == lexMOD);
    }
    return T;
}


//  ["+"|"-"] Term { "+"|"-" Term}
static tType SimpleExpr(){
    tType T;
    tLex Op;
    if(Lex == lexPlus || Lex == lexMinus){
        Op = Lex;
        NextLex();
        if((T = Term()) != typInt){
            Expected("integer expression.");
        }
        if(Op == lexMinus){
            Gen(cmNeg);
        }
    }
    else{
        T = Term();
    }

    if(Lex == lexPlus || Lex == lexMinus){
        if(T != typInt){
            Error("Invalid operation for non-int operand.");
        }
        do{
            Op = Lex;
            NextLex();
            if((T = Term()) != typInt){
                Expected("integer expression.");
            }
            switch(Op){
                case lexMinus : Gen(cmSub); break;
                case lexPlus : Gen(cmAdd); break;
            }
        }
        while(Lex == lexMinus || Lex == lexPlus);
    }
    return T;
}

//  SimpleExpr [ CompareOperation SimpleExpr ]
static tType Expression(){
    tLex Op;
    tType T = SimpleExpr();
    if(Lex == lexGT || Lex == lexGE || Lex == lexLT || Lex == lexLE || Lex == lexEQ || Lex == lexNE){
        Op = Lex;
        if(T != typInt){
            Error("Invalid operation for non-int operand.");
        }
        NextLex();
        if((T = SimpleExpr()) != typInt){
            Expected("integer expression.");
        }
        GenComp(Op);
        T = typBool;
    }
    return T;
}

//  Name (variable's name)
static tType Variable(){
    tObj *X;
    if(Lex != lexName){
        Expected("name.");
    }
    else{
        if((X = Find(Name))->Cat != catVar){
            Expected("var name");
        }
        GenAddr(X);
        NextLex();
        return X->Typ;
    }
}

//  Standard procedure
static void StProc(int P){
    switch(P){
        case spDEC :
            Variable();
            Gen(cmDup);
            Gen(cmLoad);
            if(Lex == lexComma){
                NextLex();
                IntExpression();
            }
            else{
                Gen(1);
            }
            Gen(cmSub);
            Gen(cmSave);
            return;

        case spINC :
            Variable();
            Gen(cmDup);
            Gen(cmLoad);
            if(Lex == lexComma){
                NextLex();
                IntExpression();
            }
            else{
                Gen(1);
            }
            Gen(cmAdd);
            Gen(cmSave);
            return;

        case spInOpen :
            //  Doing nothing.
            return;

        case spInInt :
            if(Variable() != typInt){
                Expected("integer variable.");
            }
            Gen(cmIn);
            Gen(cmSave);
            return;

        case spOutInt :
            IntExpression();
            Check(lexComma, "\",\"");
            IntExpression();
            Gen(cmOut);
            return;

        case spOutLn :
            Gen(cmOutLn);
            return;

        case spHALT :
            GenConst(ConstExpr());
            Gen(cmStop);
            return;
    }
}


static void BoolExpression(){
    if(Expression() != typBool){
        Expected("boolean expression.");
    }
}

//  Assumption : variable ":=" IntExpression
static AssStatement(){
    Variable();
    if(Lex == lexAss){
        NextLex();
        IntExpression();
        Gen(cmSave);
    }
    else{
        Expected("\":=\"");
    }
}

//  Standard procedure calling
static void CallStatement(int sp){
    Check(lexName, "standard procedure name.");
    if(Lex == lexLPar){
        NextLex();
        StProc(sp);
        Check(lexRPar, "\")\"");
    }
    else if(sp == spOutLn || sp == spInOpen){
        StProc(sp);
    }
    else{
        Expected("\"(\"");
    }
}


static void IfStatement(){
    int CondPC;
    int LastGOTO = 0;   //  Предыдущего перехода нет
    Check(lexIF, "IF");
    BoolExpression();
    CondPC = PC;        //  Запомнить положение условного перехода IF
    Check(lexTHEN, "THEN");
    StatSeq();
    while(Lex == lexELSIF){
        Gen(LastGOTO);  //  Фиктивный адрес, указывающий на место предыдущего перехода
        Gen(cmGOTO);
        LastGOTO = PC;  //  Запомнить место GOTO
        NextLex();
        Fixup(CondPC);  //  Зафиксировать адрес условного перехода
        BoolExpression();
        CondPC = PC;    //  Запомнить положение условного перехода
        Check(lexTHEN, "THEN");
        StatSeq();
    }
    if(Lex == lexELSE){
        Gen(LastGOTO);  //  Фиктивный адрес, указывающий на место предыдущего перехода
        Gen(cmGOTO);
        LastGOTO = PC;  //  Запомнить место последнего GOTO
        NextLex();
        Fixup(CondPC);  //  Зафиксировать адрес условного перехода
        StatSeq();
    }
    else{
        Fixup(CondPC);  //  Если ELSE отсутствует
    }
    Check(lexEND, "END");
    Fixup(LastGOTO);    //  направить сюда все GOTO
}


static void WhileStatement(){
    int CondPC;
    int WhilePC = PC;
    Check(lexWHILE, "WHILE");
    BoolExpression();
    CondPC = PC;
    Check(lexDO, "DO");
    StatSeq();
    Check(lexEND, "END");
    Gen(WhilePC);
    Gen(cmGOTO);
    Fixup(CondPC);
}

// "Оператор"
static void Statement(){
    tObj *X;
    char designator[NAMELEN + 1];
    char msg[80];

    if(Lex == lexName){
        if((X = Find(Name))->Cat == catModule){
            NextLex();
            Check(lexDot, "\".\"");
            if(Lex == lexName && strlen(X->Name) + strlen(Name) <= NAMELEN){
                strcpy(designator, X->Name);
                strcat(designator, ".");
                X = Find(strcat(designator, Name));
            }
            else{
                strcpy(msg, "name form module ");
                Expected(strcat(msg, X->Name));
            }
        }

        if(X->Cat == catVar){
            AssStatement();
        }
        else if(X->Cat == catStProc && X->Typ == typNone){
            CallStatement(X->Val);
        }
        else{
            Expected("name of variable or procedure");
        }
    }
    else if(Lex == lexIF){
        IfStatement();
    }
    else if(Lex == lexWHILE){
        WhileStatement();
    }
}

//  Statement {";" Statement}
static void StatSeq(){
    Statement();
    while(Lex == lexSemi){
        NextLex();
        Statement();
    }
}


static void ImportModule(){
    if(Lex == lexName){
        NewName(Name, catModule);
        if(strcmp(Name, "In")){
            Enter("In.Open", catStProc, typNone, spInOpen);
            Enter("In.Int", catStProc, typNone, spInInt);
        }
        else if(strcmp(Name, "Out")){
            Enter("Out.Int", catStProc, typNone, spOutInt);
            Enter("Out.Ln", catStProc, typNone, spOutLn);
        }
        else{
            Error("unknown module name.");
        }
        NextLex();
    }
    else{
        Expected("importing module's name.");
    }
}


// IMPORT Name {"," Name} ";"
static void Import(){
    Check(lexIMPORT ,"IMPORT");
    ImportModule();
    while(Lex == lexComma){
        NextLex();
        ImportModule();
    }
    Check(lexSemi, "\";\"");
}


// MODULE Name ";" [Import] DeclSeq [BEGIN StatSeq] END Name "."
static void Module(){
    tObj *ModuleRef;
    char msg[80];

    Check(lexMODULE, "MODULE");
    if(Lex != lexName){
        Expected("module name.");
    }
    else{
        ModuleRef = NewName(Name, catModule);
    }
    NextLex();
    Check(lexSemi, "\";\"");
    if(Lex == lexIMPORT){
        Import();
    }
    DeclSeq();

    if(Lex == lexBEGIN){
        NextLex();
        StatSeq();
    }
    Check(lexEND ,"END");

    // Сравнение имени модуля с именем после END
    if(Lex != lexName){
        Expected("module name.");
    }
    else if(strcmp(Name, ModuleRef->Name)){
        strcpy(msg, "module name \"");
        strcat(msg, ModuleRef->Name);
        Expected(strcat(msg, "\""));
    }
    else{
        NextLex();
    }

    if(Lex != lexDot){
        Expected("\".\"");
    }
    Gen(0); //  Код возврата
    Gen(cmStop);
    AllocateVariables();
}


//COMPILE HERE
void Compile(){
    InitNameTable();
    OpenScope();
    Enter("ABS", catStProc, typInt, spABS);
    Enter("MAX", catStProc, typInt, spMAX);
    Enter("MIN", catStProc, typInt, spMIN);
    Enter("DEC", catStProc, typNone, spDEC);
    Enter("ODD", catStProc, typBool, spODD);
    Enter("HALT", catStProc, typInt, spHALT);
    Enter("INC", catStProc, typNone, spINC);
    Enter("INTEGER", catType, typInt, 0);
    OpenScope();
    Module();
    CloseScope();
    CloseScope();
    puts("\nCompiling finished.");
}
