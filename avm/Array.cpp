/**************************************************************************

 The MIT License (MIT)

 Copyright (c) 2003 Thatcher Ulrich <tu@tulrich.com>
 Copyright (c) 2007 Vitaly Alexeev  <tishka92@yahoo.com>
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

#include "Array.h"
#include "Function.h"
#include "Domain.h"
#include "Class.h"

namespace avm2
{

// -------------------------------------------------------------- Array -------------------------------------------------------- //

// ** Array::Array
Array::Array( Domain* domain, int size ) : Object( domain )
{
    m_class = domain->findClass( "Array" );
    assert( m_class != NULL );
    
    m_array.resize( size );
}

// ** Array::Array
Array::Array( Domain* domain, const ValueArray& values ) : Object( domain )
{
    m_class = domain->findClass( "Array" );
    assert( m_class != NULL );

    for( int i = 0; i < values.size(); i++ ) {
        m_array.push_back( values[i] );
    }
}

// ** Array::to_string
const char* Array::to_string( void )
{
    m_stringValue = join().asString();

    if( get_verbose_action() ) {
        m_stringValue = Str( "[Array " ) + m_stringValue + "]";
    }

    return m_stringValue.c_str();
}

// ** Array::setMember
bool Array::set_member( const Str& name, const Value& value )
{
    int index;

    if( string_to_number( &index, name.c_str() ) ) {
        if( index >= m_array.size() ) {
            m_array.resize( index + 1 );
        }
        m_array[index] = value;

        return true;
    }

    return Object::set_member( name, value );
}

// ** Array::get_member
bool Array::get_member( const Str& name, Value* value )
{
    int index;

    if( !string_to_number( &index, name.c_str() ) ) {
        return Object::get_member( name, value );
    }

    if( index >= 0 && index < length() ) {
        if( value ) *value = m_array[index];
        return true;
    }

    return false;
}

// ** Array::createIterator
IteratorPtr Array::createIterator( const Value& value )
{
    return new ArrayIterator( &m_array, value.asInt() );
}

// ** Array::nextKey
Value Array::nextKey( const Value& key ) const
{
    return key.asInt() - 1; // ** VM passed a 1-based indices
}

// ** Array::getPropertyByKey
Value Array::getPropertyByKey( const Value& key ) const
{
    int idx = key.isNumber() ? key.asInt() - 1 : -1;
    return idx >= 0 && idx < length() ? m_array[idx] : Value::undefined;
}

// ** Array::splice
Value Array::splice( int startIndex, int deleteCount, const ValueArray& values )
{
    if( startIndex  < 0 ) startIndex  = length() + startIndex;
    if( deleteCount < 0 ) deleteCount = length() - startIndex;

    // ** Delete values
    ValueArray result;

    for( int i = 0; i < deleteCount; i++ ) {
        result.push_back( m_array[startIndex] );
        m_array.erase( m_array.begin() + startIndex );
    }

    // ** Insert values
    for( int i = 0, n = ( int )values.size(); i < n; i++ ) {
        m_array.insert( m_array.begin() + startIndex + i, values[i] );
    }

    return new Array( m_domain, result );
}

// ** Array::slice
Value Array::slice( int startIndex, int endIndex ) const
{
    if( startIndex < 0 )        startIndex = length() + startIndex;
    if( endIndex   < 0 )        endIndex   = length() + endIndex + 1;
    if( endIndex   > length() ) endIndex   = length();

    ValueArray result;

    for( int i = startIndex; i < endIndex; i++ ) {
        result.push_back( m_array[i] );
    }

    return new Array( m_domain, result );
}

// ** Array::concat
void Array::concat( ValueArray& target, const Value& value )
{
    if( value.isArray() ) {
        const ValueArray& items = value.asArray()->items();

        for( ValueArray::const_iterator i = items.begin(), end = items.end(); i != end; ++i ) {
            concat( target, *i );
        }
    } else {
        target.insert( target.end(), value );
    }
}

// ** Array::concat
Value Array::concat( const ValueArray& values ) const
{
    ValueArray result = m_array;

    for( ValueArray::const_iterator i = values.begin(), end = values.end(); i != end; ++i ) {
        concat( result, *i );
    }

    return new Array( m_domain, result );
}

// ** Array::length
int Array::length( void ) const
{
    return ( int )m_array.size();
}

// ** Array::items
const ValueArray& Array::items( void ) const
{
    return m_array;
}

// ** Array::push
int Array::push( const ValueArray& values )
{
    for( int i = 0, n = ( int )values.size(); i < n; i++ ) {
        m_array.push_back( values[i] );
    }

    return length();
}

// ** Array::push
int Array::push( const Value& value )
{
    m_array.push_back( value );
    return length();
}

// ** Array::resize
void Array::resize( int size )
{
    m_array.resize( size );
}

// ** Array::pop
Value Array::pop( void )
{
    Value value = m_array.back();
    m_array.pop_back();

    return value;
}

// ** Array::shift
Value Array::shift( void )
{
    Value result = m_array[0];
    m_array.erase( m_array.begin() );

    return result;
}

// ** Array::unshift
int Array::unshift( const Value& value )
{
    m_array.insert( m_array.begin(), value );
    return length();
}

// ** Array::every
bool Array::every( Function *function, const Value& instance ) const
{
    for( int i = 0, n = ( int )m_array.size(); i < n; i++ ) {
        Value args[] = { m_array[i], i, ( Object* )this };
        Value result = function->call( Value::undefined, args, 3 );

        if( result.asBool() == false ) {
            return false;
        }
    }

    return true;
}

// ** Array::some
bool Array::some( Function *function, const Value& instance ) const
{
    for( int i = 0, n = ( int )m_array.size(); i < n; i++ ) {
        Value args[] = { m_array[i], i, ( Object* )this };
        Value result = function->call( Value::undefined, args, 3 );

        if( result.asBool() == true ) {
            return true;
        }
    }
    
    return false;
}

// ** Array::forEach
void Array::forEach( Function* function, const Value& instance ) const
{
    for( int i = 0, n = ( int )m_array.size(); i < n; i++ ) {
        Value args[] = { m_array[i], i, ( Object* )this };
        function->call( Value::undefined, args, 3 );
    }
}

// ** Array::map
Value Array::map( Function* function, const Value& instance ) const
{
    ValueArray mapped;

    for( int i = 0, n = ( int )m_array.size(); i < n; i++ ) {
        Value args[] = { m_array[i], i, ( Object* )this };
        Value result = function->call( Value::undefined, args, 3 );

        mapped.push_back( result );
    }

    return new Array( m_domain, mapped );
}

// ** Array::filter
Value Array::filter( Function *function, const Value& instance ) const
{
    ValueArray filtered;

    for( int i = 0, n = ( int )m_array.size(); i < n; i++ ) {
        Value args[] = { m_array[i], i, ( Object* )this };
        Value result = function->call( Value::undefined, args, 3 );

        if( result.isBool() && result.asBool() ) {
            filtered.push_back( m_array[i] );
        }
    }
    
    return new Array( m_domain, filtered );
}

// ** Array::copy_to
void Array::copy_to( Object* target )
{
    assert( false );
/*
    if (target)
    {
        for (string_hash<Value>::const_iterator it = m_members.begin();
             it != m_members.end(); ++it )
        {
            if (it->second.is_enum())
            {
                target->set_member(it->first, it->second);
            }
        }
    }
*/
}

