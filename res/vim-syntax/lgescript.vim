" Syntax highlighting for LGEScript files

:setlocal iskeyword+=.
:setlocal iskeyword+=\$
:setlocal iskeyword+=%
:syntax case match

:syntax keyword lgescript_kw .set .reset .transition .sum .sub .mod .mul .div .nop .call .all
:syntax keyword lgescript_section .sprite .state .macro .sound .end

:syntax match lgescript_ident /\<\$[a-zA-Z0-9_]*\>/  contains=lgrscript_num
:syntax match lgescript_comment /#.*/ contains=lgescript_TODO
:syntax keyword lgescript_TODO TODO FIXME XXX contained

" there exists skip=/\\"/ but my lexer doesn't know that
:syntax region lgescript_path start=/"/ end=/"/ contains=lgescript_TODO

:syntax keyword lgescript_if .if .fi
:syntax match lgescript_if_sep /[;&]/

:syntax match lgescript_operator /[+-/*=<>!~]/
:syntax match lgescript_rng /\<%\.[0-9]*\>/

:syntax match lgescript_num /\<[0-9]*\>/

:syntax region lgescript_ifblock matchgroup=lgescript_ifblock_extremities start=/.if/ end=/.fi/ contains=ALL transparent fold

:highlight link lgescript_section Function
:highlight link lgescript_kw Statement
:highlight link lgescript_ident Identifier
:highlight link lgescript_comment Comment
:highlight link lgescript_TODO TODO
:highlight link lgescript_operator Operator
:highlight link lgescript_rng Operator
:highlight link lgescript_if Structure
:highlight link lgescript_ifblock Structure
:highlight link lgescript_if_sep Structure
:highlight link lgescript_path String
:highlight link lgescript_num Number
