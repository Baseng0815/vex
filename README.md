# Vex - a simple ncurses-based hex editor
## Feature list

Key | Description | Implemented?
--- | ----------- | ------------
hjkl | move left,down,up,right | yes
w | move to next word | no
b | move to beginning of word | no
e | move to end of word | no
U | move one screen up | yes
D | move one screen down | yes
g | go to beginning of file | no
G | go to end of file | no
0 | go to beginning of line | yes
$ | go to end of line | yes
\+ | increase word size | yes
\- | decrease word size | yes
: | seek offset | no
m | set marker | no
' | goto marker | no
i | insert (prepend) | no
a | insert (append) | no
Esc | exit insert mode | no
s | substitute | no
r | replace | no
