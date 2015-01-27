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

#ifndef __avm2__Exception__
#define __avm2__Exception__

#include "Common.h"

namespace avm2
{
    // ** class Exception
    class Exception : public ref_counted {
    public:

                                Exception( const Str& name );

        Class*                  type( void ) const;
        void                    setType( Class* value );
        int                     from( void ) const;
        void                    setFrom( int value );
        int                     to( void ) const;
        void                    setTo( int value );
        int                     target( void ) const;
        void                    setTarget( int value );
        bool                    isFinally( void ) const;

    private:

        ClassWeak               m_type;
        Str                     m_name;
        int                     m_from;
        int                     m_to;
        int                     m_target;
        bool                    m_isFinally;
    };

    typedef gc_ptr<Exception>   ExceptionPtr;
    typedef array<ExceptionPtr> Exceptions;
}

#endif /* defined(__avm2__Exception__) */
