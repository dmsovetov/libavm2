/**************************************************************************

 The MIT License (MIT)

 Copyright (c) 2015 Dmitry Sovetov

 https://github.com/dmsovetov

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 **************************************************************************/

#include <Domain.h>
#include <Linker.h>
#include <Abc.h>

using namespace avm2;

int main(int argc, const char * argv[])
{
    std::string fileName = "";
    bool        verbose  = false;

    for( int i = 0; i < argc; i++ ) {
        if( strcmp( argv[i], "-abc" ) == 0 ) {
            fileName = argv[i + 1];
        }
        if( strcmp( argv[i], "-verbose" ) == 0 ) {
            verbose = strcmp( argv[i + 1], "true" ) == 0;
        }
    }

    if( fileName == "" ) {
        return;
    }

    logger::set_standard_log_handlers();
    avm2::set_verbose_action( verbose );

    tu_file* file = new tu_file( fileName.c_str(), "rb" );
    if( file->get_error() ) {
        printf( "no such file %s\n", fileName.c_str() );
        return 1;
    }
    
    stream*  in   = new stream( file );

    AbcInfo* abc = new AbcInfo;
    abc->read( in, NULL );

    Domain* domain = new avm2::Domain;
    domain->registerPackages();

    Linker linker( domain, abc );
    linker.link();

    return 0;
}

