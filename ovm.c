#include <stdio.h>

#include "ovm.h"

#define readln while(getchar() != '\n')

int M[MEMSIZE]; //  ������ ����. ������

void Run(){
    register int PC = 0;            //  ����������� �������
    register int SP = MEMSIZE;    //  ��������� �����
    register int Cmd;           //  ������� �������
    int Buf;
    FILE *fOutput = fopen("Output.txt", "w");
    while( (Cmd = M[PC++]) != cmStop){
        if(Cmd >= 0){
            M[--SP] = Cmd;
        }
        else{
            switch(Cmd){

                case cmAdd :
                    SP++;
                    M[SP] += M[SP-1];
                break;

                case cmSub :
                    SP++;
                    M[SP] -= M[SP-1];
                break;

                case cmMult :
                    SP++;
                    M[SP] *= M[SP-1];
                break;

                case cmDiv :
                    SP++;
                    M[SP] /= M[SP-1];
                break;

                case cmMod :
                    SP++;
                    M[SP] %= M[SP-1];
                break;

                case cmNeg :
                    M[SP] = -M[SP];
                break;

                case cmLoad :
                    M[SP] = M[M[SP]];
                break;

                case cmSave :
                    M[M[SP+1]] = M[SP];
                    SP += 2;
                break;

                case cmDup :
                    SP--;
                    M[SP] = M[SP+1];
                break;

                case cmDrop :
                    SP++;
                break;

                case cmSwap :
                    Buf = M[SP];
                    M[SP] = M[SP+1];
                    M[SP+1] = Buf;
                break;

                case cmOver :
                    SP--;
                    M[SP] = M[SP+2];
                break;

                case cmGOTO :
                    PC = M[SP++];
                break;

                case cmIfEQ :
                    if(M[SP+2] == M[SP+1]){
                        PC = M[SP];
                    }
                    SP += 3;
                break;

                case cmIfNE :
                    if(M[SP+2] != M[SP+1]){
                        PC = M[SP];
                    }
                    SP += 3;
                break;

                case cmIfLE :
                    if(M[SP+2] <= M[SP+1]){
                        PC = M[SP];
                    }
                    SP += 3;
                break;

                case cmIfLT :
                    if(M[SP+2] < M[SP+1]){
                        PC = M[SP];
                    }
                    SP += 3;
                break;

                case cmIfGE :
                    if(M[SP+2] >= M[SP+1]){
                        PC = M[SP];
                    }
                    SP += 3;
                break;

                case cmIfGT :
                    if(M[SP+2] > M[SP+1]){
                        PC = M[SP];
                    }
                    SP += 3;
                break;

                case cmIn :
                    printf("Input integer : ");
                    scanf("%d", &(M[--SP]));
                    readln;
                break;

                case cmOut :
                    printf("%*d", M[SP], M[SP+1]);
                    SP += 2;
                break;

                case cmOutLn :
                    puts("");
                break;

                default :
                    puts("Invalid operation's code.");
                    M[PC] = cmStop;
            }
        }
        fprintf(fOutput, "PC = %i,  Cmd = %i\n", PC-1, Cmd);
        //printf("PC = %i,  Cmd = %i\n", PC, Cmd);
    }
    fprintf(fOutput, "PC = %i,  Cmd = %i\n", PC-1, Cmd);
    puts("\n");
    if(SP < MEMSIZE){
        printf("Return code : %i\n", M[SP]);
    }
    printf("Press Enter!");
    readln;
    fclose(fOutput);
}
