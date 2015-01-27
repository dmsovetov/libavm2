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

#ifndef avm2_AS_CLASS_H
#define avm2_AS_CLASS_H

#include "Function.h"
#include "Trait.h"
#include "Abc.h"

namespace avm2 {

    // ** class Class
	class Class : public Function {
    public:

        // ** enum Flags
        enum Flags {
            Sealed      = 0x1,
            Final       = 0x2,
            Interface   = 0x4,
        };

	public:

                                Class( Domain* domain, TypeId typeId, Class* superClass, QName* name, Uint8 flags, CreateInstanceThunk createInstance = NULL, Function* init = NULL, Function* staticInit = NULL );

        // ** Object
        virtual const char*     to_string();

        // ** Class
        void                    initialize( void );
        Value                   createInstance( void ) const;
        void                    construct( const Value& instance, const Arguments& args = Arguments(), Frame* parentFrame = NULL ) const;
        const Str&              name( void ) const;
        Str                     qualifiedName( const Str& separator = ":" ) const;
        const Function*         init( void ) const;
        Class*                  superClass( void ) const;
        TypeId                  typeId( void ) const;
        bool                    is( const Class* type ) const;
        bool                    isSealed( void ) const;
        Traits*                 classTraits( void ) const;
        void                    setClassTraits( Traits* value );
        Traits*                 instanceTraits( void ) const;
        void                    setInstanceTraits( Traits* value );

        bool                    findBuiltIn( const Str& name, Value* value ) const;
        void                    addBuiltIn( const Str& name, const Value& value );

    private:

        // ** Function
        virtual void            execute( Frame* frame ) const;

	private:

        QNameWeak               m_name;
        Uint8                   m_flags;
        gc_ptr<Function>        m_init;
        gc_ptr<Function>        m_staticInit;
        TraitsPtr               m_instanceTraits;
        TraitsPtr               m_classTraits;
		weak_ptr<Class>         m_superClass;
        CreateInstanceThunk     m_createInstance;
        bool                    m_isInitialized;
        Members                 m_builtIn;
        TypeId                  m_typeId;
	};
}


#endif