// ** Array::indexOf
int Array::indexOf( const Value& value, int startIndex ) const
{
    if( startIndex < 0 )        startIndex = 0;
    if( startIndex > length() ) startIndex = length();
    
    for( int i = startIndex, n = ( int )m_array.size(); i < n; i++ ) {
        if( Value::compare( m_array[i], value ) ) {
            return i;
        }
    }

    return -1;
}

// ** Array::lastIndexOf
int Array::lastIndexOf( const Value& value, int startIndex ) const
{
    if( startIndex < 0 )        startIndex = length() - 1;
    if( startIndex > length() ) startIndex = length() - 1;

    for( int i = startIndex; i >= 0; i-- ) {
        if( Value::compare( m_array[i], value ) ) {
            return i;
        }
    }

    return -1;
}

// ** Array::join
Value Array::join( const Str& separator ) const
{
    Str result = "";

    for( int i = 0, n = ( int )m_array.size(); i < n; i++ ) {
        const Value& value = m_array[i];
        result += value.isUndefined() ? "" : value.asString();

        if( i < (n - 1) ) {
            result += separator;
        }
    }

    return new String( m_domain, result );
}

// ** Array::reverse
Value Array::reverse( void )
{
    ValueArray result;

    for( int i = length() - 1; i >= 0; i-- ) {
        result.push_back( m_array[i] );
    }

    m_array = result;

    return this;
}


