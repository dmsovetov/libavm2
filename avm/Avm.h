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

#ifndef __avm2__AVM__
#define __avm2__AVM__

#include "Stack.h"
#include "Instructions.h"
#include "Exception.h"

namespace avm2
{
    // ** class Avm
    class Avm {
    public:

        // ** enum ErrorId
        enum ErrorId {
            InternalError,
            TypeError,
            ReferenceError,
            ArgumentError,
            VerifyError,

            TotalErrors
        };

    public:
        
                                Avm( const FunctionScript* function, Domain* domain );
                                ~Avm( void );

        void                    execute( const FunctionScript* function, Frame* frame );

        static void             dumpStack( const char* id, const Stack& stack );
        static void             dumpScopeStack( const char* id, const ScopeStack& stack );

    private:

        bool                    resolveProperty( Value& object, Value& value, Name* identifier, Frame* frame, bool needsClosure = false ) const;
        void                    createClosure( Object* instance, Value* value ) const;
        bool                    setProperty( Name* identifier, Frame* frame, const Value& value );
        Object*                 findProperty( Name* identifier, Frame* frame, Value* value = NULL, bool needsClosure = false ) const;
        bool                    isType( const Value& value, const Class* type ) const;

        bool                    handleException( const Exceptions& exceptions, Frame* frame, int& index, const char* opCode ) const;
        void                    throwError( Frame* frame, ErrorId errorId, const char* message, ... ) const;

    private:

        const FunctionScript*   m_function;
        Domain*                 m_domain;
    };
}

#endif /* defined(__avm2__AVM__) */
