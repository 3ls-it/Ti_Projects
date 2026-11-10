" Vim syntax file
" Language: TI-89 / TI-BASIC (TI-89 family) - plain text approximation
if exists("b:current_syntax")
  finish
endif

" Case-insensitive keywords (TI-BASIC is effectively case-insensitive for tokens)
syn case ignore

" Strings (TI-89 uses quotes for strings in text form)
syn region ti89String start=+"+ skip=+""+ end=+"+

" Numbers (basic decimal integers/floats)
syn match ti89Number '\v<\d+(\.\d+)?>'


" Operators / punctuation of interest
" Multi-char comparisons first
syn match ti89Operator '!='
syn match ti89Operator '<='
syn match ti89Operator '>='
syn match ti89Operator '\.\*'
syn match ti89Operator '\.+'
syn match ti89Operator '\./'
syn match ti89Operator '\.-'

" Single-char operators
syn match ti89Operator '='
syn match ti89Operator '<'
syn match ti89Operator '>'
syn match ti89Operator '\^'
syn match ti89Operator '+'
syn match ti89Operator '-'
syn match ti89Operator '/'
syn match ti89Operator '\*'
syn match ti89Operator '&'
syn match ti89Operator '\<or\>'
syn match ti89Operator '\<not\>'
syn match ti89Operator '\<xor\>'
syn match ti89Operator '\<and\>'


" --- Whole-word keyword matches ---
syn match ti89Keyword '\<Func\>'
syn match ti89Keyword '\<EndFunc\>'
syn match ti89Keyword '\<Prgm\>'
syn match ti89Keyword '\<EndPrgm\>'
syn match ti89Keyword '\<Return\>'
syn match ti89Keyword '\<Local\>'

syn match ti89Keyword '\<If\>'
syn match ti89Keyword '\<Then\>'
syn match ti89Keyword '\<Else\>'
syn match ti89Keyword '\<ElseIf\>'
syn match ti89Keyword '\<EndIf\>'
syn match ti89Keyword '\<Cycle\>'

syn match ti89Keyword '\<For\>'
syn match ti89Keyword '\<EndFor\>'
syn match ti89Keyword '\<While\>'
syn match ti89Keyword '\<EndWhile\>'
syn match ti89Keyword '\<Loop\>'
syn match ti89Keyword '\<EndLoop\>'
syn match ti89Keyword '\<Try\>'
syn match ti89Keyword '\<EndTry\>'

syn match ti89Keyword '\<Lbl\>'
syn match ti89Keyword '\<Goto\>'
syn match ti89Keyword '\<Stop\>'
syn match ti89Keyword '\<Exit\>'
syn match ti89Keyword '\<Custom\>'
syn match ti89Keyword '\<EndCustm\>'
syn match ti89Keyword '\<Toolbar\>'
syn match ti89Keyword '\<EndTbar\>'
syn match ti89Keyword '\<Item\>'
syn match ti89Keyword '\<Dialog\>'
syn match ti89Keyword '\<EndDialog\>'

" Common builtins (highlight whole-word)
" builtins with parens
syn match ti89Builtin '\<char\>'
syn match ti89Builtin '\<mod\>'
syn match ti89Builtin '\<int\>'
syn match ti89Builtin '\<dim\>'
syn match ti89Builtin '\<expr\>'
syn match ti89Builtin '\<mid\>'
syn match ti89Builtin '\<shift\>'
syn match ti89Builtin '\<string\>'
syn match ti89Builtin '\<isPrime\>'
syn match ti89Builtin '\<ord\>'
syn match ti89Builtin '\<rand\>'
syn match ti89Builtin '\<round\>'

syn match ti89Builtin '\<newList\>'
syn match ti89Builtin '\<newMat\>'
syn match ti89Builtin '\<rowDim\>'
syn match ti89Builtin '\<colDim\>'

syn match ti89Builtin '\<getType\>'
syn match ti89Builtin '\<setMode\>'
syn match ti89Builtin '\<getMode\>'
syn match ti89Builtin '\<setGraph\>'
syn match ti89Builtin '\<setFold\>'
syn match ti89Builtin '\<getFold\>'
syn match ti89Builtin '\<getKey\>'

" builtins without parens
syn match ti89Builtin '\<Input\>'
syn match ti89Builtin '\<InputStr\>'
syn match ti89Builtin '\<Disp\>'
syn match ti89Builtin '\<DispHome\>'
syn match ti89Builtin '\<Pause\>'
syn match ti89Builtin '\<Archive\>'
syn match ti89Builtin '\<delVar\>'
syn match ti89Builtin '\<CustomOn\>'
syn match ti89Builtin '\<ClrDraw\>'
syn match ti89Builtin '\<ClrIO\>'

syn match ti89Builtin '\<pxlTest\>'
syn match ti89Builtin '\<pxlOff\>'
syn match ti89Builtin '\<pxlOn\>'
syn match ti89Builtin '\<RclPic\>'
syn match ti89Builtin '\<RclGDB\>'

syn match ti89Builtin '\<Title\>'
syn match ti89Builtin '\<DropDown\>'

" Program/function signature on the first line: name(a,b)
" Highlight only the name, not the args.
syn match ti89SignatureName '\%^\s*\zs\h\w*\ze\s*('


" Unicode conceals to 'pretty-print' ASCII substitutions
" comments: whole line starting with optional indent then (c)
syn region ti89Comment start='^\s*(c)' end='$' contains=ti89CommentLead
" Conceal only the literal '(c)' (NOT the indent)
syn match  ti89CommentLead '(c)' contained conceal cchar=©

" Conceal two-char ASCII assignment arrow as a real arrow
syn match ti89Arrow '->' conceal cchar=→


" Highlight links
hi def link ti89SignatureName Type
hi def link ti89Comment   Comment
hi def link ti89CommentLead Comment
hi def link ti89String    String
hi def link ti89Number    Number
hi def link ti89Operator  Operator
hi def link ti89Keyword   Keyword
hi def link ti89Builtin   Function
hi def link ti89Arrow Operator
"hi def link ti89Pretty Function


let b:current_syntax = "ti89"