// ** Array::sort
void Array::sort( const Value& first, const Value& second )
{
    IntegerArray flags;

    if( first.isNumber() )       flags.push_back( first.asInt() );
    else if( second.isNumber() ) flags.push_back( second.asInt() );

    std::sort( m_array.begin(), m_array.end(), ArrayComparator( first.asFunction(), StrArray(), flags ) );
}

// ** Array::sortOn
void Array::sortOn( const Value& fieldName, const Value& options )
{
    StrArray     fields = fieldName.asStrArray();
    IntegerArray flags  = options.asIntegerArray();

    std::sort( m_array.begin(), m_array.end(), ArrayComparator( NULL, fields, flags ) );
}

// --------------------------------------------------------- ArrayIterator --------------------------------------------------------- //

// ** ArrayIterator::ArrayIterator
ArrayIterator::ArrayIterator( ValueArray* values, int idx ) : m_values( values ), m_iterator( idx )
{
    next();
}

// ** ArrayIterator::hasNext
bool ArrayIterator::hasNext( void ) const
{
    return m_iterator <= ( int )m_values->size();
}

// ** ArrayIterator::next
void ArrayIterator::next( void )
{
    ++m_iterator;
}

// ** ArrayIterator::key
Value ArrayIterator::key( void ) const
{
    return m_iterator;
}

// -------------------------------------------------------- ArrayComparator -------------------------------------------------------- //

// ** ArrayComparator::ArrayComparator
ArrayComparator::ArrayComparator( Function* function, const StrArray& fields, const IntegerArray& flags ) : m_function( function ), m_fields( fields ), m_flags( flags )
{
    int mask = flags.size() ? flags[0] : 0;

    m_compareValues = comparatorFromFlags( mask );
    m_descend       = mask & Array::Descending ? -1 : 1;
}

// ** ArrayComparator::comparatorFromFlags
ArrayComparator::CompareThunk ArrayComparator::comparatorFromFlags( int flags ) const
{
    Type type = typeFromFlags( flags );

    switch( type ) {
    case Numeric:               return &ArrayComparator::compareNumeric;             break;
    case Text:                  return &ArrayComparator::compare;                    break;
    case TextCaseInsensitive:   return &ArrayComparator::compareCaseInsensitive;     break;
    }

    assert( false );
    return NULL;
}

// ** ArrayComparator::typeFromFlags
ArrayComparator::Type ArrayComparator::typeFromFlags( int flags ) const
{
    if( flags & Array::Numeric ) {
        return Numeric;
    }

    return flags & Array::CaseInsensitive ? TextCaseInsensitive : Text;
}

// ** ArrayComparator::operator
bool ArrayComparator::operator()( const Value& a, const Value& b )
{
    // ** Compare by a function
    if( m_function ) {
        Value args[] = { b, a };
        return m_function->call( Value::undefined, args, 2 ).asNumber() * m_descend > 0;
    }

    // ** Compare by a properties
    int fields = m_fields.size();

    if( fields ) {
        return compareProperties( fields, a, b );
    }

    // ** Compare values
    return m_compareValues( a, b ) * m_descend < 0;
}

