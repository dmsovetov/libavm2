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

#ifndef __avm2__ABCDump__
#define __avm2__ABCDump__

#include "Common.h"

namespace avm2 {

    // ** class Dump
    class Dump {
    public:
        
                                Dump( void );
        
        void                    dumpABC( const AbcInfo* abc );
        
        static Str              formatMethod( const AbcInfo* abc, int index );
        static Str              formatNamespace( const AbcInfo* abc, int index, bool nameOnly = false );
        static Str              formatMultiname( const AbcInfo* abc, int index, bool nameOnly = false );
        static Str              formatNSSet( const char* name, const AbcInfo* abc, int index );
        static Str              formatClass( const AbcInfo* abc, int index );
        
        static const char*      formatOpCode( int opcode );
        static const char*      formatMultinameKind( int kind );
        static const char*      formatNamespaceKind( int kind );
        static const char*      formatString( const AbcInfo* abc, int str );
        
    private:
        
        const AbcInfo           *m_abc;
    };

} // namespace avm2

#endif /* defined(__avm2__ABCDump__) */
