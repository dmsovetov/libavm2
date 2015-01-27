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

#include "Function.h"
#include "Avm.h"
#include "Error.h"
#include "Class.h"
#include "Domain.h"
#include "Array.h"

namespace avm2
{

// --------------------------------------------------- Function --------------------------------------------------- //

// ** Function::Function
Function::Function( Domain* domain ) : Object( domain )
{
    m_class = m_domain->findClass( "Function" );
}

// ** Function::to_string
const char* Function::to_string( void )
{
    static char str[64];
    sprintf( str, "[Function %p]", this );

    return str;
}

// ** Function::setName
void Function::setName( const Str& value )
{
    assert( m_name == "" );
    m_name = value;
}

// ** Function::name
const Str& Function::name( void ) const
{
    return m_name;
}

// ** Function::nargs
int Function::nargs( void ) const
{
    return 0;
}

// ** Function::argType
const Class* Function::argType( int index ) const
{
    return NULL;
}

// ** Function::call
Value Function::call( const Value* args, int count ) const
{
    return call( Value::undefined, args, count );
}

// ** Function::call
Value Function::call( const Value& instance, const Value* args, int count ) const
{
    Arguments argsuments( args, count );
    Frame     frame( this, m_domain, NULL, instance.asObject(), &argsuments );

    execute( &frame );

    if( frame.hasUnhandledException() ) {
        printf( "%s\n", frame.exception().asCString() );
        if( String* stackTrace = cast_to<Error>( frame.exception().asObject() )->stackTrace() ) {
            printf( "%s\n", stackTrace->toCString() );
        }
    }

    return frame.result();
}

// ** Function::executeWithInstance
Value Function::executeWithInstance( Object* instance, const Arguments& args, Frame* parentFrame )
{
    Frame frame( this, m_domain, parentFrame, instance, &args );
    execute( &frame );

    if( frame.hasUnhandledException() ) {
        if( parentFrame ) {
            parentFrame->throwException( frame.exception() );
        } else {
            printf( "%s\n", frame.exception().asCString() );
        }
    }

    return frame.result();
}

// ----------------------------------------------- FunctionClosure ------------------------------------------------ //

// ** FunctionClosure::FunctionClosure
FunctionClosure::FunctionClosure( Domain* domain, Object* instance, Function* function ) : Function( domain ), m_instance( instance ), m_function( function )
{
    
}

// ** FunctionClosure::to_string
const char* FunctionClosure::to_string( void )
{
    static char str[64];
    sprintf( str, "[FunctionClosure %p]", this );
    
    return str;
}

// ** FunctionClosure::nargs
int FunctionClosure::nargs( void ) const
{
    return m_function->nargs();
}

// ** FunctionClosure::execute
void FunctionClosure::execute( Frame* frame ) const
{
    assert( frame->instance() == 0 || frame->instance() == m_instance.get() );
    frame->setInstance( m_instance.get() );
    m_function->execute( frame );
}

// ** FunctionClosure::instance
Object* FunctionClosure::instance( void ) const
{
    return m_instance.get();
}

// ** FunctionClosure::function
Function* FunctionClosure::function( void ) const
{
    return m_function.get();
}

// ------------------------------------------------ FunctionNative ------------------------------------------------ //

// ** FunctionNative::FunctionNative
FunctionNative::FunctionNative( Domain* domain, FunctionNativeThunk thunk, int nargs, void* userData )
    : Function( domain ), m_thunk( thunk ), m_nargs( nargs ), m_user_data( userData )
{

}

// ** FunctionNative::to_string
const char* FunctionNative::to_string( void )
{
    static char str[64];
    sprintf( str, "[FunctionNative %p]", this );
    
    return str;
}

// ** FunctionNative::nargs
int FunctionNative::nargs( void ) const
{
    return m_nargs;
}

// ** FunctionNative::thunk
FunctionNativeThunk FunctionNative::thunk( void ) const
{
    return m_thunk;
}

// ** FunctionNative::execute
void FunctionNative::execute( Frame* frame ) const
{
    (*m_thunk)( frame );
}

// ------------------------------------------------ FunctionScript ------------------------------------------------ //

// ** FunctionScript::FunctionScript
FunctionScript::FunctionScript( Domain* domain, int maxStack, int maxScope )
    : Function( domain ), m_maxStack( maxStack ), m_maxScope( maxScope ), m_argCheck( false )
{

}

// ** FunctionScript::to_string
const char* FunctionScript::to_string( void )
{
    static char str[64];
    sprintf( str, "[FunctionScript %p]", this );

    return str;
}

// ** FunctionScript::super
Function* FunctionScript::super( void ) const
{
    return m_super.get_ptr();
}

// ** FunctionScript::setSuper
void FunctionScript::setSuper( Function* value )
{
    m_super = value;
}

// ** FunctionScript::instructions
const Instructions& FunctionScript::instructions( void ) const
{
    return m_instructions;
}

// ** FunctionScript::setInstructions
void FunctionScript::setInstructions( const Instructions& value )
{
    m_instructions = value;
}

// ** FunctionScript::checkArguments
bool FunctionScript::checkArguments( Frame* frame ) const
{
    if( !m_argCheck ) {
        return true;
    }

    // ** Check argument count
    if( frame->nargs() != nargs() ) {
        Value error = Error::create( m_domain, "Argument count mismatch on '%s'. Expected %d, got %d.", name().c_str(), m_argTypes.size(), frame->nargs() );
        frame->throwException( error );
        return false;
    }

    // ** Check argument types
    for( int i = 0, n = nargs(); i < n; i++ ) {
        Class*       type = m_argTypes[i].get();
        const Value& arg  = frame->arg( i );

        if( arg.isNullOrUndefined() || type == NULL || arg.is( type ) ) {
            continue;
        }

        Value error = Error::create( m_domain, "Argument %d type mismatch on '%s'. Expected %s, got %s.", i + 1, name().c_str(), type->name().c_str(), arg.type() );
        frame->throwException( error );
        return false;
    }

    return true;
}

// ** FunctionScript::execute
void FunctionScript::execute( Frame* frame ) const
{
//    frame->setName( name().c_str() );
    frame->setDefaultArgs( nargs(), m_argDefaults );

    if( checkArguments( frame ) ) {
        Avm avm( this, m_domain );
        avm.execute( this, frame );
    }
}

// ** FunctionScript::addException
void FunctionScript::addException( Exception* e )
{
    m_exceptions.push_back( e );
}

// ** FunctionScript::exceptions
const Exceptions& FunctionScript::exceptions( void ) const
{
    return m_exceptions;
}

// ** FunctionScript::returnType
Class* FunctionScript::returnType( void ) const
{
    return m_returnType.get();
}

// ** FunctionScript::argType
const Class* FunctionScript::argType( int index ) const
{
    if( index < 0 || index >= nargs() ) {
        return NULL;
    }
    
    return m_argTypes[index].get();
}

// ** FunctionScript::setReturnType
void FunctionScript::setReturnType( const ClassWeak& value )
{
    m_returnType = value;
}

// ** FunctionScript::setArguments
void FunctionScript::setArguments( const ClassesWeak& types, const ValueArray& defaults )
{
    m_argTypes    = types;
    m_argDefaults = defaults;

    // ???: Is this correct
    for( int i = 0, n = ( int )types.size(); i < n; i++ ) {
        if( types[i] != NULL ) {
            m_argCheck = true;
            break;
        }
    }
}

// ** FunctionScript::nargs
int FunctionScript::nargs( void ) const
{
    return m_argTypes.size();
}

// ------------------------------------------------ FunctionScope ------------------------------------------------ //

// ** FunctionWithScope::FunctionWithScope
FunctionWithScope::FunctionWithScope( Domain* domain, Function* function, const ScopeStack& scope ) : Function( domain ), m_function( function ), m_scope( scope )
{

}

// ** FunctionWithScope::to_string
const char* FunctionWithScope::to_string( void )
{
    static char str[64];
    sprintf( str, "[FunctionWithScope %p]", this );
    
    return str;
}

// ** FunctionWithScope::nargs
int FunctionWithScope::nargs( void ) const
{
    return m_function->nargs();
}

// ** FunctionWithScope::call
void FunctionWithScope::execute( Frame* frame ) const
{
    frame->setOuterScope( &m_scope );
    m_function->execute( frame );
}

// ------------------------------------------------------- Frame ------------------------------------------------------- //

// ** Frame::Frame
Frame::Frame( const Function* callee, Domain* domain, const Frame* parent, Object* instance, const Arguments* args )
    : m_domain( domain ), m_callee( callee ), m_parent( parent ), m_arguments( args ), m_instance( instance ), m_hasException( false )
{

}

// ** Frame::domain
Domain* Frame::domain( void ) const
{
    return m_domain;
}

// ** Frame::parent
const Frame* Frame::parent( void ) const
{
    return m_parent;
}

// ** Frame::arg
const Value& Frame::arg( int index ) const
{
    return m_arguments ? m_arguments->arg( index ) : Value::undefined;
}

// ** Frame::args
const Arguments* Frame::args( void ) const
{
    return m_arguments;
}

// ** Frame::nargs
int Frame::nargs( void ) const
{
    return m_arguments ? m_arguments->count() : 0;
}

// ** Frame::argv
ValueArray Frame::argv( int startIndex ) const
{
    ValueArray result;

    for( int i = startIndex, n = nargs(); i < n; i++ ) {
        result.push_back( arg( i ) );
    }

    return result;
}

// ** Frame::result
const Value& Frame::result( void ) const
{
    return m_result;
}

// ** Frame::setResult
void Frame::setResult( const Value& value )
{
    m_result = value;
}

// ** Frame::instance
Object* Frame::instance( void ) const
{
    return m_instance.get();
}

// ** Frame::setInstance
void Frame::setInstance( Object* value )
{
    m_instance = value;
}

// ** Frame::setOuterScope
void Frame::setOuterScope( const ScopeStack* value )
{
    m_scope.setOuter( value );
}

// ** Frame::setDefaultArgs
void Frame::setDefaultArgs( int maxArgs, const ValueArray& values )
{
    const_cast<Arguments*>( m_arguments )->setDefaults( maxArgs, values );
}

// ** Frame::throwException
void Frame::throwException( const Value& value )
{
    m_exception     = value;
    m_hasException  = true;
}

// ** Frame::handleException
void Frame::handleException( void )
{
    m_exception     = Value::undefined;
    m_hasException  = false;
}

// ** Frame::hasUnhandledException
bool Frame::hasUnhandledException( void ) const
{
    return m_hasException;
}

// ** Frame::exception
const Value& Frame::exception( void ) const
{
    return m_exception;
}

// ** Frame::fillLocalRegisters
void Frame::fillLocalRegisters( ValueArray& registers, int maxSize )
{
    registers.resize( maxSize );

    registers[0] = m_instance != NULL ? m_instance.get() : m_domain->global();

    if( !m_arguments ) {
        return;
    }

    for( int i = 0, n = m_arguments->count(); i < n; i++ ) {
        assert( (i + 1) < maxSize );
        registers[i + 1] = m_arguments->arg( i );
        
        Value::coerceInPlace( registers[i + 1], m_callee->argType( i ) );
    }
}

// ** Frame::captureStackTrace
Str Frame::captureStackTrace( void ) const
{
    Str str = Str( "\t at " ) + m_callee->name();
    return m_parent ? str + "\n" + m_parent->captureStackTrace() : str;
}

// --------------------------------------------------- Arguments ---------------------------------------------------- //

// ** Arguments::Arguments
Arguments::Arguments( const Value* args, int count )
{
    for( int i = count - 1; i >= 0; i-- ) {
        m_args.push_back( args[i] );
    }
}

// ** Arguments::clear
void Arguments::clear( void )
{
    m_args.clear();
}

// ** Arguments::setDefaults
void Arguments::setDefaults( int maxArgs, const ValueArray& values )
{
    int nargs  = count();
    int offset = maxArgs - values.size();

    for( int i = nargs; i < maxArgs; i++ ) {
        const Value& value = values[i - offset];
        m_args.insert( m_args.begin(), value );
    }
}

// ** Arguments::push
void Arguments::push( const Value& value )
{
    m_args.push_back( value );
}

// ** Arguments::count
int Arguments::count( void ) const
{
    return ( int )m_args.size();
}

// ** Arguments::values
const ValueArray& Arguments::values( void ) const
{
    return m_args;
}

// ** Arguments::arg
const Value& Arguments::arg( int index ) const
{
    int nargs = count();

    if( index >= 0 && index < nargs ) {
        return m_args[nargs - index - 1];
    }

    return Value::undefined;
}

// ** Arguments::args
ValueArray Arguments::args( int startIndex ) const
{
    ValueArray result;

    for( int i = startIndex; i < count(); i++ ) {
        result.push_back( arg( i ) );
    }

    return result;
}

void FunctionScriptClosure::length( Frame* frame )
{
    Function* f = cast_to<Function>( frame->instance() );
    assert( f );

    frame->setResult( f->nargs() );
}

void FunctionScriptClosure::call( Frame* frame )
{
    Function* f = cast_to<Function>( frame->instance() );
    assert( f );

    Object* thiz = frame->arg( 0 ).asObject();
    Value   args = new Array( frame->domain(), frame->argv( 1 ) );

    Value result = f->executeWithInstance( thiz, Arguments( &args, 1 ), frame );

    frame->setResult( result );
}

void FunctionScriptClosure::apply( Frame* frame )
{
    Function* f = cast_to<Function>( frame->instance() );
    assert( f );

    Object*      thiz = frame->arg( 0 ).asObject();
    const Value& args = frame->arg( 1 );
    assert(args.asArray());

    Value result = f->executeWithInstance( thiz, Arguments( &args, 1 ), frame );

    frame->setResult( result );
}

}
