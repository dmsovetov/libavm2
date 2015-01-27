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

#ifndef __avm2__Trait__
#define __avm2__Trait__

#include "Common.h"
#include "Value.h"

namespace avm2 {

    // ** class Traits
    class Traits : public ref_counted {
    public:

        // ** enum Type
        enum Type {
            Slot      = 0,
            Method    = 1,
            Getter    = 2,
            Setter    = 3,
            Class     = 4,
            Function  = 5,
            Const     = 6
        };

        // ** enum Attribute
        enum Attribute {
            Final       = 0x1,
            Override    = 0x2,
            Metadata    = 0x4,
        };

        // ** struct Trait
        struct Trait {
            const QName*    m_name;
            Value           m_value;
            Type            m_type;
            int             m_slot;
            Uint8           m_attr;
        };

    public:

        void            addTrait( const Str& owner, const QName* name, const ::avm2::Class* valueType, const Value& value, Type type, int slot, Uint8 attr );
        void            assignSlots( void );
        void            setSuper( Traits* value );
        void            setSlots( ValueArray& slots ) const;
        bool            resolveSlot( const Str& name, TraitAccess access, int& slot ) const;
        int             slotCount( void ) const;
        void            mergeTraits( const Str& ns, const Traits* traits );

    private:

        void            addProperty( const Str& owner, const QName* name, const Value& value, Type type, int slot, Uint8 attr );
        bool            findTrait( const Str& name, Type type, Trait& trait ) const;
        Value           defaultValueForType( const ::avm2::Class* type ) const;
        bool            isSlotFree( int idx ) const;

    private:

        typedef string_hash<Trait>  TraitRegistry;
        TraitRegistry   m_traits;
        TraitsWeak      m_super;
    };

} // namespace avm2

#endif /* defined(__avm2__Trait__) */
