#ifndef otext
#define otext


#define chSpace ' '
#define chTab '\t'
#define chEOL '\n'
#define chEOT '\0'


extern char ResetError;
extern char *Message;
extern int Ch;


void ResetText();
void CloseText();
void NextCh();


#endif
