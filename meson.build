project('vex', 'c', license: 'GPLv3')

src = ['src/vex.c', 'src/main.c']
dep_ncurses = dependency('ncurses', method: 'pkg-config')
cc = meson.get_compiler('c')
dep_math = cc.find_library('m', required: true)
executable('vex', src, dependencies: [dep_ncurses, dep_math], c_args: ['-Wall', '-Werror'])
