/**************************************************************************

 The MIT License (MIT)

 Copyright (c) 2003 Thatcher Ulrich <tu@tulrich.com>
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

#ifndef avm_Common_h
#define avm_Common_h

#include "../base/membuf.h"
#include "../base/container.h"
#include "../base/smart_ptr.h"
#include "../base/tu_gc.h"
#include "../base/tu_file.h"
#include "../base/stream.h"
#include "../base/logger.h"
#include "../base/tu_gc_singlethreaded_refcount.h"

#include <vector>
#include <string>

namespace avm2 {

    DECLARE_GC_TYPES(tu_gc::singlethreaded_refcount);
    SPECIALIZE_GC_CONTAINER(gc_array, array);

    // For things that should be automatically garbage-collected.
    struct ref_counted : public gc_object, public weak_pointee_mixin
    {
    };

#if TU_CONFIG_VERBOSE
#	define IF_VERBOSE_ACTION(exp) if (avm2::get_verbose_action()) { exp; }
#	define IF_VERBOSE_PARSE(exp) if (avm2::get_verbose_parse()) { exp; }
#	define IF_VERBOSE_DEBUG(exp) if (avm2::get_verbose_debug()) { exp; }
#else
#	define IF_VERBOSE_ACTION(exp) {}
#	define IF_VERBOSE_PARSE(exp) {}
#	define IF_VERBOSE_DEBUG(exp) {}
#endif
/*
#ifdef __GNUC__
    // use the following to catch errors: (only with gcc)
    void	log_msg(const char* fmt, ...) __attribute__((format (printf, 1, 2)));
    void	log_error(const char* fmt, ...) __attribute__((format (printf, 1, 2)));
#else	// not __GNUC__
    exported_module void	log_msg(const char* fmt, ...);
    exported_module void	log_error(const char* fmt, ...);
#endif	// not __GNUC__*/

    // Control verbosity of specific categories.
    bool get_verbose_parse();
    bool get_verbose_debug();
    bool get_verbose_action();
    void set_verbose_action(bool verbose);
    void set_verbose_parse(bool verbose);

    // ** AVM2
#define AVM2_DEBUG  (1)
#define AVM2_INTERNAL_BUILTIN (1)
#define AVM2_VERBOSE_INSTRUCTIONS (1)
#define AVM2_VERBOSE( ... ) IF_VERBOSE_ACTION( logger::msg( __VA_ARGS__ ) )

#if AVM2_DEBUG
#define AVM2_DEBUG_ONLY( x ) IF_VERBOSE_ACTION( x )
#else
#define AVM2_DEBUG_ONLY( x )
#endif

#if AVM2_VERBOSE_INSTRUCTIONS
#define AVM2_VERBOSE_INSTRUCTION( ... )   AVM2_VERBOSE( __VA_ARGS__ )
#else
#define AVM2_VERBOSE_INSTRUCTION( ... )
#endif

#define AvmBeginStaticClass( type )             class type##Closure {       \
public:
#define AvmBeginClass( type )                   class type##Closure {       \
public:                     \
AvmDeclareNew( type )
#define AvmEndClass                             };
#define AvmDeclareNew( type )                   static Object*  newOp( Domain* domain ) { return new type( domain ); }
#define AvmDeclareMethod( name )                static void     name( Frame* frame );
#define AvmDeclareProperty( getter, setter )    AvmDeclareMethod( getter )  \
AvmDeclareMethod( setter )
#define AvmDeclareReadonly( getter )            AvmDeclareMethod( getter )

