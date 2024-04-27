//TODO: Make comments tag on to the token that follows
#include "token.h"
#include "tokenizer.h"
#include "char.h"
#include "operator.h"
#include "stringlib.h"
#include "token.h"
#include "keyword.h"

/*
__ _0 _1 _2 _3 _4 _5 _6 _7 _8 _9 _A _B _C _D _E _F

0_ Nu SH ST ET EO EQ AK BL BS HT LF VT PB CR SO SI
   Ct Ct Ct Ct Ct Ct Ct Ct Ct WS WS Ct Ct WS Ct Ct

1_ DL DC DC DC DC NK SY EB CN EM SB ES FS GS RS US
   Ct Ct Ct Ct Ct Ct Ct Ct Ct Ct Ct Ct Ct Ct Ct Ct

2_    !  "  #  $  %  &  '  (  )  *  +  ,  -  .  / 
   WS Op St Op ?? Op Op Ch Op Op Op Op Op Op Op Op

3_ 0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ? 
   Nu Nu Nu Nu Nu Nu Nu Nu Nu Nu Op Op Op Op Op Op

4_ @  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O 
   Op Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt

5_ P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _ 
   Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Op Op Op Op Lt

6_ `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o 
   Op Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt

7_ p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~  DE
   Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Lt Op Op Op Op Ct

Where:
Ct = Control
WS = White Space
Op = Operator
Nu = Number
Lt = Letter
St = String
Ch = Character
 */





int main (int argc, char** argv)
{
    printf ("Starting Ryn complier.\n");
    if (argc != 2)
    {
        printf ("Number of arguments (%i) did not match the expected count\n", argc);
        return -1;
    }
    printf ("File = `%s`\n", argv[1]);

    size_t dataLength;
    char* data = readFile (argv[1], &dataLength);
    if (!data) { return -1; }

    Tokenizer tokenizer = tokenizerCreate (argv[1], dataLength, data);

    TokenList tokenList = tokenListCreate (256);

    tokenize (&tokenizer, &tokenList);

    printf ("\nList of Tokens\n");
    tokenListPrint (&tokenList);

    free (data);

    return 0;
}