// ** ArrayComparator::compareProperties
bool ArrayComparator::compareProperties( int count, const Value& a, const Value& b ) const
{
    int nFlags = m_flags.size();

    for( int i = 0; i < count; i++ ) {
        int flags   = m_flags[imin(nFlags - 1, i)];
        int descend = flags & Array::Descending ? -1 : 1;
        int result  = compareProperties( m_fields[i], flags, a, b ) * descend;

        if( result ) {
            return result < 0;
        }
    }

    return false;
}

// ** ArrayComparator::compare
int ArrayComparator::compare( const Value& a, const Value& b )
{
    return strcmp( a.asCString(), b.asCString() );
}

// ** ArrayComparator::compareCaseInsensitive
int ArrayComparator::compareCaseInsensitive( const Value& a, const Value& b )
{
    return strcasecmp( a.asCString(), b.asCString() );
}

// ** ArrayComparator::compareNumeric
int ArrayComparator::compareNumeric( const Value& a, const Value& b )
{
    double va = a.asNumber();
    double vb = b.asNumber();

    if( va == vb ) {
        return 0;
    }

    if( va < vb ) {
        return -1;
    }

    return 1;
}

// ** ArrayComparator::compareProperties
int ArrayComparator::compareProperties( const Str& name, int flags, const Value& a, const Value& b ) const
{
    Object* objectA = a.asObject();
    Object* objectB = b.asObject();
    
    if( !objectA || !objectB ) {
        return 0;
    }
    
    Value propA, propB;
    
    if( !objectA->resolveProperty( name, &propA ) || !objectB->resolveProperty( name, &propB ) ) {
        return 0;
    }
    
    CompareThunk cmp = comparatorFromFlags( flags );
    return cmp( propA, propB );
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------------ //

void ArrayClosure::init( Frame* frame )
{
    Array* ao = cast_to<Array>( frame->instance() );
    assert( ao );

    if( frame->nargs() == 1 && frame->arg(0).isNumber() ) {
        ao->resize( frame->arg(0).asInt() );
    } else {
        ao->push( frame->argv( 0 ) );
    }
}

void ArrayClosure::length( Frame* frame )
{
    Array* a = cast_to<Array>( frame->instance() );
    assert(a);

    frame->setResult( a->length() );
}

void ArrayClosure::push( Frame* frame )
{
    Array* a = cast_to<Array>( frame->instance() );
    assert(a);

    if( frame->nargs() > 0 ) {
        a->push( frame->argv( 0 ) );
    }

    frame->setResult( a->length() );
}

void ArrayClosure::pop( Frame* frame )
{
    Array* a = cast_to<Array>( frame->instance() );
    assert(a);

    frame->setResult( a->pop() );
}

void ArrayClosure::every( Frame* frame )
{
    Array* a = cast_to<Array>( frame->instance() );
    assert(a);

    Function* function = frame->arg(0).asFunction();
    Object*   object   = frame->nargs() > 1 ? frame->arg(1).asObject() : NULL;

    bool r = a->every( function, object );
    frame->setResult( r );
}

void ArrayClosure::some( Frame* frame )
{
    Array* a = cast_to<Array>( frame->instance() );
    assert(a);

    Function* function = frame->arg(0).asFunction();
    Object*   object   = frame->nargs() > 1 ? frame->arg(1).asObject() : NULL;

    bool r = a->some( function, object );
    frame->setResult( r );
}

void ArrayClosure::forEach( Frame* frame )
{
    Array* a = cast_to<Array>( frame->instance() );
    assert(a);

    Function* function = frame->arg(0).asFunction();
    Object*   object   = frame->nargs() > 1 ? frame->arg(1).asObject() : NULL;

    a->forEach( function, object );
}

void ArrayClosure::sort( Frame* frame )
{
    Array* a = cast_to<Array>( frame->instance() );
    assert(a);

    Value first  = frame->nargs() > 0 ? frame->arg(0) : Value::undefined;
    Value second = frame->nargs() > 1 ? frame->arg(1) : Value::undefined;

    a->sort( first, second );
    frame->setResult( a );
}

void ArrayClosure::sortOn( Frame* frame )
{
    Array* a = cast_to<Array>( frame->instance() );
    assert(a);

    Value fieldName = frame->nargs() > 0 ? frame->arg(0) : Value::undefined;
    Value options   = frame->nargs() > 1 ? frame->arg(1) : Value::undefined;

    a->sortOn( fieldName, options );
}

void ArrayClosure::filter( Frame* frame )
{
    Array* a = cast_to<Array>( frame->instance() );
    assert(a);

    Function* function = frame->arg(0).asFunction();
    Object*   object   = frame->nargs() > 1 ? frame->arg(1).asObject() : NULL;

    frame->setResult( a->filter( function, object ) );
}

void ArrayClosure::map( Frame* frame )
{
    Array* a = cast_to<Array>( frame->instance() );
    assert(a);

    Function* function = frame->arg(0).asFunction();
    Object*   object   = frame->nargs() > 1 ? frame->arg(1).asObject() : NULL;

    frame->setResult( a->map( function, object ) );
}

void ArrayClosure::indexOf( Frame* frame )
{
    Array* a = cast_to<Array>( frame->instance() );
    assert(a);

    Value value = frame->arg(0);
    frame->setResult( a->indexOf( value ) );
}

void ArrayClosure::lastIndexOf( Frame* frame )
{
    Array* a = cast_to<Array>( frame->instance() );
    assert(a);

    Value value = frame->arg(0);
    frame->setResult( a->lastIndexOf( value ) );
}

void ArrayClosure::splice( Frame* frame )
{
    Array* a = cast_to<Array>( frame->instance() );
    assert(a);

    if( frame->nargs() <= 0 ) {
        return;
    }

    int         startIndex  = frame->arg( 0 ).asInt();
    int         deleteCount = frame->nargs() > 1 ? frame->arg( 1 ).asInt() : -1;
    ValueArray  values      = frame->argv( 2 );

    frame->setResult( a->splice( startIndex, deleteCount, values ) );
}

void ArrayClosure::slice( Frame* frame )
{
    Array* a = cast_to<Array>( frame->instance() );
    assert(a);

    int startIndex  = frame->nargs() > 0 ? frame->arg( 0 ).asInt() : 0;
    int endIndex    = frame->nargs() > 1 ? frame->arg( 1 ).asInt() : -1;

    frame->setResult( a->slice( startIndex, endIndex ) );
}

// remove the first item of array
void ArrayClosure::shift( Frame* frame )
{
    Array* a = cast_to<Array>( frame->instance() );
    assert(a);

    frame->setResult( a->shift() );
}

// remove the first item of array
void ArrayClosure::unshift( Frame* frame )
{
    Array* a = cast_to<Array>( frame->instance() );
    assert(a);

    if( frame->nargs() == 0 ) {
        return;
    }

    frame->setResult( a->unshift(frame->arg(0)) );
}

// public concat([value:Object]) : Array
// Concatenates the elements specified in the parameters with the elements
// in an array and creates a new array.
// If the value parameters specify an array, the elements of that array are concatenated,
// rather than the array itself. The array my_array is left unchanged.
void ArrayClosure::concat( Frame* frame )
{
    Array* a = cast_to<Array>( frame->instance() );
    assert(a);

    if( frame->nargs() == 0 ) {
        return;
    }

    frame->setResult( a->concat( frame->argv(0) ) );
}

void ArrayClosure::join( Frame* frame )
{
    Array* a = cast_to<Array>( frame->instance() );
    assert(a);

    Str sep = frame->nargs() > 0 ? frame->arg(0).asString() : ",";

    frame->setResult( a->join( sep ) );
}

void ArrayClosure::reverse( Frame* frame )
{
    Array* a = cast_to<Array>( frame->instance() );
    assert(a);

    frame->setResult( a->reverse() );
}

};
