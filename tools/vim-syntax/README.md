Vim syntax highlighting for lgescript
=====================================

To install in the default location (`~/.vim/syntax`), simply do a "make".

To install this file to a different location, export the INSTALLDIR evn var to point to somewhere else (e.g. `/usr/share/vim/vim*/syntax/`).

You should also set up a hook in your vimrc file to trigger lgescript syntax highlighting when opening *.lge files. You can do this by adding
```viml
autocmd BufNewFile,BufRead *.lge setlocal syntax=lgescript
```
in your `~/.vimrc` file.
