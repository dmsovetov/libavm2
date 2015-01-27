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

#include "Trait.h"

#include "Object.h"
#include "Multiname.h"
#include "Class.h"

namespace avm2
{

// ---------------------------------------------- Traits ----------------------------------------------- //

// ** Traits::addTrait
void Traits::addTrait( const Str& owner, const QName* name, const ::avm2::Class* valueType, const Value& value, Type type, int slot, Uint8 attr )
{
    assert( name );

    const Namespace* ns     = name->ns();

    if( type == Setter || type == Getter ) {
        addProperty( owner, name, value, type, slot, attr );
        return;
    }

    Trait trait;
    trait.m_name   = name;
    trait.m_value  = value.isUndefined() ? defaultValueForType( valueType ) : value;
    trait.m_type   = type;
    trait.m_slot   = slot;
    trait.m_attr   = attr;

    if( ::avm2::Function* function = value.asFunction() ) {
        function->setName( owner + "/" + name->name() + "()" );
    }

    m_traits.set( name->qualifiedName(), trait );
}

// ** Traits::mergeTraits
void Traits::mergeTraits( const Str& ns, const Traits* traits )
{
    if( traits == NULL ) {
        return;
    }
    
    for( TraitRegistry::const_iterator i = traits->m_traits.begin(), end = traits->m_traits.end(); i != end; ++i ) {
        m_traits.set( ns + "." + i->second.m_name->name(), i->second );
    }
}

// ** Traits::defaultValueForType
Value Traits::defaultValueForType( const ::avm2::Class* type ) const
{
    if( type == NULL ) {
        return Value::undefined;
    }

    Value NaN;
    NaN.setNaN();

    if( type->name() == "Boolean" ) return false;
    if( type->name() == "int" )     return 0;
    if( type->name() == "Number" )  return NaN;

    return Value::null;
}

// ** Traits::addProperty
void Traits::addProperty( const Str& owner, const QName* name, const Value& value, Type type, int slot, Uint8 attr )
{
    Trait trait;

    if( !m_traits.get( name->name(), &trait ) ) {
        trait.m_value  = Value( Value::undefined, Value::undefined );
        trait.m_type   = type;
        trait.m_slot   = slot;
        trait.m_attr   = attr;
        m_traits.set( name->qualifiedName(), trait );
    }

    switch( type ) {
    case Setter:    {
                        ::avm2::Function* setter = value.asFunction();
                        trait.m_value.asProperty()->setSetter( setter );
                        setter->setName( owner + "/set " + name->name() + "()" );
                    }
                    break;

    case Getter:    {
                        ::avm2::Function* getter = value.asFunction();
                        trait.m_value.asProperty()->setGetter( getter );
                        getter->setName( owner + "/get " + name->name() + "()" );
                    }
                    break;
            
    default:        assert( false );
    }
}

// ** Traits::setSlots
void Traits::setSlots( ValueArray& slots ) const
{
    assert( slots.size() > slotCount() );

    if( m_super != NULL ) {
        m_super->setSlots( slots );
    }

    for( TraitRegistry::const_iterator i = m_traits.begin(), end = m_traits.end(); i != end; ++i ) {
        const Trait& trait = i->second;
        assert( trait.m_slot > 0 );
        slots[trait.m_slot] = i->second.m_value;
    }
}

// ** Traits::slotCount
int Traits::slotCount( void ) const
{
    return ( int )m_traits.size() + (m_super != NULL ? m_super->slotCount() : 0);
}

// ** Traits::setSuper
void Traits::setSuper( Traits* value )
{
    assert( m_super == NULL );
    m_super = value;

    if( m_super == NULL ) {
        return;
    }

    for( TraitRegistry::const_iterator i = m_traits.begin(), end = m_traits.end(); i != end; ++i ) {
        Trait        super;
        const Trait& trait = i->second;

        switch( trait.m_type ) {
        case Method:    {
                            FunctionScript* function = cast_to<FunctionScript>( trait.m_value.asFunction() );
                            if( function && m_super->findTrait( i->first, trait.m_type, super ) ) {
                                function->setSuper( super.m_value.asFunction() );
                            }
                        }
                        break;

        case Getter:    {
                            FunctionScript* function = cast_to<FunctionScript>( trait.m_value.asProperty()->m_getter );

                            if( !m_super->findTrait( i->first, trait.m_type, super ) ) {
                                continue;
                            }

                            function->setSuper( super.m_value.asProperty()->m_getter );
                        }
                        break;

        case Setter:    {
                            FunctionScript* function = cast_to<FunctionScript>( trait.m_value.asProperty()->m_setter );

                            if( !m_super->findTrait( i->first, trait.m_type, super ) ) {
                                continue;
                            }

                            function->setSuper( super.m_value.asProperty()->m_setter );
                        }
                        break;

        default:        break;
        }
    }
}

// ** Traits::assignSlots
void Traits::assignSlots( void )
{
    int idx   = m_super != NULL ? (m_super->slotCount() + 1) : 1;

    for( TraitRegistry::iterator i = m_traits.begin(), end = m_traits.end(); i != end; ++i ) {
        Trait& trait = i->second;

        while( trait.m_slot == 0 ) {
            trait.m_slot = isSlotFree( idx ) ? idx : 0;
            idx++;
        }
    }
}

// ** Traits::isSlotFree
bool Traits::isSlotFree( int idx ) const
{
    for( TraitRegistry::const_iterator i = m_traits.begin(), end = m_traits.end(); i != end; ++i ) {
        if( i->second.m_slot == idx ) {
            return false;
        }
    }

    return true;
}

// ** Traits::resolveSlot
bool Traits::resolveSlot( const Str& name, TraitAccess access, int& slot ) const
{
    Trait trait;

    // ** Find trait by name
    if( m_traits.get( name, &trait ) == false ) {
        return m_super != NULL ? m_super->resolveSlot( name, access, slot ) : false;
    }

    slot = trait.m_slot;

    return true;
}

// ** Traits::findTrait
bool Traits::findTrait( const Str& name, Type type, Trait& trait ) const
{
    if( m_traits.get( name, &trait ) && trait.m_type == type ) {
        return true;
    }

    return m_super != NULL ? m_super->findTrait( name, type, trait ) : false;
}

} // namespace avm2