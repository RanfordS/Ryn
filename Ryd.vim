syn clear
set conceallevel=1

syn region Special start=_\[_ end=_\]_ contains=Special
syn match Keyword _\[\@<=\<\([^\[|\]]\{2,}\|[^lr|{}]\)\>[|\]]\@=_ contained containedin=Special
syn match Special _\[l\]_ conceal cchar=[
syn match Special _\[r\]_ conceal cchar=]
syn match Special _\[|\]_ conceal cchar=|
syn match Special _\[{\]_ conceal cchar={
syn match Special _\[}\]_ conceal cchar=}

syn region Italics start=_\[i|_ end=_\]_ contains=Special,Italics

syn region Ryn start=_{_ end=_}_ contains=Ryn
syn keyword RynKeyword for while return break continue if elseif else match using providing contained containedin=Ryn
syn match RynType "[&?]*\<[A-Z][a-zA-Z0-9_]*\>"
syn region RynComment start=_(\*_ end=_\*)_ contained containedin=Ryn contains=RynComment
syn region RynString start=_"_ skip=_\\\\\|\\"_ end=_"_ contained containedin=Ryn
syn match RynSpecial contained containedin=RynString _\\._

syn match RynConstant "\<0\>" contained containedin=Ryn
syn match RynConstant "\<0b[01]\+\([ui]\([1-9][0-9]*\)\?\)\?\>" contained containedin=Ryn
syn match RynConstant "\<0o[0-7]\+\([ui]\([1-9][0-9]*\)\?\)\?\>" contained containedin=Ryn
syn match RynConstant "\<0x[0-9A-F]\+\([ui]\([1-9][0-9]*\)\?\)\?\>" contained containedin=Ryn
syn match RynConstant "\<[1-9][0-9]*\([uifd]\([1-9][0-9]*\)\?\)\?\>" contained containedin=Ryn
syn match RynConstant "\<\([1-9][0-9]*\|0\)\.[0-9]\+\([fd]\([1-9][0-9]*\)\?\)\?\>" contained containedin=Ryn
syn match RynConstant "\<\([1-9][0-9]*\|0\)\(\.[0-9]\+\)\?e[-+]\?[1-9][0-9]*\([fd]\([1-9][0-9]*\)\?\)\?\>" contained containedin=Ryn

hi link RynKeyword Keyword
hi link RynComment Comment
hi link RynString String
hi link RynSpecial Special
hi link RynConstant Constant
