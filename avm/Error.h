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

#ifndef __avm2__Error__
#define __avm2__Error__

#include "Object.h"

namespace avm2 {

    // ** class Error
    class Error : public Object {
    public:

                            AvmDeclareType( AS_ERROR, Object )

                            Error( Domain* domain, String* message = NULL, int errorId = 0 );

        // ** Object
        virtual const char* to_string( void );

        // ** Error
        String*             message( void ) const;
        void                setMessage( String* value );
        String*             name( void ) const;
        void                setName( String* value );
        int                 errorId( void ) const;
        void                setErrorId( int value );
        String*             stackTrace( void ) const;
        void                setStackTrace( String* value );

        static Error*       create( Domain* domain, const char* message, ... );

    private:

        StringPtr           m_message;
        StringPtr           m_name;
        StringPtr           m_stackTrace;
        int                 m_errorId;
    };

    AvmBeginClass( Error )
        AvmDeclareMethod( init )
        AvmDeclareMethod( getStackTrace )
        AvmDeclareProperty( message, setMessage )
        AvmDeclareProperty( name, setName )
        AvmDeclareReadonly( errorId )
    AvmEndClass

}

#endif /* defined(__avm2__Error__) */
