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

#ifndef __avm2__Multiname__
#define __avm2__Multiname__

#include "Common.h"

namespace avm2 {

    // ** class Namespace
    class Namespace : public ref_counted {
    public:

        static Str  AS3;
        static Str  XML;

        // enum Kind
        enum Kind {
            Ns,
			Package,
			PackageInternal,
            Private,
			Protected,
            StaticProtected,
			Explicit
        };

                        Namespace( Kind kind, const Str& name );

        const Str&      name( void ) const;
        Str             uri( void ) const;
        Kind            kind( void ) const;

    private:

        Str             m_name;
        Kind            m_kind;
    };

    // ** class Name
    class Name : public ref_counted {
    public:

                                    Name( const Str& name );

        const Str&                  name( void ) const;
        void                        setName( const Str& value );

        virtual bool                hasRuntimeName( void ) const;
        virtual bool                hasRuntimeNamespace( void ) const;

        virtual const QName*        isQName( void ) const;
        virtual const Multiname*    isMultiname( void ) const;
        virtual const Typename*     isTypename( void ) const;

    protected:

        Str                         m_name;
    };

    // ** class QName
    class QName : public Name {
    public:

                                    QName( const Str& name, Namespace* ns );

        // ** Name
        virtual const QName*        isQName( void ) const;

        // ** QName
        Str                         qualifiedName( void ) const;
        const Namespace*            ns( void ) const;

    protected:

        NamespacePtr                m_namespace;
    };

    // ** class RTQName
    class RTQName : public QName {
    public:

                                    RTQName( Namespace* ns );

        // ** Name
        virtual bool                hasRuntimeNamespace( void ) const;

        // ** RTQName
        void                        setName( const Str& value ) const;
    };

    // ** class RTQNameL
    class RTQNameL : public RTQName {
    public:

                                    RTQNameL( void );

        // ** Name
        virtual bool                hasRuntimeName( void ) const;
        virtual bool                hasRuntimeNamespace( void ) const;

        // ** RTQNameL
        void                        setNamespace( Namespace* value ) const;
    };

    // ** class Multiname
    class Multiname : public Name {
    public:

                                    Multiname( const Str& name, const Namespaces& namespaces );

        // ** Name
        virtual const Multiname*    isMultiname( void ) const;

        // ** Multiname
        const Namespaces&           namespaces( void ) const;
        int                         count( void ) const;
        Str                         qualifiedName( int index ) const;

    private:

        Namespaces                  m_namespaces;
    };

    // ** class MultinameL
    class MultinameL : public Multiname {
    public:

                                    MultinameL( const Namespaces& namespaces );

        // ** Name
        virtual bool                hasRuntimeName( void ) const;
    };

    // ** class Typename
    class Typename : public Name {
    public:

                                    Typename( const QName* type, const QName* arg );

        // ** Name
        virtual const Typename*     isTypename( void ) const;

        // ** Typename
        const QName*                type( void ) const;

    private:

        const QName*                m_type;
        const QName*                m_arg;
    };

} // namespace avm2

#endif /* defined(__avm2__Multiname__) */
