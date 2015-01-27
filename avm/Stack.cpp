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

#include "Stack.h"
#include "Domain.h"
#include "Abc.h"
#include "Multiname.h"
#include "Object.h"

namespace avm2
{

// ** Stack::swap
void Stack::swap( void )
{
    assert( size() >= 2 );
    
    int    sz = size();
    Stack& stack = *this;

    Value temp    = stack[sz - 2];
    stack[sz - 2] = stack[sz - 1];
    stack[sz - 1] = temp;
}

// ** Stack::drop
void Stack::drop( int count )
{
    while( count-- ) {
        pop();
    }
}

// ** Stack::arguments
void Stack::arguments( Arguments& args, int count )
{
    args.clear();

    AVM2_VERBOSE( "( " );
    for( int i = 0; i < count; i++ ) {
        args.push( pop() );
        AVM2_VERBOSE( "%s ", args.values().back().asCString() );
    }
    AVM2_VERBOSE( ")\n" );
}

// ------------------------------------------------------ ScopeStack ------------------------------------------------------ //

// ** ScopeStack::ScopeStack
ScopeStack::ScopeStack( int size ) : m_outer( NULL )
{
}

// ** ScopeStack::ScopeStack
ScopeStack::ScopeStack( const ScopeStack& other ) : m_outer( NULL )
{
    for( int i = 0, n = other.size(); i < n; i++ ) {
        push( other.at( i ) );
    }

    setOuter( other.m_outer );
}

ScopeStack::~ScopeStack( void )
{
    delete m_outer;
}

// ** ScopeStack::operator []
Object* ScopeStack::operator [] ( int index ) const
{
    return m_stack[index];
}

// ** ScopeStack::globalScope
Object* ScopeStack::globalScope( void ) const
{
    return m_outer ? m_outer->globalScope() : at( 0 );
}

// ** ScopeStack::setOuter
void ScopeStack::setOuter( const ScopeStack* value )
{
    if( m_outer ) {
        delete m_outer;
        m_outer = NULL;
    }

    if( value ) {
        m_outer = new ScopeStack( *value );
    }
}

// ** ScopeStack::find
Object* ScopeStack::find( const Name* name, Value* value ) const
{
    // ** Search a scope stack
    for( int i = m_stack.size() - 1; i >= 0; i-- )
    {
        Object* object = m_stack[i];

        if( object && object->resolveProperty( name, value ) ) {
            return object;
        }
    }

    return m_outer ? m_outer->find( name, value ) : NULL;
}

// ** ScopeStack::size
int ScopeStack::size( void ) const
{
    return m_stack.size();
}

// ** ScopeStack::push
void ScopeStack::push( Object* value, const char* pushedBy )
{
    m_stack.push_back( value );
    m_pushedBy.push_back( pushedBy );
}

// ** ScopeStack::pop
void ScopeStack::pop( void )
{
    m_stack.pop_back();
    m_pushedBy.pop_back();
}

// ** ScopeStack::at
Object* ScopeStack::at( int index ) const
{
    return m_stack[index];
}

}