project('vex', 'c', license: 'GPLv3')

src = ['src/vex.c', 'src/main.c']
dep_ncurses = dependency('ncurses', method: 'pkg-config')
executable('vex', src, dependencies: [dep_ncurses], c_args: ['-Wall', '-Werror'])
