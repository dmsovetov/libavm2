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

#include "Multiname.h"

namespace avm2
{

// ------------------------------------------------ Namespace ------------------------------------------------ //

Str Namespace::AS3 = "http://adobe.com/AS3/2006/builtin";
Str Namespace::XML = "http://www.w3.org/XML/1998/namespace";

// ** Namespace::Namespace
Namespace::Namespace( Kind kind, const Str& name ) : m_kind( kind ), m_name( name )
{
    if( m_name == AS3 ) m_name = "";    // !!!: AS3 namespace workaround
}

// ** Namespace::name
const Str& Namespace::name( void ) const
{
    return m_name;
}

// ** Namespace::uri
Str Namespace::uri( void ) const
{
    const Str& n = name();

    switch( m_kind ) {
//    case Package:           return n + ":package";
    case PackageInternal:   return n + ":packageInternal";
    case Private:           return n + ":private";
    case Protected:         return n + ":protected";
    case StaticProtected:   return n + ":staticProtected";
    default:                break;
    }

    return n;
}

// ** Namespace::kind
Namespace::Kind Namespace::kind( void ) const
{
    return m_kind;
}

// ------------------------------------------------ Name ------------------------------------------------ //

// ** Name::Name
Name::Name( const Str& name ) : m_name( name )
{

}

// ** Name::name
const Str& Name::name( void ) const
{
    return m_name;
}

// ** Name::setName
void Name::setName( const Str& value )
{
    assert( hasRuntimeName() );
    m_name = value;
}

// ** Name::hasRuntimeName
bool Name::hasRuntimeName( void ) const
{
    return false;
}

// ** Name::hasRuntimeNamespace
bool Name::hasRuntimeNamespace( void ) const
{
    return false;
}

// ** Name::isQName
const QName* Name::isQName( void ) const
{
    return NULL;
}

// ** Name::isMultiname
const Multiname* Name::isMultiname( void ) const
{
    return NULL;
}

// ** Name::isTypename
const Typename* Name::isTypename( void ) const
{
    return NULL;
}

// ------------------------------------------------- RTQName ------------------------------------------------- //

// ** RTQName::RTQName
RTQName::RTQName( Namespace* ns ) : QName( "", ns )
{

}

// ** RTQName::hasRuntimeNamespace
bool RTQName::hasRuntimeNamespace( void ) const
{
    return true;
}

// ** RTQName::setName
void RTQName::setName( const Str& value ) const
{
    const_cast<RTQName*>( this )->m_name = value;
}

// ------------------------------------------------- RTQNameL ------------------------------------------------- //

// ** RTQNameL::RTQNameL
RTQNameL::RTQNameL( void ) : RTQName( NULL )
{

}

// ** RTQNameL::hasRuntimeName
bool RTQNameL::hasRuntimeName( void ) const
{
    return true;
}

// ** RTQNameL::hasRuntimeNamespace
bool RTQNameL::hasRuntimeNamespace( void ) const
{
    return true;
}

// ** RTQNameL::setNamespace
void RTQNameL::setNamespace( Namespace* value ) const
{
    const_cast<RTQNameL*>( this )->m_namespace = value;
}

// ------------------------------------------------ QName ------------------------------------------------ //

// ** QName::QName
QName::QName( const Str& name, Namespace* ns ) : Name( name ), m_namespace( ns )
{

}

// ** QName::qualifiedName
Str QName::qualifiedName( void ) const
{
    assert( m_namespace );

    const Str& ns = m_namespace->uri();
    return ns == "" ? m_name : ns + "." + m_name;
}

// ** QName::isQName
const QName* QName::isQName( void ) const
{
    return this;
}

// ** QName::ns
const Namespace* QName::ns( void ) const
{
    return m_namespace;
}

// ---------------------------------------------------- Multiname -------------------------------------------------- //

// ** Multiname::Multiname
Multiname::Multiname( const Str& name, const Namespaces& namespaces ) : Name( name ), m_namespaces( namespaces )
{

}

// ** Multiname::isMultiname
const Multiname* Multiname::isMultiname( void ) const
{
    return this;
}

// ** Multiname::namespaces
const Namespaces& Multiname::namespaces( void ) const
{
    return m_namespaces;
}

// ** Multiname::count
int Multiname::count( void ) const
{
    return ( int )m_namespaces.size();
}

// ** Multiname::qualifiedName
Str Multiname::qualifiedName( int index ) const
{
    assert( index >= 0 && index < count() );
    const Namespace* ns  = m_namespaces[index];
    const Str&       uri = ns->uri();

    return uri == "" ? m_name : uri + "." + m_name;
}

// -------------------------------------------------- MultinameLate ------------------------------------------------ //

// ** MultinameL::MultinameL
MultinameL::MultinameL( const Namespaces& namespaces ) : Multiname( "", namespaces )
{
    
}

// ** MultinameL::hasRuntimeName
bool MultinameL::hasRuntimeName( void ) const
{
    return true;
}

// ---------------------------------------------------- Typename -------------------------------------------------- //

// ** Typename::Typename
Typename::Typename( const QName* type, const QName* arg ) : Name( "" ), m_type( type ), m_arg( arg )
{

}

// ** Typename::type
const QName* Typename::type( void ) const
{
    return m_type;
}

// ** Typename::isTypename
const Typename* Typename::isTypename( void ) const
{
    return this;
}

} // namespace avm2