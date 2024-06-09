#include "core.h"
#include "operator.h"
#include "token.h"

/*
│   (* Diagram Key *)
│   (* First line lists operator precedence values *)
│   (* Second line is the input token list *)
│   Left   Right
│    ╰─Token─╯ Next NextOfNext
│        ○──────╯───────╯
│
│   (* Note that the structure of Next is more explicitly graphed as *)
│   Token Next NextOfNext
│     │    ○───────╯
│     ○────╯
│
│   (* An equivalent Ryn declaration is as follows *)
│   tree := Token
│       #left_token=Left
│       #right_token=Right
│       #next_token=(Token
│           #next_token=NextOfNext);

│     I   5  4  5
│   r = a + b*c - d
│   │   │   ╰*╯   │
│   │   ╰─+──╯    │
│   │     ╰─────-─╯
│   ╰─=─────────╯
│    fin

│     I  4 4  5  4  5
│   r = a*x*x + b*x + c
│   │   ╰*╯ │   │ │   │
│   │    ╰─*╯   ╰*╯   │
│   │      ╰──+──╯    │
│   │         ╰─────+─╯
│   ╰─=─────────────╯
│    fin

│     I  0 0 4 0 0  5  0 0 4 0 0
│   r = a.v.x*b.v.y - a.v.y*b.v.x
│   │   ╰.╯ │ ╰.╯ │   ╰.╯ │ ╰.╯ │
│   │    ╰─.╯  ╰─.╯    ╰─.╯  ╰─.╯
│   │      ╰─*───╯       ╰─*───╯
│   │        ╰──────-──────╯
│   ╰─=─────────────╯
│    fin

│     I   5  4 0
│   r = a + b*c.d
│   │   │   │ ╰.╯
│   │   │   ╰*─╯
│   │   ╰─+──╯
│   ╰─=───╯
│    fin

│     I  0 4  5
│   r = a.b*c + d
│   │   ╰.╯ │   │
│   │    ╰─*╯   │
│   │      ╰──+─╯
│   ╰─=───────╯
│    fin

│     I  0 00 5  0 0  0 4  00
│   r = a.b@! + c.d[e]~ * f!!
│   │   ╰.╯     ╰.╯ │     ╰!
│   │    ╰─@     ╰─[╵]     ╰!
│   │      ╰!        ╰~     │
│   │       │         ╰─*───╯
│   │       ╰─+─────────╯
│   ╰─=───────╯
│    fin

│     I   5   4  2 0  2 00 5
│   r = a + b * f c[d] e@! + g
│   │   │   │   │ ╰[╵] ╰@    │
│   │   │   │   │    │  ╰!   │
│   │   │   │   ○────╯───╯   │
│   │   │   ╰─*─╯            │
│   │   ╰─+───╯              │
│   │     ╰────────────────+─╯
│   ╰─=────────────────────╯
│    fin

Func %tokens &[length U64]Token in_args in_definition Bool -> advance_length U64

│     I   5   4  2  0    2  0 0   5
│   r = a + b * f c [ d ] e @ ! g + h
│   │   │   │   │ │   │   ╰─@   │   │
│   │   │   │   │ ╰─[ ╵ ]   ╰─! │   │
│   │   │   │   ○───────╯─────╯─╯   │
│   │   │   ╰─*─╯                   │
│   │   ╰─+───╯                     │
│   │     ╰───────────────────────+─╯
│   ╰─=───────────────────────────╯
│    fin

│   (* If increment was to make a return, it would look like this *)
│     I  2  0 2  5 1
│   r = f a ++ b + ++ c
│   │   │ ╰─++ │   ++─╯
│   │   ○───╯──╯   │
│   │   ╰────────+─╯
│   ╰─=──────────╯
│    fin

│     I  2  0   5     0  2
│   r = f a [ b + c ] . d e
│   │   │ │   ╰─+─╯     │ │
│   │   │ ╰─[   ╵   ]   │ │
│   │   │           ╰─.─╯ │
│   │   ○─────────────╯───╯
│   ╰─=─╯
│    fin

│     I  2  0     0     0  2  0 0
│   r = f a [ b ] [ c ] . d e @ !
│   │   │ ╰─[ ╵ ]   │     │ ╰─@
│   │   │       ╰─[ ╵ ]   │   ╰─!
│   │   │             ╰─.─╯     │
│   │   ○───────────────╯───────╯
│   ╰─=─╯
│    fin

│     I  2 0  0  0 2 00
│   r = f a[b][c].d e@!
│   │   │ ╰[╵] │  │ ╰@
│   │   │    ╰[╵] │  ╰!
│   │   │       ╰.╯   │
│   │   ○────────╯────╯
│   ╰─=─╯
│    fin

│   if a {} elseif b {} else {}
│   if─◁─╯  elseif─◁─╯  else─╯
│   ○─────────╯──────────╯
│  fin

│   a; if b {} c; d
│   │  if─◁─╯  ╰;─╯
│   ╰;─╯
│   fin

│   a; if b {} c; d
│   │  ○──╯ {} ╰;─╯
│   │  if───◁───╯
│   ╰;─╯
│   fin

│       I      2 2 2   2  2   ?    ?      5
│   add :: Func a b Int -> Int { return a + b }
│    │      ○───╯─╯──╯──╯───╯           ╰─+─╯
│    │                         { return───╯   }
│    │     Func────────────────╯
│    ╰──::──╯
│      fin

│   (* Real examples from [m|test/Prims.ryn] *)
│              I           2        0   3     21 2        0  2        0    J
│   new_vertex := get_added new_edge.to #> ifx #0 new_edge.to new_edge.from;
│        │            │        ╰────.╯      │  #╯    ╰────.╯     ╰────.─╯
│        │            ○─────────────╯       ○──╯──────────╯───────────╯
│        │            ╰─────────────────#>──╯
│        ╰─────:=───────────────────────╯
│              ╰───────────────────────────────────────────────────────────;
│                                                                         fin

│             I      2 2                  0 4   0  4    I
│   set_added :: Func i U64 { vertex_added[i/64]|[i%64] = true }
│       │         ○───╯──╯         │       ╰/╯    ╰%╯      │
│       │                          ╰──────[ ╵  ]   │       │
│       │                                      ╰|[ ╵  ]    │
│       │                                             ╰─=──╯
│       │                   {                           ╵      }
│       │        Func───────╯
│       ╰─────::──╯
│            fin

│         I           5   7                            I       0  0      J
│   for j := done_til + 1 -> consider_til { new_weight := edges[i].weight;
│    │  │       ╰─────+─╯         │             │           ╰──[╵]   │
│    │  │             ╰───->──────╯             │                ╰.──╯
│    │  ╰─:=──────────────╯                     ╰──────:=─────────╯
│    ○────╯                                            ╰─────────────────;─◄ if
│                                         {                              ╵ ◄ }
│   for───────────────────────────────────◁
│
│                 8                        I  J            I           J
│   if new_weight < min_weight { min_index = j; min_weight = new_weight; } }
│          ╰──────<─────╯            ╰─────=─╯      ╰──────=─────╯       ◄─╯
│   ○─────────────╯                        │               ╰───────────;
│                                          ╰──;────────────────────────╯
│                              {              ╵                          }
│   if─────────────────────────◁
│ ◄─╯
│   ╭─┐─╮
│   ╰─┬─╯╵
│   ╰┤│├╯

0 SUFFIX   x! x$ x++ x-- x.x x<<< x>>> x[x] x|[x] x<> x@ x~
1 PREFIX   +x -x ?x %x []T $T #x
2 ARGUMENT Parse Args
3 ARGFEED  x#>x

4 MULMATH  x%x x*x x/x
5 ADDMATH  x+x x-x
6 SHIFT    x<<x x>>x

7 RANGE    x->x x->.x
8 COMPARE  x<x x>x x<=x x>=x x!=x x==x

9 BITAND   x&x
A BITXOR   x^x
B BITOR    x|x

C LOGICAND x&&x
D LOGICXOR x^^x
E LOGICOR  x||x

F ARRAY    x<><x x<>>x x<<>x x><>x x..x

G SLICE    x:x x:+x
H COMMA    x,x
I ASSIGN   x%=x x&=x x*=x x+=x x-=x x/=x x::x x:=x x=x x=@x x><x x^=x x^^=x x>>=x x<<=x
J SEMI     x;x
│
│
│
│
│
*/

static size_t parseRecursive (size_t count, Token* tokens, OperatorPrecedence precedence, bool isArguments, bool isDefinition)
{
}
