#include "core.h"
#include "token.h"

/*
│   r = a + b*c - d
│   │   │   ╰*╯   │
│   │   ╰─+──╯    │
│   │     ╰─────-─╯
│   ╰─=─────────╯
│    fin

│   r = a*x*x + b*x + c
│   │   ╰*╯ │   │ │   │
│   │    ╰─*╯   ╰*╯   │
│   │      ╰──+──╯    │
│   │         ╰─────+─╯
│   ╰─=─────────────╯
│    fin

│   r = a.v.x*b.v.y - a.v.y*b.v.x
│   │   ╰.╯ │ ╰.╯ │   ╰.╯ │ ╰.╯ │
│   │    ╰─.╯  ╰─.╯    ╰─.╯  ╰─.╯
│   │      ╰─*───╯       ╰─*───╯
│   │        ╰──────-──────╯
│   ╰─=─────────────╯
│    fin

│   r = a + b*c.d
│   │   │   │ ╰.╯
│   │   │   ╰*─╯
│   │   ╰─+──╯
│   ╰─=───╯
│    fin

│   r = a.b*c + d
│   │   ╰.╯ │   │
│   │    ╰─*╯   │
│   │      ╰──+─╯
│   ╰─=───────╯
│    fin

│   r = a.b@! + c.d[e]~ * f!!
│   │   ╰.╯     ╰.╯ │     ╰!
│   │    ╰─@     ╰─[╵]     ╰!
│   │      ╰!        ╰~     │
│   │       │         ╰─*───╯
│   │       ╰─+─────────╯
│   ╰─=───────╯
│    fin

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

│   r = a + b * f c [ d ] e @ ! g + h
│   │   │   │   │ │   │   ╰─@   │   │
│   │   │   │   │ ╰─[ ╵ ]   ╰─! │   │
│   │   │   │   ○───────╯─────╯─╯   │
│   │   │   ╰─*─╯                   │
│   │   ╰─+───╯                     │
│   │     ╰───────────────────────+─╯
│   ╰─=───────────────────────────╯
│    fin

│   (* Probably the best AST *)
│   (* Should be able to program the graph-vis too *)
│   r = f a ++ b + ++ c
│   │   │ ╰─++ │   ++─╯
│   │   ○───╯──╯   │
│   │   ╰────────+─╯
│   ╰─=──────────╯
│    fin

│   r = f a [ b + c ] . d e
│   │   │ │   ╰─+─╯     │ │
│   │   │ ╰─[   ╵   ]   │ │
│   │   │           ╰─.─╯ │
│   │   ○─────────────╯───╯
│   ╰─=─╯
│    fin

│   r = f a [ b ] [ c ] . d e @ !
│   │   │ ╰─[ ╵ ]   │     │ ╰─@
│   │   │       ╰─[ ╵ ]   │   ╰─!
│   │   │             ╰─.─╯     │
│   │   ○───────────────╯───────╯
│   ╰─=─╯
│    fin

│   r = f a[b][c].d e@!
│   │   │ ╰[╵] │  │ ╰@
│   │   │    ╰[╵] │  ╰!
│   │   │       ╰.╯   │
│   │   ○────────╯────╯
│   ╰─=─╯
│    fin

│   if a {} elseif b {} else {}
│   ○──╯ │    ○────╯ │  else─╯
│        │  elseif───◁───╯
│   if───◁────╯

│   a; if b {} c; d
│   │  ○──╯ {} ╰;─╯
│   │  if───◁───╯
│   ╰;─╯
│

│   (* Real examples from [m|test/Prims.ryn] *)
│   new_vertex := get_added new_edge.to #> ifx #0 new_edge.to new_edge.from;
│        │            │        ╰────.╯      │  #╯    ╰────.╯     ╰────.─╯
│        │            ○─────────────╯       ○──╯──────────╯───────────╯
│        │            ╰─────────────────#>──╯
│        ╰─────:=───────────────────────╯
│              ╰───────────────────────────────────────────────────────────;
│                                                                         fin

│   set_added :: Func i U64 { vertex_added[i/64]|[i%64] = true }
│       │         ○───╯──╯         │       ╰/╯    ╰%╯      │
│       │                          ╰──────[ ╵  ]   │       │
│       │                                      ╰|[ ╵  ]    │
│       │                                             ╰─=──╯
│       │                   {                           ╵      }
│       │        Func───────◁
│       ╰─────::──╯
│            fin

│   for j := done_til + 1 -> consider_til { new_weight := edges[i].weight;
│    │  │       ╰─────+─╯         │             │           ╰──[╵]   │
│    │  │             ╰───->──────╯             │                ╰.──╯
│    │  ╰─:=──────────────╯                     ╰──────:=─────────╯
│    ○────╯                                            ╰─────────────────;─◄ if
│                                         {                              ╵ ◄ }
│   for───────────────────────────────────◁
│
│
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

SUFFIX   x! x$ x++ x-- x.x x<<< x>>> x[x] x|[x] x<> x@ x~
PREFIX   +x -x ?x %x []T $T #x
ARGUMENT Parse Args
ARGFEED  x#>x

MULMATH  x%x x*x x/x
ADDMATH  x+x x-x
SHIFT    x<<x x>>x

RANGE    x->x x->.x
COMPARE  x<x x>x x<=x x>=x x!=x x==x

BITAND   x&x
BITXOR   x^x
BITOR    x|x

LOGICAND x&&x
LOGICXOR x^^x
LOGICOR  x||x

ARRAY    x<><x x<>>x x<<>x x><>x x..x

SLICE    x:x x:+x
COMMA    x,x
ASSIGN   x%=x x&=x x*=x x+=x x-=x x/=x x::x x:=x x=x x=@x x><x x^=x x^^=x x>>=x x<<=x
SEMI     x;x
│
│
│
│
│
*/

static size_t parseRecursive (size_t count, Token* tokens, bool isArguments, bool isDefinition)
{
}