#define AvmDeclarePtrs( type, container )       typedef weak_ptr<type>          type##Weak;         \
typedef weak_ptr<const type>    type##WeakConst;    \
typedef gc_ptr<type>            type##Ptr;          \
typedef gc_ptr<const type>      type##PtrConst;     \
typedef array<type##Ptr>        container;          \
typedef array<type##Weak>       container##Weak;

    struct as_property;
    typedef tu_string   tu_stringi;
    typedef tu_string   Str;
    typedef as_property Property;

    typedef array<int> IntegerArray;
    typedef array<Str> StrArray;

    class Value;
    class AbcInfo;
    class Domain;
    class Name;
    class Typename;
    class Multiname;
    class QName;
    class RTQName;
    class RTQNameL;
    class MultinameL;
    class Namespace;
    class Traits;
    class Script;
    class Arguments;
    class Exception;
    class Frame;
    class Iterator;
    class ObjectInterface;
        class Object;
            class GlobalObject;
            class Class;
            class Array;
            class String;
            class Function;
                class FunctionClosure;
                class FunctionNative;
                class FunctionScript;

    AvmDeclarePtrs( Object, Objects )
    AvmDeclarePtrs( Iterator, Iterators )
    AvmDeclarePtrs( Class, Classes )
    AvmDeclarePtrs( Function, Functions )
    AvmDeclarePtrs( Namespace, Namespaces )
    AvmDeclarePtrs( Name, Names )
    AvmDeclarePtrs( QName, QNames );
    AvmDeclarePtrs( Traits, TraitsContainer )
    AvmDeclarePtrs( GlobalObject, GlobalObjects )
    AvmDeclarePtrs( Script, Scripts )

    // ** enum TraitAccess
    enum TraitAccess {
        TraitRead,
        TraitWrite,
    };

    // ** enum TypeId
    enum TypeId {
        TypeNull,
        TypeUndefined,
        TypeVoid,
        TypeBoolean,
        TypeNumber,
        TypeInt,
        TypeUInt,
        TypeString,
        TypeArray,
        TypeFunction,
        TypeClass,
        TypeObject,

        TotalTypeIds
    };

    // ** enum ClassId
    enum ClassId
    {
        AS_OBJECT,
        AS_STRING,
        AS_XML,
        AS_INT,
        AS_UINT,
        AS_NUMBER,
        AS_BOOLEAN,
        AS_ERROR,
        AS_FUNCTION,
        AS_FUNCTION_CLOSURE,
        AS_C_FUNCTION,
        AS_3_FUNCTION,	// action script 3 function
        AS_ACTIVATION_SCOPE,
        AS_CATCH_SCOPE,
        AS_ARRAY,
     /*   AS_CHARACTER,
        AS_SPRITE,
        AS_MOVIE_DEF,
        AS_MOVIE_DEF_SUB,
        AS_CHARACTER_DEF,
        AS_SPRITE_DEF,
        AS_VIDEO_DEF,
        AS_SOUND_SAMPLE,
        AS_VIDEO_INST,
        AS_KEY,

        AS_COLOR,
        AS_SOUND,
        AS_FONT,
        AS_CANVAS,
        AS_NETSTREAM,

        AS_SELECTION,
        AS_POINT,
        AS_MATRIX,
        AS_TRANSFORM,
        AS_COLOR_TRANSFORM,
        AS_NETCONNECTION,
        AS_LISTENER,
        AS_DATE,
        AS_EDIT_TEXT,
        AS_XML_SOCKET,
        AS_TEXTFORMAT,
        AS_MCLOADER,
        AS_LOADVARS,
        AS_TIMER,
        AS_MOUSE,
*/
    };

    // ** class ObjectInterface
    class ObjectInterface : public ref_counted {
    public:

        virtual bool is( int class_id ) const = 0;
    };

    // cast_to<avm2 object>(obj) implementation (from Julien Hamaide)
    template <typename cast_class>
    cast_class* cast_to(ObjectInterface* object)
    {
        if (object)
        {
            return object->is(cast_class::m_class_id) ? static_cast<cast_class*>(object) : 0;
        }
        return 0;
    }

    template <typename cast_class>
    const cast_class* cast_to(const ObjectInterface* object)
    {
        if (object)
        {
            return object->is(cast_class::m_class_id) ? static_cast<const cast_class*>(object) : 0;
        }
        return 0;
    }

    typedef weak_ptr<FunctionClosure>               FunctionClosureWeak;
    typedef hash<Function*, FunctionClosureWeak>    FunctionClosures;

    typedef void (*FunctionNativeThunk)( Frame* frame );
    typedef Object* (*CreateInstanceThunk)( Domain* domain );

} // namespace avm2

//typedef unsigned char Uint8;

#endif
