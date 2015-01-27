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

#ifndef __avm2__ScopeStack__
#define __avm2__ScopeStack__

#include "Common.h"
#include "Value.h"

namespace avm2 {

    class AbcInfo;

    class Stack : private array<Value> {
    public:

                                Stack( int size = 0 ) { /*if( size ) resize( size );*/ }

        void                    push( const Value& v, const char* pushedBy = "" ) { push_back( v ); m_pushedBy.push_back( pushedBy ); }
        Value                   pop( void ) { Value v = back(); pop_back(); m_pushedBy.pop_back(); return v; }
        void                    arguments( Arguments& args, int count );
        const Value&            top( int index = 0 ) const { return (*this)[size() - index - 1]; }
        int                     size( void ) const { return array<Value>::size(); }
        const Value&            at( int index ) const { return (*this)[index]; }
        void                    swap( void );
        void                    drop( int count );
        void                    clear( void ) { array<Value>::clear(); m_pushedBy.clear(); }
        Str                     pushedBy( int index ) const { return m_pushedBy[index]; }

    private:

        array<Str>              m_pushedBy;
    };

    // ** class ScopeStack
    class ScopeStack {
    public:

                                ScopeStack( int size = 0 );
                                ScopeStack( const ScopeStack& other );
                                ~ScopeStack( void );

        Object*                 operator [] ( int index ) const;

        Object*                 globalScope( void ) const;
        Object*                 at( int index ) const;
        int                     size( void ) const;
        void                    push( Object* value, const char* pushedBy = "" );
        Object*                 find( const Name* name, Value* value ) const;
        void                    clear( void ) { m_stack.clear(); m_pushedBy.clear(); }

        Object*                 top( int index = 0 ) const { return (*this)[size() - index - 1]; }
        void                    pop( void );
        Str                     pushedBy( int index ) const { return m_pushedBy[index]; }
        void                    setOuter( const ScopeStack* value );

    private:

        ScopeStack*             m_outer;
        array<gc_ptr<Object> >  m_stack;
        array<Str>              m_pushedBy;
    };

} // namespace avm2

#endif /* defined(__avm2__ScopeStack__) */
