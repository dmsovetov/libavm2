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

#include "Error.h"
#include "Function.h"

namespace avm2
{

// ------------------------------------------------------ Error ------------------------------------------------------ //

// ** Error::Error
Error::Error( Domain* domain, String* message, int errorId ) : Object( domain ), m_message( message ), m_errorId( errorId )
{

}

// ** Error::toString
const char* Error::to_string( void )
{
    if( m_message == NULL || m_message->length() == 0 ) {
        return "Error";
    }

    static char buf[64];
    sprintf( buf, "Error: %s", m_message->toCString() );

    return buf;
}

// ** Error::message
String* Error::message( void ) const
{
    return m_message.get();
}

// ** Error::setMessage
void Error::setMessage( String* value )
{
    m_message = value;
}

// ** Error::name
String* Error::name( void ) const
{
    return m_name.get();
}

// ** Error::setName
void Error::setName( String* value )
{
    m_name = value;
}

// ** Error::errorId
int Error::errorId( void ) const
{
    return m_errorId;
}

// ** Error::setErrorId
void Error::setErrorId( int value )
{
    m_errorId = value;
}

// ** Error::stackTrace
String* Error::stackTrace( void ) const
{
    return m_stackTrace.get();
}

// ** Error::setStackTrace
void Error::setStackTrace( String* value )
{
    m_stackTrace = value;
}

// ** Error::create
Error* Error::create( Domain* domain, const char* message, ... )
{
    const int kFormatBufferSize = 4096;

    char szBuf[kFormatBufferSize];

    va_list ap;
    va_start( ap, message );
    vsnprintf( szBuf, kFormatBufferSize, message, ap );
    va_end( ap );

    return new Error( domain, new String( domain, szBuf ) );
}

// --------------------------------------------------- ErrorClosure -------------------------------------------------- //

void ErrorClosure::init( Frame* frame )
{
    Error* e = cast_to<Error>( frame->instance() );
    assert(e);

    String* message = cast_to<String>( frame->arg(0).asObject() );
    int     errorId = frame->arg(1).asInt();

    e->setMessage( message );
    e->setErrorId( errorId );

    if( const Frame* parent = frame->parent() ) {
        e->setStackTrace( new String( parent->domain(), Str( e->to_string() ) + "\n" + parent->captureStackTrace() ) );
    }
}

void ErrorClosure::getStackTrace( Frame *frame )
{
    Error* e = cast_to<Error>( frame->instance() );
    assert(e);

    String* stackTrace = e->stackTrace();
    frame->setResult( stackTrace );
}

void ErrorClosure::message( Frame* frame )
{
    
}

void ErrorClosure::setMessage( Frame* frame )
{
    
}

void ErrorClosure::name( Frame* frame )
{
    
}

void ErrorClosure::setName( Frame* frame )
{
    
}

void ErrorClosure::errorId( Frame* frame )
{
    
}

}