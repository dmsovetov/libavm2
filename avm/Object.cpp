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

#include "Object.h"

#include "Class.h"
#include "Array.h"
#include "Function.h"
#include "Domain.h"

namespace avm2
{

// ** Object::Object
Object::Object( Domain* domain ) : m_domain( domain ), m_isInsideToString( false )
{
    assert( m_domain );
    m_class = m_domain->findClass( "Object" );
}

Object::~Object()
{

}

// ** Object::domain
Domain* Object::domain( void ) const
{
    return m_domain;
}

// ** Object::type
Class* Object::type( void ) const
{
    return m_class.get();
}

// ** Object::setType
void Object::setType( Class* value )
{
    m_class = value;
}

// called from a object constructor only
void Object::builtin_member( const Str& name, const Value& val )
{
    m_members.set(name, val);
}

// ** Object::setMember
bool Object::set_member( const Str& name, const Value& value )
{
    Value previous;

    // ** Try to set a property
    if( Object::get_member( name, &previous ) ) {
        if( Property* property = previous.asProperty() ) {
            property->set( this, value );
            return true;
        }
    }

    // ** Set object member
    m_members.set( name, value );

    return true;
}

// ** Object::resolveProperty
bool Object::resolveProperty( const Str& name, Value* value ) const
{
    if( m_traits != NULL ) {
        int  idx    = -1;

        if( m_traits->resolveSlot( name, TraitRead, idx ) ) {
            if( value ) *value = m_slots[idx];
            return true;
        }

    //    return false;
    }

    return const_cast<Object*>( this )->get_member( name, value );
}

// ** Object::resolveProperty
bool Object::resolveProperty( const Name* name, Value* value ) const
{
    if( const Multiname* mname = name->isMultiname() ) {
        for( int i = 0, n = mname->count(); i < n; i++ ) {
            if( resolveProperty( mname->qualifiedName( i ), value ) ) {
                return true;
            }
        }
    }
    else {
        return resolveProperty( name->isQName()->qualifiedName(), value );
    }

    return false;
}

// ** Object::setProperty
bool Object::setProperty( const Str& name, const Value& value )
{
    if( m_traits != NULL ) {
        int  idx    = -1;

        if( m_traits->resolveSlot( name, TraitWrite, idx ) ) {
            setSlot( idx, value );
            return true;
        }

        if( m_class != NULL && m_class->isSealed() ) {
            return false;
        }
    }

    if( m_class != NULL && m_class->isSealed() ) {
        return false;
    }

    return set_member( name, value );
}

// ** Object::setProperty
bool Object::setProperty( const Name* name, const Value& value )
{
    if( const Multiname* mname = name->isMultiname() ) {
        for( int i = 0, n = mname->count(); i < n; i++ ) {
            if( setProperty( mname->qualifiedName( i ), value ) ) {
                return true;
            }
        }
    } else {
        return setProperty( name->isQName()->qualifiedName(), value );
    }

    return false;
}

// ** Object::getMember
bool Object::get_member( const Str& name, Value* value )
{
    if( m_members.get( name, value ) == false ) {
        return m_class != NULL ? m_class->findBuiltIn( name, value ) : false;
    }

    return true;
}

// ** Object::createIterator
IteratorPtr Object::createIterator( const Value& value )
{
    return new ObjectIterator( &m_members, value );
}

// ** Object::nextKey
Value Object::nextKey( const Value& key ) const
{
    Members::const_iterator it = m_members.find( key.asString() );
    return it != m_members.end() ? it->first.c_str() : "";
}

// ** Object::getPropertyByKey
Value Object::getPropertyByKey( const Value& key ) const
{
    Members::const_iterator it = m_members.find( key.asString() );
    return it != m_members.end() ? it->second : Value::undefined;
}

// ** Object::deletePropertyByName
bool Object::deletePropertyByName( const Str& name )
{
    if( m_class != NULL && m_class->isSealed() ) {
        return false;
    }

    Members::iterator it = m_members.find( name );
    if( it == m_members.end() ) {
        return false;
    }

    m_members.set( name, NULL );
    return true;
}

// ** Object::valueOf
Value Object::valueOf( void ) const
{
    Value function;

    if( resolveProperty( "valueOf", &function ) ) {
        return function.asFunction()->call( this );
    }

    return new String( m_domain, const_cast<Object*>( this )->to_string() );
}

// ** Object::asString
const char* Object::to_string( void )
{
    Value toString;
    static char str[64];

    if( resolveProperty( "toString", &toString ) && toString.isFunction() && !m_isInsideToString ) {
        m_isInsideToString = true;
        Value result = toString.asFunction()->call( this );
        m_isInsideToString = false;
        sprintf( str, "%s", result.asCString() );
    } else {
        sprintf( str, "[object %s]", (m_class != NULL) ? m_class->name().c_str() : "Object" );
    }

    return str;
}

// ** Object::enclose
FunctionClosure* Object::enclose( Function* function ) const
{
    FunctionClosureWeak cached;

    // ** Search for a cached closure
    if( m_closures.get( function, &cached ) && cached != NULL ) {
        return cached.get_ptr();
    }

    // ** Create a new function closure
    FunctionClosure* closure = new FunctionClosure( m_domain, const_cast<Object*>( this ), function );
    m_closures[function] = closure;

    return closure;
}

// ** Object::setSlot
void Object::setSlot( int index, const Value& value )
{
    if( index >= m_slots.size() ) {
        m_slots.resize( index + 1 );
    }

    assert( index >= 0 && index < m_slots.size() );

    if( Property* property = m_slots[index].asProperty() ) {
        property->set( this, value );
    } else {
        m_slots[index] = value;
    }
}

// ** Object::slot
const Value& Object::slot( int index ) const
{
    assert( index >= 0 && index < m_slots.size() );
    return m_slots[index];
}

// ** Object::setTraits
void Object::setTraits( const Traits* value )
{
    m_traits = const_cast<Traits*>( value );

    if( m_traits != NULL ) {
        m_slots.resize( m_traits->slotCount() + 1 );
        m_traits->setSlots( m_slots );
    }
}

// ** Object::traits
const Traits* Object::traits( void ) const
{
    return m_traits.get();
}

// ** Object::prototype
Object* Object::prototype( void ) const
{
    if( m_prototype == NULL ) {
        m_prototype = new Object( m_domain );
    }

    return m_prototype.get();
}

// ** Object::hasOwnProperty
bool Object::hasOwnProperty( const Str& name ) const
{
    return resolveProperty( name, NULL );
}

// ** Object::clearReft
void Object::clear_refs( hash<Object*, bool>* visited_objects, Object* this_ptr )
{
    // Is it a reentrance ?
    if (visited_objects->get(this, NULL))
    {
        return;
    }
    visited_objects->set(this, true);

    Value undefined;
    for (string_hash<Value>::iterator it = m_members.begin();
        it != m_members.end(); ++it)
    {
        Object* obj = it->second.asObject();
        if (obj)
        {
            if (obj == this_ptr)
            {
                it->second.setUndefined();
            }
            else
            {
                obj->clear_refs(visited_objects, this_ptr);
            }
            continue;
        }
    }
}

void Object::copy_to(Object* target)
// Copy all members from 'this' to target
{
    assert(false);

    if (target)
    {
        for (string_hash<Value>::const_iterator it = m_members.begin();
            it != m_members.end(); ++it ) 
        { 
            target->set_member(it->first, it->second); 
        } 
    }
}

Object*	Object::find_target(const Value& target)
// Find the object referenced by the given target.
{
    assert(false);
    return NULL;
/*
    if (target.isString() == false)
    {
        return target.asObject();
    }

    const tu_string& path = target.asString();
    if (path.length() == 0)
    {
        return this;
    }

    Value val;
    Object* tar = NULL;

    // absolute path ?
    if (*path.c_str() == '/')
    {
        return m_player->get_root_movie()->find_target(path.c_str() + 1);
    }

    const char* slash = strchr(path.c_str(), '/');
    if (slash == NULL)
    {
        slash = strchr(path.c_str(), '.');
        if (slash)
        {
            if (slash[1] == '.')
            {
                slash = NULL;
            }
        }
    }

    if (slash)
    {
        tu_string name(path.c_str(), int(slash - path.c_str()));
        get_member(name, &val);
        tar = val.asObject();
        if (tar)	
        {
            return tar->find_target(slash + 1);
        }
    }
    else
    {
        get_member(path, &val);
        tar = val.asObject();
    }

    if (tar == NULL)
    {
    //	log_error("can't find target %s\n", path.c_str());
    }
    return tar;
*/
}

// mark 'this' as alive
void Object::this_alive()
{
    // Whether there were we here already ?
//    if (m_player != NULL && m_player->is_garbage(this))
//    {
        // 'this' and its members is alive
//        m_player->set_alive(this);
//        for (string_hash<Value>::iterator it = m_members.begin();
//            it != m_members.end(); ++it)
//        {
//            Object* obj = it->second.asObject();
//            if (obj)
//            {
//                obj->this_alive();
//            }
//        }
//    }
}

double	Object::to_number()
{
    const char* str = to_string();
    if (str)
    {
        return atof(str);
    }
    return 0;
}

// ** ObjectIterator::ObjectIterator
ObjectIterator::ObjectIterator( Members* members, const Value& key ) : m_members( members ), m_iterator( m_members->begin() )
{
    if( key.isNumber() ) {
        return;
    }

    m_iterator = m_members->find( key.asCString() );
    next();
}

// ** ObjectIterator::hasNext
bool ObjectIterator::hasNext( void ) const
{
    return m_iterator != m_members->end();
}

// ** ObjectIterator::next
void ObjectIterator::next( void )
{
    ++m_iterator;
}

// ** ObjectIterator::key
Value ObjectIterator::key( void ) const
{
    return m_iterator->first.c_str();
}


// ** XML::XML
XML::XML( Domain* domain ) : Object( domain )
{
    m_class = m_domain->findClass( "XML" );
    assert( m_class != NULL );
}

// ** XML::setText
void XML::setText( const Str& value )
{
    m_text = value;
}

// ** XML::text
const Str& XML::text( void ) const
{
    return m_text;
}

// ** String::String
String::String( Domain* domain, const Str& string ) : Object( domain ), m_string( string )
{
    m_class = m_domain->findClass( "String" );
    assert( m_class != NULL );
}

// ** String::split
Value String::split( const Str& delimiter, int limit )
{
    std::string str = m_string.c_str();
    std::string sep = delimiter.c_str();
    ValueArray  result;

    int index = 0;
    int next  = 0;

    do {
        if( result.size() >= limit && limit >= 0 ) {
            break;
        }

        next = ( int )str.find( sep, index );

        if( next != std::string::npos ) {
            result.push_back( str.substr( index, next - index ).c_str() );
            index = next + ( int )sep.length();
        }
        else if( index < str.length() ) {
            result.push_back( str.substr( index, str.length() - index ).c_str() );
        }
    } while( next != std::string::npos );

    assert( result.size() <= limit || limit < 0 );

    return new Array( m_domain, result );
}

// ** String::charAt
Str String::charAt( int index ) const
{
    Str result;

    if( index >= 0 && index < length() ) {
        result = m_string[index];
    }

    return result;
}

// ** String::charCodeAt
int String::charCodeAt( int index ) const
{
    int code = 0;

    if( index >= 0 && index < length() ) {
        code = m_string[index];
    }

    return code;
}

// ** String::slice
Value String::slice( int start, int end ) const
{
    if( start < 0 ) start = m_string.length() + start;
    if( end   < 0 ) end   = m_string.length() + end;

    std::string str = m_string.c_str();

    return new String( m_domain, str.substr( start, end - start ).c_str() );
}

// ** String::substr
Value String::substr( int start, int length ) const
{
    std::string str = m_string.c_str();
    return new String( m_domain, str.substr( start, length ).c_str() );
}

// ** String::substring
Value String::substring( int start, int end ) const
{
    start = imax( 0, start );
    end   = imin( length(), end );

    std::string str = m_string.c_str();

    return new String( m_domain, str.substr( start, end - start ).c_str() );
}

// ** String::concat
Value String::concat( const ValueArray& strings ) const
{
    Str text = m_string;

    for( int i = 0, n = ( int )strings.size(); i < n; i++ ) {
        text += strings[i].asString();
    }

    return new String( m_domain, text );
}

// ** String::fromCharCode
Value String::fromCharCode( Domain* domain, const ValueArray& codes )
{
    Str text = "";
    for( int i = 0, n = ( int )codes.size(); i < n; i++ ) {
        text += codes[i].asInt();
    }

    return new String( domain, text );
}

// ** String::indexOf
int String::indexOf( const Str& value, int startIndex ) const
{
    startIndex = imax( 0, startIndex );

    std::string str = m_string.c_str();
    return str.find( value.c_str(), startIndex );
}

// ** String::lastIndexOf
int String::lastIndexOf( const Str& value, int startIndex ) const
{
    int end = startIndex < 0 ? length() : startIndex;
    int pos = 0;
    int idx = -1;

    while( pos < end ) {
        int next = indexOf( value, pos );

        if( next == -1 || next > end ) {
            break;
        }

        idx = next;
        pos = next + 1;
    }

    return idx;
}

// ** String::toUpperCase
Value String::toUpperCase( void ) const
{
    std::string str = m_string.c_str();
    std::transform( str.begin(), str.end(), str.begin(), ::toupper );

    return new String( m_domain, str.c_str() );
}

// ** String::toLowerCase
Value String::toLowerCase( void ) const
{
    std::string str = m_string.c_str();
    std::transform( str.begin(), str.end(), str.begin(), ::tolower );

    return new String( m_domain, str.c_str() );
}

// ** Int::Int
Int::Int( Domain* domain ) : Object( domain ), m_value( 0 )
{
    m_class = m_domain->findClass( "int" );
    assert( m_class != NULL );
}

// ** Int::valueOf
Value Int::valueOf( void ) const
{
    return m_value;
}

// ** Int::to_string
const char* Int::to_string( void )
{
    if( m_value == 0 ) {
        return "0";
    }

    static char buffer[50];
    snprintf( buffer, 50, "%d", m_value );
    
    return buffer;
}

// ** Int::value
int Int::value( void ) const
{
    return m_value;
}

// ** Int::setValue
void Int::setValue( int value )
{
    m_value = value;
}

// ** Number::Number
Number::Number( Domain* domain ) : Object( domain ), m_value( 0.0 )
{
    m_class = m_domain->findClass( "Number" );
    assert( m_class != NULL );
}

// ** Number::valueOf
Value Number::valueOf( void ) const
{
    return m_value;
}

// ** Number::to_string
const char* Number::to_string( void )
{
    if( m_value == 0.0 ) {
        return "0";
    }

    static char buffer[50];
    snprintf( buffer, 50, "%.14g", m_value );

    return buffer;
}

// ** Number::value
double Number::value( void ) const
{
    return m_value;
}

// ** Number::setValue
void Number::setValue( double value )
{
    m_value = value;
}

// ** Boolean::Boolean
Boolean::Boolean( Domain* domain ) : Object( domain ), m_value( false )
{
    m_class = m_domain->findClass( "Boolean" );
    assert( m_class != NULL );
}

// ** Boolean::value
bool Boolean::value( void ) const
{
    return m_value;
}

// ** Boolean::setValue
void Boolean::setValue( bool value )
{
    m_value = value;
}

// ** Boolean::valueOf
Value Boolean::valueOf( void ) const
{
    return m_value;
}

// ** Boolean::to_string
const char* Boolean::to_string( void )
{
    return m_value ? "true" : "false";
}

// ** UInt::UInt
UInt::UInt( Domain* domain ) : Object( domain ), m_value( 0 )
{
    m_class = m_domain->findClass( "uint" );
    assert( m_class != NULL );
}

// ** UInt::value
unsigned int UInt::value( void ) const
{
    return m_value;
}

// ** UInt::setValue
void UInt::setValue( unsigned int value )
{
    m_value = value;
}

// ** UInt::valueOf
Value UInt::valueOf( void ) const
{
    return m_value;
}

// ** UInt::to_string
const char* UInt::to_string( void )
{
    if( m_value == 0 ) {
        return "0";
    }

    static char buffer[50];
    snprintf( buffer, 50, "%d", m_value );
    
    return buffer;
}

void IntClosure::init( Frame* frame )
{
    Int* i = cast_to<Int>( frame->instance() );
    assert(i);

    i->setValue( frame->nargs() ? frame->arg( 0 ).asInt() : 0 );
}

void UIntClosure::init( Frame* frame )
{
    UInt* i = cast_to<UInt>( frame->instance() );
    assert(i);

    i->setValue( frame->nargs() ? frame->arg( 0 ).asUInt() : 0 );
}

void BooleanClosure::init( Frame* frame )
{
    Boolean* i = cast_to<Boolean>( frame->instance() );
    assert(i);

    i->setValue( frame->nargs() ? frame->arg( 0 ).asBool() : 0 );
}

void NumberClosure::init( Frame* frame )
{
    Number* n = cast_to<Number>( frame->instance() );
    assert(n);

    n->setValue( frame->nargs() ? frame->arg( 0 ).asNumber() : 0 );
}

void XMLClosure::init( Frame* frame )
{
    XML* s = cast_to<XML>( frame->instance() );
    assert(s);

    String* text = frame->nargs() > 0 ? cast_to<String>( frame->arg(0).asObject() ) : NULL;
    s->setText( text ? text->toString() : "" );
}

void XMLClosure::text( Frame* frame )
{
    XML* s = cast_to<XML>( frame->instance() );
    assert(s);

    frame->setResult( new String( frame->domain(), s->text() ) );
}

void StringClosure::split( Frame* frame )
{
    String* s = cast_to<String>( frame->instance() );
    assert(s);

    String* delimiter = frame->nargs() > 0 ? cast_to<String>( frame->arg(0).asObject() ) : NULL;
    int     limit     = frame->nargs() > 1 ? frame->arg(1).asInt() : -1;

    frame->setResult( s->split( delimiter ? delimiter->toString() : "", limit ) );
}

void StringClosure::slice( Frame* frame )
{
    String* s = cast_to<String>( frame->instance() );
    assert(s);

    int start = frame->nargs() > 0 ? frame->arg(0).asInt() : -1;
    int end   = frame->nargs() > 1 ? frame->arg(1).asInt() :  0;

    frame->setResult( s->slice( start, end ) );
}

void StringClosure::substr( Frame* frame )
{
    String* s = cast_to<String>( frame->instance() );
    assert(s);

    int start  = frame->nargs() > 0 ? frame->arg(0).asInt() : -1;
    int length = frame->nargs() > 1 ? frame->arg(1).asInt() :  0;

    frame->setResult( s->substr( start, length ) );
}

void StringClosure::substring( Frame* frame )
{
    String* s = cast_to<String>( frame->instance() );
    assert(s);

    int start = frame->nargs() > 0 ? frame->arg(0).asInt() : -1;
    int end   = frame->nargs() > 1 ? frame->arg(1).asInt() :  0;

    frame->setResult( s->substring( start, end ) );
}

void StringClosure::concat( Frame* frame )
{
    String* s = cast_to<String>( frame->instance() );
    assert(s);

    frame->setResult( s->concat( frame->argv(0) ) );
}

void StringClosure::init( Frame* frame )
{
    String* s = cast_to<String>( frame->instance() );
    assert(s);

    String* text = frame->nargs() > 0 ? cast_to<String>( frame->arg(0).asObject() ) : NULL;
    s->setString( text ? text->toString() : "" );
}

void StringClosure::length( Frame* frame )
{
    String* s = cast_to<String>( frame->instance() );
    assert(s);

    frame->setResult( s->length() );
}

void StringClosure::charAt( Frame* frame )
{
    String* s = cast_to<String>( frame->instance() );
    assert(s);

    int index = frame->nargs() > 0 ? frame->arg(0).asInt() : -1;

    frame->setResult( new String( s->domain(), s->charAt( index ) ) );
}

void StringClosure::charCodeAt( Frame* frame )
{
    String* s = cast_to<String>( frame->instance() );
    assert(s);

    int index = frame->nargs() > 0 ? frame->arg(0).asInt() : -1;
    int code  = s->charCodeAt( index );

    frame->setResult( code ? code : get_nan() );
}

void StringClosure::fromCharCode( Frame* frame )
{
    frame->setResult( String::fromCharCode( frame->domain(), frame->argv(0) ) );
}

void StringClosure::indexOf( Frame* frame )
{
    String* s = cast_to<String>( frame->instance() );
    assert(s);

    Str str   = frame->nargs() > 0 ? frame->arg(0).asString() : "";
    int index = frame->nargs() > 1 ? frame->arg(1).asInt()    : -1;

    frame->setResult( s->indexOf( str, index ) );
}

void StringClosure::lastIndexOf( Frame* frame )
{
    String* s = cast_to<String>( frame->instance() );
    assert(s);

    Str str   = frame->nargs() > 0 ? frame->arg(0).asString() : "";
    int index = frame->nargs() > 1 ? frame->arg(1).asInt()    : -1;

    frame->setResult( s->lastIndexOf( str, index ) );
}

void StringClosure::toLowerCase( Frame* frame )
{
    String* s = cast_to<String>( frame->instance() );
    assert(s);

    frame->setResult( s->toLowerCase() );
}

void StringClosure::toUpperCase( Frame* frame )
{
    String* s = cast_to<String>( frame->instance() );
    assert(s);

    frame->setResult( s->toUpperCase() );
}

void ObjectClosure::toString( Frame* frame )
{
    Object* o = cast_to<Object>( frame->instance() );
    assert( o );

    frame->setResult( o->to_string() );
}

void ObjectClosure::hasOwnProperty( Frame* frame )
{
    Object* o = cast_to<Object>( frame->instance() );
    assert(o);

    String* name = cast_to<String>( frame->arg( 0 ).asObject() );
    frame->setResult( o->hasOwnProperty( name->to_string() ) );
}

void ObjectClosure::prototype( Frame* frame )
{
    Object* o = cast_to<Object>( frame->instance() );
    assert(o);

    frame->setResult( o->prototype() );
}

void MathClosure::pow( Frame* frame )
{
    frame->setResult( ::pow( frame->arg(0).asNumber(), frame->arg(1).asNumber() ) );
}

void MathClosure::cos( Frame* frame )
{
    frame->setResult( ::cos( frame->arg(0).asNumber() ) );
}

void MathClosure::sin( Frame* frame )
{
    frame->setResult( ::sin( frame->arg(0).asNumber() ) );
}

void MathClosure::sqrt( Frame* frame )
{
    frame->setResult( ::sqrt( frame->arg(0).asNumber() ) );
}

void MathClosure::round( Frame* frame )
{
    frame->setResult( ::round( frame->arg(0).asNumber() ) );
}

}
