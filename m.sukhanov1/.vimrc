filetype plugin indent on
set nocompatible
set number
syntax enable
set tabstop=4
set softtabstop=4
set smarttab
set expandtab
set autoindent
set smartindent
set shiftwidth=4
set noexpandtab
set colorcolumn=110
highlight ColorColumn ctermbg=255

" from Habr https://habr.com/ru/articles/140161/

:syn keyword Keyword todo

:syn match Float /\d\+\(\.\d\)\=/

:syn match Function /\w\+\((\)\@=/

:syn match Function /\(new\s+\)\@<=\w\+/

:syn region SomeHighlightStyle start=/start-regexp/ end=/end-regexp/ skip=/regexp-to-skip-and-not-treat-as-an-end-regexp/

:syn region Comment start=/\/\*/ end=/\*\//

:syn region String start=/"/ skip=/\\"/ end=/"/

:syn region Comment start=/\/\*/ end=/\*\// fold

"set foldmethod=syntax

:syn region CodeBlock start=/{/ end=/}/ transparent fold
:syn keyword pascalKeyword begin end var procedure function
:syn keyword pascalBuiltinFunction WriteLn ReadLn Assign

:hi link pascalKeyword Keyword
:hi link pascalBuiltinFunction Keyword


