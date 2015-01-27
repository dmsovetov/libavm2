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

#include "Linker.h"

#include "Multiname.h"
#include "Script.h"
#include "Domain.h"
#include "Class.h"
#include "Avm.h"
#include "Dump.h"
#include "Function.h"

namespace avm2 {

// ** Linker::Linker
Linker::Linker( Domain* domain, const AbcInfo* abc ) : m_domain( domain ), m_abc( abc )
{

}

// ** Linker::link
bool Linker::link( void )
{
    bool errors = false;

    linkStrings();
    linkNamespaces();

    // ** Link multinames
    for( int i = 0, n = ( int )m_abc->m_multiname.size(); i < n; i++ ) {
        m_names.push_back( linkMultiname( &m_abc->m_multiname[i] ) );
    }
    m_domain->setNames( m_names );

    // ** Link functions
    for( int i = 0, n = ( int )m_abc->m_method.size(); i < n; i++ ) {
        if( FunctionScript* func = linkFunction( m_abc->m_method[i].get() ) ) {
            m_functions.push_back( func );
        } else {
            errors = true;
        }
    }
    m_domain->setFunctions( m_functions );

    // ** Link classes
    for( int i = 0, n = ( int )m_abc->m_instance.size(); i < n; i++ ) {
        if( Class* cls = linkClass( m_abc->m_instance[i].get(), m_abc->m_class[i].get() ) ) {
            m_classes.push_back( cls );
        } else {
            errors = true;
        }
    }

    // ** Link class traits
    for( int i = 0, n = ( int )m_classes.size(); i < n; i++ ) {
        linkClassTraits( m_classes[i].get(), i );
    }

    // ** Link scripts
    Scripts scripts;
    for( int i = 0, n = ( int )m_abc->m_script.size(); i < n; i++ ) {
        scripts.push_back( linkScript( m_abc->m_script[i].get() ) );
    }

    // ** Link instructions
    for( int i = 0, n = ( int )m_functions.size(); i < n; i++ ) {
        Instructions instructions = linkInstructions( m_functions[i].get(), &m_abc->m_method[i]->m_body );
        m_functions[i]->setInstructions( instructions );
    }

    // ** Link exception/argument types
    for( int i = 0, n = ( int )m_functions.size(); i < n; i++ ) {
        FunctionScript*   function   = m_functions[i].get();
        const MethodInfo& signature  = *m_abc->m_method[i];
        const BodyInfo&   body       = m_abc->m_method[i]->m_body;
        const Exceptions& exceptions = function->exceptions();

        // ** Return type
        function->setReturnType( resolveClass( signature.m_return_type ) );

        // ** Exceptions
        for( int j = 0, jn = ( int )exceptions.size(); j < jn; j++ ) {
            const ExceptInfo* info = body.m_exception[j].get();
            if( info->m_exc_type == 0 ) {
                continue;
            }

            Class* type = resolveClass( info->m_exc_type );
            assert( type );
            exceptions[j]->setType( type );
        }

        // ** Arguments
        ClassesWeak args;
        ValueArray  defaults;

        for( int j = 0, jn = ( int )signature.m_param_type.size(); j < jn; j++ ) {
            Class* type = NULL;

            if( signature.m_param_type[j] ) {
                type = resolveClass( signature.m_param_type[j] );
                assert( type );
            }
            
            args.push_back( type );
        }

        for( int j = 0, jn = ( int )signature.m_options.size(); j < jn; j++ ) {
            const MethodInfo::OptionDetail& opt = signature.m_options[j];
            defaults.push_back( linkConstant( opt.m_kind, opt.m_index ) );
        }

        function->setArguments( args, defaults );
    }

    // ** Run script initializers
    for( int i = 0, n = ( int )m_abc->m_script.size(); i < n; i++ ) {
        Script*  script  = scripts[i].get();
        m_domain->global()->setTraits( script->traits() );
        script->init()->call();
    }

    return errors;
}

// ** Linker::linkStrings
void Linker::linkStrings( void )
{
    for( int i = 0, n = ( int )m_abc->m_string.size(); i < n; i++ ) {
        m_strings.push_back( new String( m_domain, m_abc->m_string[i] ) );
    }

    m_domain->setStrings( m_strings );
}

// ** Linker::linkNamespaces
void Linker::linkNamespaces( void )
{
    m_namespaces.push_back( NULL );

    for( int i = 1, n = ( int )m_abc->m_namespace.size(); i < n; i++ ) {
        const NamespaceInfo& ns = m_abc->m_namespace[i];
        m_namespaces.push_back( new Namespace( namespaceKind( ns.m_kind ), m_abc->m_string[ns.m_name] ) );
    }
}

// ** Linker::linkNamespaceSet
Namespaces Linker::linkNamespaceSet( const NsSetInfo& nsSet )
{
    Namespaces namespaces;

    for( int i = 0; i < nsSet.size(); i++ ) {
        namespaces.push_back( m_namespaces[nsSet[i]] );
    }

    return namespaces;
}

// ** Linker::linkMultiname
Name* Linker::linkMultiname( const MultinameInfo* multinameInfo )
{
    if( multinameInfo->m_index == 0 ) {
        return NULL;
    }

    switch( multinameInfo->m_kind ) {
    case MultinameInfo::QName:
    case MultinameInfo::QNameA:         return new QName( resolveString( multinameInfo->m_name ), m_namespaces[multinameInfo->m_ns] );
    case MultinameInfo::RTQName:
    case MultinameInfo::RTQNameA:       return new RTQName( m_namespaces[multinameInfo->m_ns] );
    case MultinameInfo::RTQNameL:
    case MultinameInfo::RTQNameLA:      return new RTQNameL;
    case MultinameInfo::Multiname:
    case MultinameInfo::MultinameA:     return new Multiname( resolveString( multinameInfo->m_name ), linkNamespaceSet( m_abc->m_ns_set[multinameInfo->m_ns_set] ) );
    case MultinameInfo::MultinameL:
    case MultinameInfo::MultinameLA:    return new MultinameL( linkNamespaceSet( m_abc->m_ns_set[multinameInfo->m_ns_set] ) );
    case MultinameInfo::Typename:       {
                                            const QName* type = m_names[multinameInfo->m_type_name]->isQName();
                                            const QName* arg  = m_names[multinameInfo->m_type_arg]->isQName();
                                            return new Typename( type, arg );
                                        }
                                        break;
    default:                                assert( false ); break;
    }

    return NULL;
}

// ** Linker::linkScript
Script* Linker::linkScript( const ScriptInfo* scriptInfo )
{
    Traits*      traits  = linkTraits( "global", scriptInfo->m_trait );
    Function*    init    = m_functions[scriptInfo->m_init].get();
    Script*      script  = new Script( init, traits );

    traits->assignSlots();

    return script;
}

// ** Linker::linkException
Exception* Linker::linkException( const ExceptInfo* exceptionInfo )
{
    Str name = "";

    if( exceptionInfo->m_var_name ) {
        const QName* qname = m_names[exceptionInfo->m_var_name]->isQName();
        assert( qname );
        name = qname->name();
    }

    return new Exception( name );
}

// ** Linker::linkFunction
FunctionScript* Linker::linkFunction( const MethodInfo* methodInfo )
{
    const BodyInfo& body = methodInfo->m_body;

    Traits*         traits     = linkTraits( "", body.m_trait );
    FunctionScript* function   = new FunctionScript( m_domain, body.m_max_stack, body.m_max_scope_depth );
    function->setTraits( traits );

    // ** Link exceptions
    for( int i = 0, n = ( int )body.m_exception.size(); i < n; i++ ) {
        Exception* exception = linkException( body.m_exception[i].get() );
        function->addException( exception );
    }

/*
    function->m_return_type = methodInfo->m_return_type;
    function->m_param_type  = methodInfo->m_param_type;
    function->m_name        = methodInfo->m_name;
    function->m_flags       = methodInfo->m_flags;
//	function->m_options     = methodInfo->m_options;

    function->m_init_scope_depth    = methodInfo->m_body.m_init_scope_depth;
    function->m_max_scope_depth     = methodInfo->m_body.m_max_scope_depth;
    function->m_max_stack           = methodInfo->m_body.m_max_stack;
    function->m_local_count         = methodInfo->m_body.m_local_count;
    function->m_code                = methodInfo->m_body.m_code;
*/

    return function;
}

// ** Linker::linkClassTraits
bool Linker::linkClassTraits( Class* cls, int index )
{
    const InstanceInfo* instanceInfo = m_abc->m_instance[index].get();
    const ClassInfo*    classInfo    = m_abc->m_class[index].get();

    Traits* instanceTraits = linkTraits( cls->qualifiedName(), instanceInfo->m_trait );
    Traits* classTraits    = linkTraits( cls->qualifiedName() + "$", classInfo->m_trait );

    instanceTraits->mergeTraits( cls->qualifiedName() + ":staticProtected", classTraits );

    if( Class* super = cls->superClass() ) {
        classTraits->mergeTraits( cls->qualifiedName() + ":staticProtected", super->classTraits() );
    }

    cls->setClassTraits( classTraits );
    cls->setInstanceTraits( instanceTraits );

    return true;
}

// ** Linker::linkClass
Class* Linker::linkClass( const InstanceInfo* instanceInfo, const ClassInfo* classInfo )
{
    // ** Resolve class names
    const QName* className  = resolveQName( instanceInfo->m_name );
    Class*       superClass = resolveClass( instanceInfo->m_super_name );

    assert( className );

    // ** Create class flags
    Uint8 flags = 0;

    if( instanceInfo->m_flags & InstanceInfo::ClassSealed )     flags |= Class::Sealed;
    if( instanceInfo->m_flags & InstanceInfo::ClassFinal  )     flags |= Class::Final;
    if( instanceInfo->m_flags & InstanceInfo::ClassInterface )  flags |= Class::Interface;

    // ** Create and register class
    Class* cls = new Class( m_domain, TypeObject, superClass, const_cast<QName*>( className ), flags, NULL, m_functions[instanceInfo->m_iinit], m_functions[classInfo->m_cinit] );

    return m_domain->registerClass( className, cls );
}

// ** Linker::linkTraits
Traits* Linker::linkTraits( const Str& owner, const TraitsArray& traits )
{
    Traits* result = new Traits;

    for( int i = 0, n = traits.size(); i < n; i++ ) {
        const TraitsInfo*   trait = traits[i].get();
        Traits::Type        type;
        Class*              valueType;
        Value               value;
        int                 slot = 0;

        switch( trait->m_kind ) {
        case TraitsInfo::Const:
        case TraitsInfo::Slot:      type        = (trait->m_kind == TraitsInfo::Const) ? Traits::Const : Traits::Slot;
                                    slot        = trait->m_slot.m_slot_id;
                                    value       = linkConstant( trait->m_slot.m_vkind, trait->m_slot.m_vindex );
                                    valueType   = resolveClass( trait->m_slot.m_type_name );
                                    break;

        case TraitsInfo::Method:    type  = Traits::Method;
                                    value = Value( m_functions[trait->m_method.m_method] );
                                    break;

        case TraitsInfo::Setter:
        case TraitsInfo::Getter:    type  = trait->m_kind == TraitsInfo::Setter ? Traits::Setter : Traits::Getter;
                                    value = Value( m_functions[trait->m_method.m_method] );
                                    break;

        case TraitsInfo::Class:     type  = Traits::Class;
                                    slot  = trait->m_cls.m_slot_id;
                                    value = Value( m_classes[trait->m_cls.m_classi] );
                                    break;

        case TraitsInfo::Function:  assert( false );
                                    break;

        default: assert( false );
        }

        result->addTrait( owner, m_names[trait->m_name]->isQName(), valueType, value, type, slot, trait->m_attr );
    }

    return result;
}

// ** Linker::linkConstant
Value Linker::linkConstant( ConstantKind kind, int index ) const
{
    switch( kind ) {
    case ConstTypeInt:                  return Value( m_abc->m_integer[index] );
    case ConstTypeUInt:                 return Value( m_abc->m_uinteger[index] );
    case ConstTypeDouble:               return Value( m_abc->m_double[index] );
    case ConstTypeUtf8:                 return Value( m_strings[index].get() );
    case ConstTypeTrue:                 return Value( true );
    case ConstTypeFalse:                return Value( false );
    case ConstTypeNull:                 return Value::null;
    case ConstTypeUndefined:            return Value::undefined;
    case ConstTypeNamespace:            //assert( false ); break;
    case ConstTypePackageNamespace:     //assert( false ); break;
    case ConstTypePackageInternalNs:    //assert( false ); break;
    case ConstTypeProtectedNamespace:   //assert( false ); break;
    case ConstTypeExplicitNamespace:    //assert( false ); break;
    case ConstTypeStaticProtectedNs:    //assert( false ); break;
    case ConstTypePrivateNs:            //assert( false ); break;
                                        AVM2_VERBOSE( "Linker::linkConstant : unhandled trait constant type %x\n", kind );
                                        break;
    }

    return Value::undefined;
}

// ** Linker::multinameKind
//Name::Kind Linker::multinameKind( MultinameInfo::Kind kind )
//{
//    switch( kind ) {
//    case MultinameInfo::QName:
//    case MultinameInfo::QNameA:         return Name::Qualified;
//    case MultinameInfo::RTQName:
//    case MultinameInfo::RTQNameA:       return Name::RuntimeQualified;
//    case MultinameInfo::RTQNameL:
//    case MultinameInfo::RTQNameLA:      return Name::RuntimeQualifiedLate;
//    case MultinameInfo::Multiname:
//    case MultinameInfo::MultinameA:     return Name::MultipleNamespace;
//    case MultinameInfo::MultinameL:
//    case MultinameInfo::MultinameLA:    return Name::MultipleNamespaceLate;
//    default:                            break;
//    }
//
//    return Name::Unknown;
//}

// ** Linker::namespaceKind
Namespace::Kind Linker::namespaceKind( NamespaceInfo::Kind kind )
{
    switch( kind ) {
    case NamespaceInfo::Namespace:          return Namespace::Ns;
    case NamespaceInfo::PackageNamespace:   return Namespace::Package;
    case NamespaceInfo::PackageInternalNs:  return Namespace::PackageInternal;
    case NamespaceInfo::ProtectedNamespace: return Namespace::Protected;
    case NamespaceInfo::ExplicitNamespace:  return Namespace::Explicit;
    case NamespaceInfo::StaticProtectedNs:  return Namespace::StaticProtected;
    case NamespaceInfo::PrivateNs:          return Namespace::Private;
    default: break;
    }

    assert(false);
    return Namespace::Ns;
}

// ** Linker::resolveClass
Class* Linker::resolveClass( int index ) const
{
    if( index == 0 ) {
        return NULL;
    }

    Name* name = m_names[index];

    if( const Typename* tname = name->isTypename() ) {
        return resolveTemplateClass( tname );
    }

    const QName* qname = name->isQName();
    assert( qname );

    Class* cls = m_domain->findClass( qname->qualifiedName(), false );
    if( !cls ) {
        AVM2_VERBOSE( "Linker::resolveClass : unresolved class %s\n", qname->qualifiedName().c_str() );
    }

    return cls;
}

// ** Linker::resolveTemplateClass
Class* Linker::resolveTemplateClass( const Typename* name ) const
{
    return m_domain->findClass( name->type()->name() );
}

// ** Linker::resolveString
const Str& Linker::resolveString( int index ) const
{
    assert( index >= 0 && index < m_strings.size() );
    return m_strings[index]->toString();
}

// ** Linker::resolveQName
const QName* Linker::resolveQName( int index ) const
{
    return index ? m_names[index]->isQName() : NULL;
}

// ** Linker::linkInstructions
Instructions Linker::linkInstructions( const FunctionScript* function, const BodyInfo* bodyInfo )
{
    const Exceptions&   exceptions = function->exceptions();
    Instruction         instr;
    Instructions        result;
    const Uint8*        data = ( const Uint8* )bodyInfo->m_code.data();
    int                 size = bodyInfo->m_code.size();
    hash<int, int>      offsetToInstruction, instructionToOffset;

    #define U32( dst ) stream::readU30( dst, data + i )
    #define U30( dst ) stream::readU30( dst, data + i )
    #define S24( dst ) stream::readS24( dst, data + i )
    #define U8( dst )  stream::readU8( dst,  data + i )

    AVM2_VERBOSE( "\t\t\t\t%s\n", function->name().c_str() );
    for( int i = 0; i < size; ) {
        memset( &instr, 0, sizeof( instr ) );

        int opCode;
        i += U8( opCode );

        instr.opCode = static_cast<OpCode>( opCode );

        int currentOffset       = i - 1;
        int currentInstruction  = result.size();

        offsetToInstruction[currentOffset]      = currentInstruction;
        instructionToOffset[currentInstruction] = currentOffset;

        AVM2_VERBOSE_INSTRUCTION( "\t\t\t\t%d : %s (offset %d)", currentInstruction, Dump::formatOpCode( instr.opCode ), currentOffset );

        switch( instr.opCode ) {
            case ApplyType:
            case Call:
                //    i = Stream::ReadU30( instr.argCount, body.m_code + i );
                i += U30( instr.ArgCount );
                break; // arg_count : u30

            case CallSuperVoid:
            case CallSuper:
            case CallPropVoid:
            case CallPropLex:
            case CallProperty:
            case ConstructProp:
            {
                int index;

                i += U30( index );
                i += U30( instr.ArgCount );

                instr.Identifier = m_domain->name( index );

                AVM2_VERBOSE_INSTRUCTION( " [%s], argCount=%d", Dump::formatMultiname( m_abc, index ).c_str(), instr.ArgCount );
            }
                break; // index : u30, arg_count : u30

            case CallMethod:
            case CallStatic:
                assert( false );
                break;

            case Construct:
                i += U30( instr.ArgCount );
                break; // arg_count : u30

            case Coerce:
            {
                int index;

                i += U30( index );
                instr.Identifier = m_domain->name( index );
            }
                break; // index : u30

            case ConstructSuper:
                i += U30( instr.ArgCount );
                AVM2_VERBOSE_INSTRUCTION( " argCount=%d", instr.ArgCount );
                break; // arg_count : u30

            case Debug:
                i += U8( instr.dbgType );
                i += U30( instr.dbgIndex );
                i += U8( instr.dbgReg );
                i += U30( instr.dbgExtra );
                break; // debug_type : u8, index : u30, reg : u8, extra : u30

            case DebugFile:
            {
                int index;
                i += U30( index );
                instr.Str = m_strings[index];
            }
                break; // index : u30

            case DebugLine:
                i += U30( instr.line );
                break; // line : u30

            case DecLocal:
                i += U30( instr.Integer );
                break; // index : u30

            case DeleteProperty:
            {
                int index;
                i += U30( index );
                instr.Identifier = m_domain->name( index );
            }
                break; // index : u30

            case DXNS:
            {
                int index;
                i += U30( index );
                instr.Str = m_strings[index];
            }
                break; // index : u30

            case FindProperty:
            {
                int index;
                i += U30( index );
                instr.Identifier = m_domain->name( index );
                AVM2_VERBOSE_INSTRUCTION( " [%s]", Dump::formatMultiname( m_abc, index ).c_str() );
            }
                break; // index : u30

            case FindPropertyStrict:
            {
                int index;
                i += U30( index );
                instr.Identifier = m_domain->name( index );
                AVM2_VERBOSE_INSTRUCTION( " [%s]", Dump::formatMultiname( m_abc, index ).c_str() );
            }
                break; // index : u30

            case GetDescendants:
            {
                int index;
                i += U30( index );
                instr.Identifier = m_domain->name( index );
            }
                break; // index : u30

            case GetGlobalSlot:
                i += U30( instr.Integer );
                break; // slot_index : u30

            case GetLex:
            {
                int index;
                i += U30( index );
                instr.Identifier = m_domain->name( index );
            }
                break; // index : u30

            case GetLocal:
                i += U30( instr.Integer );
                break; // index : u30

            case GetProperty:
            {
                int index;
                i += U30( index );
                instr.Identifier = m_domain->name( index );
                AVM2_VERBOSE_INSTRUCTION( " [%s]", Dump::formatMultiname( m_abc, index ).c_str() );
            }
                break; // index : u30

            case GetScopeObject:
                i += U8( instr.Integer );
                AVM2_VERBOSE_INSTRUCTION( " index=%d", instr.Integer );
                break; // index : u8

            case GetSlot:
                i += U30( instr.Integer );
                AVM2_VERBOSE_INSTRUCTION( " slot=%d", instr.Integer );
                break; // slot_index : u30

            case GetSuper:
            {
                int index;
                i += U30( index );
                instr.Identifier = m_domain->name( index );
                AVM2_VERBOSE_INSTRUCTION( " [%s]", Dump::formatMultiname( m_abc, index ).c_str() );
            }
                break; // index : u30

            case HasNext2:
                i += U32( instr.objectReg );
                i += U32( instr.indexReg );
                break; // object_reg : uint, index_reg : uint

            case IfEqual:
            case IfFalse:
            case IfGreaterEqual:
            case IfGreater:
            case IfLessEqual:
            case IfLess:
            case IfNotGreaterEqual:
            case IfNotGreater:
            case IfNotLessEqual:
            case IfNotLess:
            case IfNotEqual:
            case IfStrictEqual:
            case IfStictNotEqual:
            case IfTrue:
            case Jump:
                i += S24( instr.offset );
                instr.offset = i + instr.offset;
                AVM2_VERBOSE_INSTRUCTION( " target=%d", instr.offset );
                break;// offset : s24

            case IncLocal:
                i += U30( instr.Integer );
                break; // index : u30

            case IncLocalI:
                i += U30( instr.Integer );
                break; // index : u30

            case InitProperty:
            {
                int index;
                i += U30( index );
                instr.Identifier = m_domain->name( index );
                AVM2_VERBOSE_INSTRUCTION( " [%s]", Dump::formatMultiname( m_abc, index ).c_str() );
            }
                break; // index : u30

            case IsType:
            {
                int index;
                i += U30( index );
                instr.Identifier = m_domain->name( index );
            }
                break; // index : u30

            case Kill:
                i += U30( instr.Integer );
                break; // index : u30

            case LookupSwitch:
                i += S24( instr.defaultOffset );
                i += U30( instr.caseCount );

                instr.caseOffsets = new int[instr.caseCount];
                for( int j = 0; j <= instr.caseCount; j++ ) {
                    i += S24( instr.caseOffsets[j] );
                }
                break; // default_offset : s24, case_count : u30, case_offsets... : u24

            case NewArray:
                i += U30( instr.ArgCount );
                break; // arg_count : u30

            case NewCatch:
            {
                int index;
                i += U30( index );
                
                instr.Exception = exceptions[index].get();
            }
                break; // index : u30

            case NewClass:
            {
                int index;
                i += U30( index );
                instr.Class = m_classes[index].get();
                AVM2_VERBOSE_INSTRUCTION( " [%s]", Dump::formatClass( m_abc, index ).c_str() );
            }
                break; // index : u30

            case NewFunction:
            {
                int index;
                i += U30( index );
                instr.Function = m_functions[index].get();
            }
                break; // index : u30

            case NewObject:
                i += U30(instr.ArgCount );
                break; // arg_count : u30

            case PushByte:
                i += U8( instr.Integer );
                instr.Integer = ( char )instr.Integer;
                break;// value : u8

            case PushDouble:
            {
                int index;
                i += U30( index );
                instr.Number = m_abc->m_double[index];
            }
                break; // index : u30

            case PushInt:
            {
                int index;
                i += U30( index );
                instr.Integer = m_abc->m_integer[index];
            }
                break; // index : u30

            case PushNamespace:
            {
                int index;
                i += U30( index );
            }
                break; // index : u30

            case PushShort:
                i += U30( instr.Integer );
                break; // value : u30

            case PushString:
            {
                int index;

                i += U30( index );
                instr.Str = m_strings[index];
                AVM2_VERBOSE_INSTRUCTION( " %s", instr.Str->toCString() );
            }
                break; // index : u30

            case PushUInt:
            {
                int index;
                i += U30( index );
                instr.UInt = m_abc->m_uinteger[index];
            }
                break; // index : u30

            case SetLocal:
                i += U30( instr.Integer );
                break; // index : u30

            case SetGlobalSlot:
                i += U30( instr.Integer );
                break; // slot_index : u30

            case SetProperty:
            {
                int index;
                i += U30( index );
                instr.Identifier = m_domain->name( index );
            }
                break;// index : u30

            case SetSlot:
                i += U30( instr.Integer );
                break; // slot_index : u30

            case SetSuper:
            {
                int index;
                i += U30( index );
                instr.Identifier = m_domain->name( index );
            }
                break; // index : u30

            case PushTrue:          break;
            case PushUndefined:     break;
            case PushWith:          break;
            case ReturnValue:       break;
            case ReturnVoid:        break;
            case RightShift:        break;
            case SetLocal0:         break;
            case SetLocal1:         break;
            case SetLocal2:         break;
            case SetLocal3:         break;
            case GetGlobalScope:    break;
            case Add:               break;
            case AddI:              break;
            case AsType:            break;
            case AsTypeLate:        break;
            case BitAnd:            break;
            case BitNot:            break;
            case BitOr:             break;
            case BitXOR:            break;
            case CheckFilter:       break;
            case CoerceToAny:       break;
            case CoerceToString:    break;
            case ConvertToBool:     break;
            case ConvertToInt:      break;
            case ConvertToDouble:   break;
            case ConvertToObject:   break;
            case ConvertToUInt:     break;
            case ConvertToString:   break;
            case DecLocalI:         break;
            case Decrement:         break;
            case DecrementI:        break;
            case Divide:            break;
            case Dup:               break;
            case DXNSLate:          break;
            case Equals:            break;
            case EscXAttr:          break;
            case EscXElem:          break;
            case GetLocal0:         break;
            case GetLocal1:         break;
            case GetLocal2:         break;
            case GetLocal3:         break;
            case GreaterEquals:     break;
            case GreaterThan:       break;
            case HasNext:           break;
            case In:                break;
            case Increment:         break;
            case IncrementI:        break;
            case InstanceOf:        break;
            case IsTypeLate:        break;
            case Label:             break;
            case LessEquals:        break;
            case LessThan:          break;
            case LeftShift:         break;
            case Modulo:            break;
            case Multiply:          break;
            case MultiplyI:         break;
            case Negate:            break;
            case NegateI:           break;
            case NewActivation:     break;
            case NextName:          break;
            case NextValue:         break;
            case Nop:               break;
            case Not:               break;
            case Pop:               break;
            case PopScope:          break;
            case PushFalse:         break;
            case PushNaN:           break;
            case PushNull:          break;
            case PushScope:         break;
            case Throw:             break;
            case Swap:              break;
            case TypeOf:            break;
            case Subtract:          break;
            case SubtractI:         break;
            case StrictEquals:      break;
            case URightShift:       break;
        }

        result.push_back( instr );
        
        AVM2_VERBOSE_INSTRUCTION( "\n" );
    }
    
    // ** Calculate instruction index from offset
    for( int i = 0, n = ( int )result.size(); i < n; i++ ) {
        Instruction& instr = result[i];
        
        switch( instr.opCode ) {
            case IfEqual:
            case IfFalse:
            case IfGreaterEqual:
            case IfGreater:
            case IfLessEqual:
            case IfLess:
            case IfNotGreaterEqual:
            case IfNotGreater:
            case IfNotLessEqual:
            case IfNotLess:
            case IfNotEqual:
            case IfStrictEqual:
            case IfStictNotEqual:
            case IfTrue:
            case Jump:
            {
                assert( instructionToOffset.find( i ) != instructionToOffset.end() );

                int nextOffset = instr.offset;
                assert( offsetToInstruction.find( nextOffset ) != instructionToOffset.end() );
                instr.offset = offsetToInstruction[nextOffset] - 1;
            }
            break;

            case LookupSwitch:
            {
                assert( instructionToOffset.find( i ) != instructionToOffset.end() );
                int currentOffset = instructionToOffset[i];

                instr.defaultOffset = offsetToInstruction[currentOffset + instr.defaultOffset];
                for( int j = 0; j < instr.caseCount + 1; j++ ) {
                    instr.caseOffsets[j] = offsetToInstruction[currentOffset + instr.caseOffsets[j]];
                }
            }
                
            default:
                break;
        }
    }

    AVM2_VERBOSE_INSTRUCTION( "\n" );

    // ** Map exceptions
    for( int i = 0, n = ( int )exceptions.size(); i < n; i++ ) {
        const ExceptInfo* info = bodyInfo->m_exception[i].get();

        assert( offsetToInstruction.get( info->m_from, NULL ) );
        assert( offsetToInstruction.get( info->m_to, NULL ) );
        assert( offsetToInstruction.get( info->m_target + 1, NULL ) );

        exceptions[i]->setFrom( offsetToInstruction[info->m_from] );
        exceptions[i]->setTo( offsetToInstruction[info->m_to] );
        exceptions[i]->setTarget( offsetToInstruction[info->m_target + 1] - 1 );
    }

    return result;
}

} // namespace avm2