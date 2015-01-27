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

#ifndef avm2_FUNCTION_H
#define avm2_FUNCTION_H

#include "Instructions.h"
#include "Stack.h"
#include "Object.h"
#include "Exception.h"

namespace avm2 {

    // ** class Arguments
    class Arguments {
    public:

                                    Arguments( const Value* args = NULL, int count = 0 );

        void                        clear( void );
        void                        push( const Value& value );
        int                         count( void ) const;
        const ValueArray&           values( void ) const;
        ValueArray                  args( int startIndex ) const;
        const Value&                arg( int index ) const;
        void                        setDefaults( int maxArgs, const ValueArray& values );

    private:

        ValueArray                  m_args;
    };

    // ** class Function
	class Function : public Object {
    public:

                                    AvmDeclareType( AS_FUNCTION, Object );

                                    Function( Domain* domain );

        // ** Object
		virtual const char*         to_string( void );

		// ** Function
        Value                       call( const Value* args = NULL, int count = 0 ) const;
        Value                       call( const Value& instance, const Value* args = NULL, int count = 0 ) const;
        Value                       executeWithInstance( Object* instance, const Arguments& args, Frame* parentFrame );
        virtual void                execute( Frame* frame ) const = 0;
        virtual int                 nargs( void ) const;
        virtual const Class*        argType( int index ) const;
        const Str&                  name( void ) const;
        void                        setName( const Str& value );

    private:

        Str                         m_name;
	};

    // ** class FunctionClosure
    class FunctionClosure : public Function {
    public:

                                    AvmDeclareType( AS_FUNCTION_CLOSURE, Function );

                                    FunctionClosure( Domain* domain, Object* instance, Function* function );

        // ** Object
        virtual const char*         to_string( void );
        virtual int                 nargs( void ) const;

        // ** FunctionClosure
        Object*                     instance( void ) const;
        Function*                   function( void ) const;

    private:

		// ** Function
        virtual void                execute( Frame* frame ) const;

    private:

        ObjectPtr                   m_instance;
        FunctionPtr                 m_function;
    };

    // ** class FunctionNative
	class FunctionNative : public Function {
    public:

                                    AvmDeclareType( AS_C_FUNCTION, Function );

                                    FunctionNative( Domain* domain, FunctionNativeThunk func, int nargs, void* userData = NULL );

        // ** Object
        virtual const char*         to_string( void );
        virtual int                 nargs( void ) const;

        // ** FunctionNative
        FunctionNativeThunk         thunk( void ) const;

    private:

        // ** Function
        virtual void                execute( Frame* frame ) const;

    private:

		FunctionNativeThunk         m_thunk;
        int                         m_nargs;
        void*                       m_user_data;
	};

    // ** class FunctionScript
    class FunctionScript : public Function {
    friend class Avm;
    public:

                                    AvmDeclareType( AS_3_FUNCTION, Function );

                                    FunctionScript( Domain* domain, int maxStack = 0, int maxScope = 0 );

        // ** Object
        virtual const char*         to_string( void );
        virtual const Class*        argType( int index ) const;
        virtual int                 nargs( void ) const;

        // ** FunctionScript
        Function*                   super( void ) const;
        void                        setSuper( Function* value );
        const Instructions&         instructions( void ) const;
        void                        setInstructions( const Instructions& value );
        const Exceptions&           exceptions( void ) const;
        void                        addException( Exception* e );
        void                        setArguments( const ClassesWeak& types, const ValueArray& defaults );
        Class*                      returnType( void ) const;
        void                        setReturnType( const ClassWeak& value );

    private:

        // ** Function
        virtual void                execute( Frame* frame ) const;

        // ** FunctionScript
        bool                        checkArguments( Frame* frame ) const;

    private:

        Instructions                m_instructions;
        FunctionWeak                m_super;
        Exceptions                  m_exceptions;
        int                         m_maxStack;
        int                         m_maxScope;
        ClassWeak                   m_returnType;
        ClassesWeak                 m_argTypes;
        ValueArray                  m_argDefaults;
        bool                        m_argCheck;
    };

    typedef gc_ptr<FunctionScript>      FunctionScriptPtr;
    typedef array<FunctionScriptPtr>    FunctionScripts;

    // ** class FunctionWithScope
    class FunctionWithScope : public Function {
    public:

                                    FunctionWithScope( Domain* domain, Function* function, const ScopeStack& scope );

        // ** Object
        virtual const char*         to_string( void );
        virtual int                 nargs( void ) const;

    private:

        // ** Function
        virtual void                execute( Frame* frame ) const;

    private:

        FunctionPtr                 m_function;
        ScopeStack                  m_scope;
    };

    // ** class ActivationScope
    class ActivationScope : public Object {
    public:

                                    AvmDeclareType( AS_ACTIVATION_SCOPE, Object );

                                    ActivationScope( Domain* domain, const Traits* traits ) : Object( domain ) { setTraits( traits ); }

        // ** Object
        virtual const char*         to_string( void ) { return "[object ActivationScope]"; }
    };

    // ** class CatchScope
    class CatchScope : public ActivationScope {
    public:

                                    AvmDeclareType( AS_CATCH_SCOPE, ActivationScope );

                                    CatchScope( Domain* domain, const Traits* traits ) : ActivationScope( domain, traits ) {}

        // ** Object
        virtual const char*         to_string( void ) { return "[object CatchScope]"; }
    };

    // ** class Frame
    class Frame {
    friend class Avm;
    public:

                                    Frame( const Function* callee, Domain* domain, const Frame* parent, Object* instance, const Arguments* args );

        Domain*                     domain( void ) const;
        const Frame*                parent( void ) const;
        const Value&                arg( int index ) const;
        int                         nargs( void ) const;
        ValueArray                  argv( int startIndex ) const;
        const Arguments*            args( void ) const;
        Object*                     instance( void ) const;
        const Value&                result( void ) const;
        void                        setResult( const Value& value );
        void                        setInstance( Object* value );
        void                        setOuterScope( const ScopeStack* value );
        void                        setDefaultArgs( int maxArgs, const ValueArray& values );

        void                        throwException( const Value& error );
        bool                        hasUnhandledException( void ) const;
        void                        handleException( void );
        const Value&                exception( void ) const;
        Str                         captureStackTrace( void ) const;

    private:

        void                        fillLocalRegisters( ValueArray& registers, int maxSize );

    private:

        Domain*                     m_domain;
        const Function*             m_callee;
        const Frame*                m_parent;
        const Arguments*            m_arguments;
        ObjectPtr                   m_instance;
        Value                       m_exception;
        bool                        m_hasException;
        Value                       m_result;
        Stack                       m_stack;
        ScopeStack                  m_scope;
    };

    AvmBeginClass( FunctionScript )
        AvmDeclareMethod( call )
        AvmDeclareMethod( apply )
        AvmDeclareReadonly( length )
    AvmEndClass
}

#endif
