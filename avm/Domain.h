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

#ifndef __avm2__Domain__
#define __avm2__Domain__

#include "Function.h"
#include "Multiname.h"

namespace avm2 {

    // ** class GlobalObject
    class GlobalObject : public Object {
    public:

                            GlobalObject( Domain* domain ) : Object( domain ) {}

        // ** Object
        virtual const char* to_string( void ) { return "[object global]"; }
    };

    // ** class Domain
    class Domain {
    public:

                            Domain( void );

        void                setNames( const Names& value );
        Name*               name( int index ) const;
        void                setStrings( const Strings& value );
        void                setFunctions( const FunctionScripts& value );
        GlobalObject*       global( void ) const;

        virtual void        registerPackages( void );
        Class*              registerClass( TypeId typeId, const Str& name, const Str& superClass, CreateInstanceThunk createInstance = NULL, FunctionNative* init = NULL );
        Class*              registerClass( const QName* name, Class* cls );
        Class*              findClass( const Str& name, bool initialize = true ) const;
        QName*              findQName( const Str& name, const Str& ns ) const;
        QName*              internQName( const Str& name, const Str& ns );

    protected:

        GlobalObjectPtr     m_global;
        Names               m_names;
        Strings             m_strings;
        FunctionScripts     m_functions;

        Names               m_nameCache;
    };

#if 0
    // ** class FlashDomain
    class FlashDomain : public Domain {
    public:

                            FlashDomain( player* player );

        // ** Domain
        virtual void        registerPackages( void );

    private:

        void                registerDisplay( void );
        void                registerEvents( void );
    };
#endif

} // namespace avm2

#endif /* defined(__avm2__Domain__) */
