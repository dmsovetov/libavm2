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

#ifndef __avm2__Linker__
#define __avm2__Linker__

#include "Abc.h"

#include "Multiname.h"
#include "Function.h"
#include "Class.h"

namespace avm2 {

    // ** class Linker
    class Linker {
    public:

                                Linker( Domain* domain, const AbcInfo* abc );

        bool                    link( void );

    private:

        void                    linkStrings( void );
        void                    linkNamespaces( void );
        Namespaces              linkNamespaceSet( const NsSetInfo& nsSet );
        Name*                   linkMultiname( const MultinameInfo* multinameInfo );
        Script*                 linkScript( const ScriptInfo* scriptInfo );
        Class*                  linkClass( const InstanceInfo* instanceInfo, const ClassInfo* classInfo );
        bool                    linkClassTraits( Class* cls, int index );
        Traits*                 linkTraits( const Str& owner, const TraitsArray& traitsInfo );
        FunctionScript*         linkFunction( const MethodInfo* methodInfo );
        Exception*              linkException( const ExceptInfo* exceptionInfo );
        Instructions            linkInstructions( const FunctionScript* function, const BodyInfo* bodyInfo );
        Value                   linkConstant( ConstantKind kind, int index ) const;

        Class*                  resolveClass( int index ) const;
        Class*                  resolveTemplateClass( const Typename* name ) const;
        const QName*            resolveQName( int index ) const;
        const Str&              resolveString( int index ) const;
        static Namespace::Kind  namespaceKind( NamespaceInfo::Kind kind );

    public:

        const AbcInfo*          m_abc;
        Domain*                 m_domain;

        Classes                 m_classes;
        FunctionScripts         m_functions;
        Names                   m_names;
        Strings                 m_strings;
        Namespaces              m_namespaces;
    };

} // namespace avm2

#endif /* defined(__avm2__Linker__) */
