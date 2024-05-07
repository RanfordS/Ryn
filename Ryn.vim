set nospell
syn keyword Keyword for while return break continue if elseif else match using providing label goto case
syn match Type "[&?]*\<[A-Z][a-zA-Z0-9_]*\>"
syn region Comment start=_(\*_ end=_\*)_ contains=Comment
syn region String start=_"_ skip=_\\\\\|\\"_ end=_"_
syn match Special contained containedin=String _\\._
syn match Special _'\\\?._

syn keyword Constant true false null
syn match Constant "\<0\([uifd]\([1-9][0-9]*\)\?\)\?\>"
syn match Constant "\<0b[01]\+\([ui]\([1-9][0-9]*\)\?\)\?\>"
syn match Constant "\<0o[0-7]\+\([ui]\([1-9][0-9]*\)\?\)\?\>"
syn match Constant "\<0x[0-9A-F]\+\([ui]\([1-9][0-9]*\)\?\)\?\>"
syn match Constant "\<[1-9][0-9]*\([uifd]\([1-9][0-9]*\)\?\)\?\>"
syn match Constant "\<\([1-9][0-9]*\|0\)\.[0-9]\+\([fd]\([1-9][0-9]*\)\?\)\?\>"
syn match Constant "\<\([1-9][0-9]*\|0\)\(\.[0-9]\+\)\?e[-+]\?[1-9][0-9]*\([fd]\([1-9][0-9]*\)\?\)\?\>"

syn match Macro "\\\w\+\>"
