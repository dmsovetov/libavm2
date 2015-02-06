avm = StaticLibrary( 'avm2', sources = [ 'avm', 'base' ] )
Executable( 'avmshell', sources = [ 'avmshell/main.cpp' ], include = [ 'avm' ], libs = [ avm ] )