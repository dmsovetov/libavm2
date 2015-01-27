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

#include "Class.h"

#include "Avm.h"
#include "Function.h"
#include "Trait.h"
#include "Multiname.h"

namespace avm2
{

// ** Class::Class
Class::Class( Domain* domain, TypeId typeId, Class* superClass, QName* name, Uint8 flags, CreateInstanceThunk createInstance, Function* init, Function* staticInit )
    : Function( domain ), m_name( name ), m_flags( flags ), m_superClass( superClass ), m_createInstance( createInstance ), m_init( init ), m_staticInit( staticInit ), m_isInitialized( false ), m_typeId( typeId )
{
    m_class  = this;

    if( init )       init->setName( qualifiedName( "::" ) + "()" );
    if( staticInit ) staticInit->setName( qualifiedName( "::" ) + "$cinit()" );
}

// ** Class::to_string
const char* Class::to_string()
{
    static char str[64];
    sprintf( str, "[class %s]", m_name->name().c_str() );
    return str;
}

// ** Class::execute
void Class::execute( Frame* frame ) const
{
    const Str& className = name();

    if( className == "uint" || className == "Boolean" || className == "Object" || className == "int" || className == "Number" || className == "String" || frame->instance() == NULL ) { // !!!: This is not OK
        Value newInstance = createInstance();
        construct( newInstance, *frame->args() );
        frame->setResult( newInstance );
    }
}

// ** Class::typeId
TypeId Class::typeId( void ) const
{
    return m_typeId;
}

// ** Class::name
const Str& Class::name( void ) const
{
    return m_name->name();
}

// ** Class::qualifiedName
Str Class::qualifiedName( const Str& separator ) const
{
    const Str& name = m_name->name();
    const Str& ns   = m_name->ns()->name();

    return ns == "" ? name : ns + separator + name;
}

// ** Class::createInstance
Value Class::createInstance( void ) const
{
    Class* thiz = const_cast<Class*>( this );

    assert( m_createInstance );
    Object* instance = m_createInstance( thiz->m_domain );
    instance->setType( thiz );
    instance->setTraits( m_instanceTraits.get() );

    return instance;
}

// ** Class::superClass
Class* Class::superClass( void ) const
{
    return m_superClass.get();
}

// ** Class::classTraits
Traits* Class::classTraits( void ) const
{
    return m_classTraits.get();
}

// ** Class::setClassTraits
void Class::setClassTraits( Traits* value )
{
    assert( m_classTraits == NULL );

    m_classTraits = value;
}

// ** Class::instanceTraits
Traits* Class::instanceTraits( void ) const
{
    return m_instanceTraits.get();
}

// ** Class::addInstanceTrait
void Class::setInstanceTraits( Traits* value )
{
    assert( m_instanceTraits == NULL );

    m_instanceTraits = value;
}

// ** Class::initialize
void Class::initialize( void )
{
    if( m_isInitialized ) {
        return;
    }

    m_isInitialized = true;

    // ** Initialize super class
    if( m_superClass != NULL ) {
        m_superClass->initialize();
    }

    if( m_createInstance == NULL ) {
        m_createInstance = m_superClass != NULL ? m_superClass->m_createInstance : ObjectClosure::newOp;
    }

    IF_VERBOSE_PARSE(logger::msg("Initializing class %s\n", name().c_str()));

    // ** Initialize class
    if( m_superClass != NULL && m_init ) {
        if( m_instanceTraits != NULL ) {
            m_instanceTraits->setSuper( m_superClass->m_instanceTraits.get() );
        }

        if( FunctionScript* script = cast_to<FunctionScript>( m_init ) ) {
            script->setSuper( m_superClass.get() );
        }
    }

    if( m_classTraits != NULL ) {
        m_classTraits->assignSlots();
        setTraits( m_classTraits.get() );
    }

    if( m_instanceTraits != NULL ) {
        m_instanceTraits->assignSlots();
    }

    // ** Run static initializer
    if( m_staticInit ) {
        m_staticInit->call( this );
    }
}

// ** Class::construct
void Class::construct( const Value& instance, const Arguments& args, Frame* parentFrame ) const
{
    Object* object = instance.asObject();
    if( !object ) {
        assert( false );
        return;
    }

    // ** Invoke constructor
    if( m_init ) {
        m_init->executeWithInstance( object, args, parentFrame );
    }
}

// ** Class::findBuiltIn
bool Class::findBuiltIn( const Str& name, Value* value ) const
{
    if( m_builtIn.get( name, value ) ) {
        return true;
    }

    return m_superClass != NULL ? m_superClass->findBuiltIn( name, value ) : NULL;
}

// ** Class::addBuiltIn
void Class::addBuiltIn( const Str& name, const Value& value )
{
    m_builtIn[name] = value;
}

// ** Class::is
bool Class::is( const Class* type ) const
{
    if( type == this ) {
        return true;
    }

    return m_superClass != NULL ? m_superClass->is( type ) : false;
}

// ** Class::isSealed
bool Class::isSealed( void ) const
{
    return m_flags & Sealed ? true : false;
}

}
