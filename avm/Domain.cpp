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

#include "Domain.h"

#include "Class.h"
#include "Array.h"
#include "Error.h"
#include "Linker.h"

#define NativeClosure( function, nargs )    new FunctionNative( this, function, nargs )
#define Readonly( function )                Value( NativeClosure( function, 0 ), Value() )
#define Property( getter, setter )          Value( NativeClosure( getter, 0 ), NativeClosure( setter, 1 ) )

namespace avm2 {

void as_global_trace( Frame* frame );

// ------------------------------------------------ Domain ------------------------------------------------ //

// ** Domain::Domain
Domain::Domain( void )
{
    m_global = new GlobalObject( this );
}

// ** Domain::name
Name* Domain::name( int index ) const
{
    return m_names[index].get();
}

// ** Domain::setNames
void Domain::setNames( const Names& value )
{
    m_names = value;
}

// ** Domain::setStrings
void Domain::setStrings( const Strings& value )
{
    m_strings = value;
}

// ** Domain::setFunctions
void Domain::setFunctions( const FunctionScripts& value )
{
    m_functions = value;
}

// ** Domain::global
GlobalObject* Domain::global( void ) const
{
    return m_global.get();
}

// ** Domain::registerClass
Class* Domain::registerClass( TypeId typeId, const Str& className, const Str& superClass, CreateInstanceThunk createInstance, FunctionNative* init )
{
    QName* name = findQName( className, "" );
    if( !name ) {
        name = internQName( className, "" );
    }

    Class* sup  = superClass != "" ? findClass( superClass ) : NULL;
    Class* cls  = new Class( this, typeId, sup, name, 0, createInstance, init );

    m_global->setProperty( name, cls );

    return cls;
}

// ** Domain::registerClass
Class* Domain::registerClass( const QName* name, Class* cls )
{
    Value value;

    if( m_global->resolveProperty( name, &value ) ) {
        assert( false );
        return NULL;
    }

    m_global->setProperty( name->qualifiedName(), cls );
    return cls;
}

// ** Domain::registerPackages
void Domain::registerPackages( void )
{
#if !AVM2_INTERNAL_BUILTIN
    AbcInfo* builtin = new AbcInfo( m_player.get_ptr() );
    stream*  in      = new stream( new tu_file( "builtin.abc", "rb" ) );
    builtin->read( in, NULL );

    Linker linker( this, builtin, m_player.get_ptr() );
    linker.link();
#else

    {
        Class* cls = registerClass( TypeObject, "Object", "", ObjectClosure::newOp );
    }

    {
        Class* cls = registerClass( TypeFunction, "Function", "Object", FunctionScriptClosure::newOp );
        cls->addBuiltIn( "call", NativeClosure( FunctionScriptClosure::call, 2 ) );
        cls->addBuiltIn( "apply", NativeClosure( FunctionScriptClosure::apply, 1 ) );
        cls->addBuiltIn( "length", Readonly( FunctionScriptClosure::length ) );
    }

    {
        Class* cls = findClass( "Object" );
        cls->addBuiltIn( "hasOwnProperty", NativeClosure( ObjectClosure::hasOwnProperty, 1 ) );
        cls->addBuiltIn( "toString", NativeClosure( ObjectClosure::toString, 0 ) );
        cls->addBuiltIn( "prototype", Readonly( ObjectClosure::prototype ) );
    }

    {
        Class* cls = registerClass( TypeNumber, "Number", "Object", NumberClosure::newOp, NativeClosure( NumberClosure::init, 1 ) );
    }

    {
        Class* cls = registerClass( TypeBoolean, "Boolean", "Object", BooleanClosure::newOp, NativeClosure( BooleanClosure::init, 1 ) );
    }

    {
        Class* cls = registerClass( TypeInt, "int", "Object", IntClosure::newOp, NativeClosure( IntClosure::init, 1 ) );
    }

    {
        Class* cls = registerClass( TypeVoid, "void", "" );
    }


    {
        Class* cls = registerClass( TypeUInt, "uint", "Object", UIntClosure::newOp, NativeClosure( UIntClosure::init, 1 ) );
    }

    {
        Class* cls = registerClass( TypeObject, "Math", "" );
        cls->set_member( "PI", M_PI );
        cls->set_member( "pow", NativeClosure( MathClosure::pow, 2 ) );
        cls->set_member( "cos", NativeClosure( MathClosure::cos, 1 ) );
        cls->set_member( "sin", NativeClosure( MathClosure::sin, 1 ) );
        cls->set_member( "sqrt", NativeClosure( MathClosure::sqrt, 1 ) );
        cls->set_member( "round", NativeClosure( MathClosure::round, 1 ) );
    }

    {
        Class* cls = registerClass( TypeClass, "Class", "Object" );
    }

    {
        Class* cls = registerClass( TypeObject, "Date", "Object" );
    }

    {
        Class* cls = registerClass( TypeObject, "Dictionary", "Object", ObjectClosure::newOp );
    }

    {
        Class* cls = registerClass( TypeObject, "RegExp", "Object" );
    }

    {
        Class* cls = registerClass( TypeObject, "Vector", "Object", ObjectClosure::newOp );
    }

    {
        Class* cls = registerClass( TypeObject, "XML", "Object", XMLClosure::newOp, NativeClosure( XMLClosure::init, 1 ) );
        cls->addBuiltIn( "text", NativeClosure( XMLClosure::text, 0 ) );
    }

    {
        Class* cls = registerClass( TypeString, "String", "Object", StringClosure::newOp, NativeClosure( StringClosure::init, 1 ) );
        cls->addBuiltIn( "split", NativeClosure( StringClosure::split, 2 ) );
        cls->addBuiltIn( "slice", NativeClosure( StringClosure::slice, 2 ) );
        cls->addBuiltIn( "substr", NativeClosure( StringClosure::substr, 2 ) );
        cls->addBuiltIn( "substring", NativeClosure( StringClosure::substring, 2 ) );
        cls->addBuiltIn( "concat", NativeClosure( StringClosure::concat, 1 ) );
        cls->addBuiltIn( "charAt", NativeClosure( StringClosure::charAt, 1 ) );
        cls->addBuiltIn( "charCodeAt", NativeClosure( StringClosure::charCodeAt, 1 ) );
        cls->addBuiltIn( "indexOf", NativeClosure( StringClosure::indexOf, 1 ) );
        cls->addBuiltIn( "lastIndexOf", NativeClosure( StringClosure::lastIndexOf, 1 ) );
        cls->addBuiltIn( "length", Readonly( StringClosure::length ) );
        cls->addBuiltIn( "toUpperCase", NativeClosure( StringClosure::toUpperCase, 0 ) );
        cls->addBuiltIn( "toLowerCase", NativeClosure( StringClosure::toLowerCase, 0 ) );
        cls->addBuiltIn( "lastIndexOf", NativeClosure( StringClosure::lastIndexOf, 1 ) );
        cls->set_member( "fromCharCode", NativeClosure( StringClosure::fromCharCode, 1 ) );
    }

    {
        Class* cls = registerClass( TypeArray, "Array", "Object", ArrayClosure::newOp, NativeClosure( ArrayClosure::init, 1 ) );
        cls->addBuiltIn( "push", NativeClosure( ArrayClosure::push, 1 ) );
        cls->addBuiltIn( "pop", NativeClosure( ArrayClosure::pop, 0 ) );
        cls->addBuiltIn( "splice", NativeClosure( ArrayClosure::splice, 3 ) );
        cls->addBuiltIn( "slice", NativeClosure( ArrayClosure::slice, 2 ) );
        cls->addBuiltIn( "shift", NativeClosure( ArrayClosure::shift, 0 ) );
        cls->addBuiltIn( "unshift", NativeClosure( ArrayClosure::unshift, 0 ) );
        cls->addBuiltIn( "concat", NativeClosure( ArrayClosure::concat, 1 ) );
        cls->addBuiltIn( "every", NativeClosure( ArrayClosure::every, 2 ) );
        cls->addBuiltIn( "some", NativeClosure( ArrayClosure::some, 2 ) );
        cls->addBuiltIn( "sort", NativeClosure( ArrayClosure::sort, 1 ) );
        cls->addBuiltIn( "sortOn", NativeClosure( ArrayClosure::sortOn, 2 ) );
        cls->addBuiltIn( "forEach", NativeClosure( ArrayClosure::forEach, 2 ) );
        cls->addBuiltIn( "filter", NativeClosure( ArrayClosure::filter, 2 ) );
        cls->addBuiltIn( "map", NativeClosure( ArrayClosure::map, 2 ) );
        cls->addBuiltIn( "indexOf", NativeClosure( ArrayClosure::indexOf, 1 ) );
        cls->addBuiltIn( "lastIndexOf", NativeClosure( ArrayClosure::lastIndexOf, 1 ) );
        cls->addBuiltIn( "join", NativeClosure( ArrayClosure::join, 1 ) );
        cls->addBuiltIn( "reverse", NativeClosure( ArrayClosure::reverse, 0 ) );
        cls->addBuiltIn( "length", Readonly( ArrayClosure::length ) );
        cls->set_member( "CASEINSENSITIVE", Array::CaseInsensitive );
        cls->set_member( "DESCENDING", Array::Descending );
        cls->set_member( "UNIQUESORT", Array::UniqueSort );
        cls->set_member( "RETURNINDEXEDARRAY", Array::ReturnIndexedArray );
        cls->set_member( "NUMERIC", Array::Numeric );
    }

    {
        Class* cls = registerClass( TypeObject, "Error", "Object", ErrorClosure::newOp, NativeClosure( ErrorClosure::init, 2 ) );
        cls->addBuiltIn( "getStackTrace", NativeClosure( ErrorClosure::getStackTrace, 0 ) );
        cls->addBuiltIn( "errorID", Readonly( ErrorClosure::errorId ) );
        cls->addBuiltIn( "message", Property( ErrorClosure::message, ErrorClosure::setMessage ) );
        cls->addBuiltIn( "name", Property( ErrorClosure::name, ErrorClosure::setName ) );
    }

    {
        Class* cls = registerClass( TypeObject, "EvalError", "Error", ErrorClosure::newOp );
    }

    {
        Class* cls = registerClass( TypeObject, "RangeError", "Error", ErrorClosure::newOp );
    }

    {
        Class* cls = registerClass( TypeObject, "SyntaxError", "Error", ErrorClosure::newOp );
    }

    {
        Class* cls = registerClass( TypeObject, "TypeError", "Error", ErrorClosure::newOp );
    }

    {
        Class* cls = registerClass( TypeObject, "URIError", "Error", ErrorClosure::newOp );
    }

    m_global->setProperty( "undefined", Value::undefined );
    m_global->setProperty( "NaN", Value::NaN() );
    m_global->setProperty( "Infinity", Value::Infinity() );
    m_global->setProperty( "trace", NativeClosure( as_global_trace, 0 ) );
    m_global->setProperty( "print", NativeClosure( as_global_trace, 0 ) );
#endif
}

// ** Domain::findClass
Class* Domain::findClass( const Str& name, bool initialize ) const
{
    if( m_global == NULL ) {
        return NULL;
    }
    
    Value value;

    if( m_global->resolveProperty( name, &value ) ) {
        return cast_to<Class>( value.asObject() );
    }

    return NULL;
}

// ** Domain::findQName
QName* Domain::findQName( const Str& name, const Str& ns ) const
{
    for( int i = 1, n = ( int )m_names.size(); i < n; i++ ) {
        const QName* qname = m_names[i]->isQName();

        if( !qname ) {
            continue;
        }

        if( qname->name() == name && qname->ns()->name() == ns ) {
            return const_cast<QName*>( qname );
        }
    }

    return NULL;
}

// ** Domain::internQName
QName* Domain::internQName( const Str& name, const Str& ns )
{
    QName* qname = new QName( name, new Namespace( Namespace::Package, ns ) );
    m_nameCache.push_back( qname );

    return qname;
}

#if 0

// ------------------------------------------- FlashDomain ------------------------------------------------ //

// ** FlashDomain::FlashDomain
FlashDomain::FlashDomain( player* player ) : Domain( player )
{

}

// ** FlashDomain::registerPackages
void FlashDomain::registerPackages( void )
{
    Domain::registerPackages();

    registerEvents();
    registerDisplay();
}

// ** FlashDomain::registerEvents
void FlashDomain::registerEvents( void )
{
    Package* package = resolvePackage( "flash.events" );

    {
        Class* cls = registerClass( package, "EventDispatcher", "Object", EventDispatcherClosure::newOp );
        cls->addBuiltIn( "addEventListener", NativeClosure( EventDispatcherClosure::addEventListener ) );
        cls->addBuiltIn( "dispatchEvent", NativeClosure( EventDispatcherClosure::dispatchEvent ) );
        cls->addBuiltIn( "hasEventListener", NativeClosure( EventDispatcherClosure::hasEventListener ) );
        cls->addBuiltIn( "removeEventListener", NativeClosure( EventDispatcherClosure::removeEventListener ) );
        cls->addBuiltIn( "willTrigger", NativeClosure( EventDispatcherClosure::willTrigger ) );
    }

    {
        Class* cls = registerClass( package, "Event", "Object" );
        cls->set_member( "ADDED", "ADDED" );
        cls->set_member( "ADDED_TO_STAGE", "ADDED_TO_STAGE" );
        cls->set_member( "ENTER_FRAME", "ENTER_FRAME" );
    }
}

// ** FlashDomain::registerDisplay
void FlashDomain::registerDisplay( void )
{
    Package* package = resolvePackage( "flash.display" );

    {
        Class* cls = registerClass( package, "DisplayObject", "flash.events.EventDispatcher" );
    }

    {
        Class* cls = registerClass( package, "InteractiveObject", "flash.display.DisplayObject" );
    }

    {
        Class* cls = registerClass( package, "DisplayObjectContainer", "flash.display.InteractiveObject" );
    }

    {
        Class* cls = registerClass( package, "Sprite", "flash.display.DisplayObjectContainer" );
    }

    {
        Class* cls = registerClass( package, "MovieClip", "flash.display.Sprite" );
    }
}

#endif

}