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

#include "Exception.h"

#include "Class.h"

namespace avm2
{

// ** Exception::Exception
Exception::Exception( const Str& name ) : m_type( NULL ), m_name( name ), m_from( -1 ), m_to( -1 ), m_target( -1 )
{
    m_isFinally = (name == "");
}

// ** Exception::type
Class* Exception::type( void ) const
{
    return m_type.get();
}

// ** Exception::setType
void Exception::setType( Class* value )
{
    m_type = value;
}

// ** Exception::from
int Exception::from( void ) const
{
    return m_from;
}

// ** Exception::setFrom
void Exception::setFrom( int value )
{
    m_from = value;
}

// ** Exception::to
int Exception::to( void ) const
{
    return m_to;
}

// ** Exception::setTo
void Exception::setTo( int value )
{
    m_to = value;
}

// ** Exception::target
int Exception::target( void ) const
{
    return m_target;
}

// ** Exception::setTarget
void Exception::setTarget( int value )
{
    m_target = value;
}

// ** Exception::isFinally
bool Exception::isFinally( void ) const
{
    return m_isFinally;
}

}