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

#include "Dump.h"
#include "Instructions.h"
#include "Abc.h"

namespace avm2
{
    
Dump::Dump( void )
{
        
}
    
void Dump::dumpABC( const AbcInfo* abc )
{
    m_abc = abc;
}
    
const char* Dump::formatNamespaceKind( int kind )
{
    switch( kind ) {
        case NamespaceInfo::Namespace:             return "namespace";
        case NamespaceInfo::PackageNamespace:      return "package";
        case NamespaceInfo::PackageInternalNs:     return "internal";
        case NamespaceInfo::ProtectedNamespace:    return "protected";
        case NamespaceInfo::ExplicitNamespace:     return "explicit";
        case NamespaceInfo::StaticProtectedNs:     return "staticProtected";
        case NamespaceInfo::PrivateNs:             return "private";
    }
    
    return "unknown";
}

Str Dump::formatNamespace( const AbcInfo* abc, int index, bool nameOnly )
{
    const NamespaceInfo& ns = abc->m_namespace[index];
    
    char buf[256];
    
    const Str& str = abc->m_string[ns.m_name];

    if( nameOnly ) {
        sprintf( buf, "%s", str.size() == 0 ? "" : str.c_str() );
    } else {
        sprintf( buf, "%s : %s", formatNamespaceKind( ns.m_kind ), str.size() == 0 ? "" : str.c_str() );
    }
    
    return buf;
}

Str Dump::formatMethod( const AbcInfo* abc, int index )
{
    const MethodInfo& mi = *abc->m_method[index];
    
    char buf[256];
    
    Str name = formatString( abc, mi.m_name );
    Str type = formatMultiname( abc, mi.m_return_type, true );
    
    sprintf( buf, "name=%d(%s), type=%d(%s)", mi.m_name, name.c_str(), mi.m_return_type, type.c_str() );
    
    return buf;
}

Str Dump::formatNSSet( const char* name, const AbcInfo* abc, int index )
{
    const NsSetInfo& nss = abc->m_ns_set[index];
    
    Str result;
    
    for( int i = 0; i < nss.size(); i++ ) {
        if( i > 0 ) {
            result += ", ";
        }
        
        result += formatNamespace( abc, nss[i], true );
        result += ":";
        result += formatNamespaceKind( abc->m_namespace[nss[i]].m_kind );
        result += ".";
        result += name;
    }
    
    return result;
}

Str Dump::formatClass( const AbcInfo* abc, int index )
{
    const ClassInfo& cls = *abc->m_class[index];
    char buf[256];
    sprintf( buf, "%s", formatMethod( abc, cls.m_cinit ).c_str() );
    
    return buf;
}

Str Dump::formatMultiname( const AbcInfo* abc, int index, bool nameOnly )
{
    if( index == 0 ) {
        return "*";
    }
    
    assert( index < abc->m_multiname.size() );
    const MultinameInfo& mn = abc->m_multiname[index];
    
    char buf[512];
    
    switch( mn.m_kind ) {
        case MultinameInfo::QNameA:
        case MultinameInfo::QName:
        {
        //    assert( nameOnly == false );
        //    if( nameOnly ) {
        //        const NamespaceInfo& ns     = abc->m_namespace[mn.m_ns];
        //        const Str&           nsName = abc->m_string[ns.m_name];
        //
        //        sprintf( buf, "%s%s%s", nsName.c_str(), nsName.size() > 0 ? "." : "", formatString( abc, mn.m_name ) );
        //    } else {
                sprintf( buf, "%s.%s", formatNamespace( abc, mn.m_ns, true ).c_str(), formatString( abc, mn.m_name ) );
        //    }
        }
            break;
            
        case MultinameInfo::RTQName:
        case MultinameInfo::RTQNameA:
        {
        //    if( nameOnly ) {
        //        sprintf( buf, "%s", formatString( abc, mn.m_name ) );
        //    } else {
                sprintf( buf, "kind=%d(%s), name=%d(%s)", mn.m_kind, formatMultinameKind( mn.m_kind ), mn.m_name, formatString( abc, mn.m_name ) );
        //    }
        }
            break;
            
        case MultinameInfo::RTQNameL:
        case MultinameInfo::RTQNameLA:
        {
        //    if( nameOnly ) {
        //        sprintf( buf, "-" );
        //    } else {
                sprintf( buf, "kind=%d(%s)", mn.m_kind, formatMultinameKind( mn.m_kind ) );
        //    }
        }
            break;
            
            
            
        case MultinameInfo::Multiname:
        case MultinameInfo::MultinameA:
        {
        //    if( nameOnly ) {
        //        sprintf( buf, "%s", formatString( abc, mn.m_name ) );
        //    } else {
                sprintf( buf, "%s", formatNSSet( formatString( abc, mn.m_name ), abc, mn.m_ns_set ).c_str() );
        //    }
        }
            break;
            
            
        case MultinameInfo::MultinameL:
        case MultinameInfo::MultinameLA:
        {
        //    if( nameOnly ) {
        //        sprintf( buf, "-" );
        //    } else {
                sprintf( buf, "kind=%d(%s), nsSet=%d(%s)", mn.m_kind, formatMultinameKind( mn.m_kind ), mn.m_ns_set, formatNSSet( "", abc, mn.m_ns_set ).c_str() );
        //    }
        }
            break;
    }
    
    
    return buf;
}

const char* Dump::formatMultinameKind( int kind )
{
    switch( kind ) {
        case MultinameInfo::QName:          return "QName";         break;
        case MultinameInfo::QNameA:         return "QNameA";        break;
        case MultinameInfo::RTQName:        return "RTQName";       break;
        case MultinameInfo::RTQNameA:       return "RTQNameA";      break;
        case MultinameInfo::RTQNameL:       return "RTQNameL";      break;
        case MultinameInfo::RTQNameLA:      return "RTQNameLA";     break;
        case MultinameInfo::Multiname:      return "Multiname";     break;
        case MultinameInfo::MultinameA:     return "MultinameA";    break;
        case MultinameInfo::MultinameL:     return "MultinameL";    break;
        case MultinameInfo::MultinameLA:    return "MultinameLA";   break;
    }
    
    return "unknown";
}

const char* Dump::formatOpCode( int opcode )
{
    switch( opcode ) {
        case Call: return "Call";
        case CallSuperVoid: return "CallSuperVoid";
        case CallSuper: return "CallSuper";
        case CallStatic: return "CallStatic";
        case CallPropVoid: return "CallPropVoid";
        case CallPropLex: return "CallPropLex";
        case CallProperty: return "CallProperty";
        case CallMethod: return "CallMethod";
        case Construct: return "Construct";
        case ConstructProp: return "ConstructProp";
        case Coerce: return "Coerce";
        case ConstructSuper: return "ConstructSuper";
        case Debug: return "Debug";
        case DebugFile: return "DebugFile";
        case DebugLine: return "DebugLine";
        case DecLocal: return "DecLocal";
        case DeleteProperty: return "DeleteProperty";
        case DXNS: return "DXNS";
        case FindProperty: return "FindProperty";
        case FindPropertyStrict: return "FindPropertyStrict";
        case GetDescendants: return "GetDescendants";
        case GetGlobalSlot: return "GetGlobalSlot";
        case GetLex: return "GetLex";
        case GetLocal: return "GetLocal";
        case GetProperty: return "GetProperty";
        case GetScopeObject: return "GetScopeObject";
        case GetSlot: return "GetSlot";
        case GetSuper: return "GetSuper";
        case HasNext2: return "HasNext2";
        case IfEqual: return "IfEqual";
        case IfFalse: return "IfFalse";
        case IfGreaterEqual: return "IfGreaterEqual";
        case IfGreater: return "IfGreater";
        case IfLessEqual: return "IfLessEqual";
        case IfLess: return "IfLess";
        case IfNotGreaterEqual: return "IfNotGreaterEqual";
        case IfNotGreater: return "IfNotGreater";
        case IfNotLessEqual: return "IfNotLessEqual";
        case IfNotLess: return "IfNotLess";
        case IfNotEqual: return "IfNotEqual";
        case IfStrictEqual: return "IfStrictEqual";
        case IfStictNotEqual: return "IfStictNotEqual";
        case IfTrue: return "IfTrue";
        case IncLocal: return "IncLocal";
        case IncLocalI: return "IncLocalI";
        case InitProperty: return "InitProperty";
        case IsType: return "IsType";
        case Jump: return "Jump";
        case Kill: return "Kill";
        case LookupSwitch: return "LookupSwitch";
        case NewArray: return "NewArray";
        case NewCatch: return "NewCatch";
        case NewClass: return "NewClass";
        case NewFunction: return "NewFunction";
        case NewObject: return "NewObject";
        case PushByte: return "PushByte";
        case PushDouble: return "PushDouble";
        case PushInt: return "PushInt";
        case PushNamespace: return "PushNamespace";
        case PushShort: return "PushShort";
        case PushString: return "PushString";
        case PushUInt: return "PushUInt";
        case SetLocal: return "SetLocal";
        case SetGlobalSlot: return "SetGlobalSlot";
        case SetProperty: return "SetProperty";
        case SetSlot: return "SetSlot";
        case SetSuper: return "SetSuper";
        case PushTrue: return "PushTrue";
        case PushUndefined: return "PushUndefined";
        case PushWith: return "PushWith";
        case ReturnValue: return "ReturnValue";
        case ReturnVoid: return "ReturnVoid";
        case RightShift: return "RightShift";
        case SetLocal0: return "SetLocal0";
        case SetLocal1: return "SetLocal1";
        case SetLocal2: return "SetLocal2";
        case SetLocal3: return "SetLocal3";
        case GetGlobalScope: return "GetGlobalScope";
        case Add: return "Add";
        case AddI: return "AddI";
        case AsType: return "AsType";
        case AsTypeLate: return "AsTypeLate";
        case BitAnd: return "BitAnd";
        case BitNot: return "BitNot";
        case BitOr: return "BitOr";
        case BitXOR: return "BitXOR";
        case CheckFilter: return "CheckFilter";
        case CoerceToAny: return "CoerceToAny";
        case CoerceToString: return "CoerceToString";
        case ConvertToBool: return "ConvertToBool";
        case ConvertToInt: return "ConvertToInt";
        case ConvertToDouble: return "ConvertToDouble";
        case ConvertToObject: return "ConvertToObject";
        case ConvertToUInt: return "ConvertToUInt";
        case ConvertToString: return "ConvertToString";
        case DecLocalI: return "DecLocalI";
        case Decrement: return "Decrement";
        case DecrementI: return "DecrementI";
        case Divide: return "Divide";
        case Dup: return "Dup";
        case DXNSLate: return "DXNSLate";
        case Equals: return "Equals";
        case EscXAttr: return "EscXAttr";
        case EscXElem: return "EscXElem";
        case GetLocal0: return "GetLocal0";
        case GetLocal1: return "GetLocal1";
        case GetLocal2: return "GetLocal2";
        case GetLocal3: return "GetLocal3";
        case GreaterEquals: return "GreaterEquals";
        case GreaterThan: return "GreaterThan";
        case HasNext: return "HasNext";
        case In: return "In";
        case Increment: return "Increment";
        case IncrementI: return "IncrementI";
        case InstanceOf: return "InstanceOf";
        case IsTypeLate: return "IsTypeLate";
        case Label: return "Label";
        case LessEquals: return "LessEquals";
        case LessThan: return "LessThan";
        case LeftShift: return "LeftShift";
        case Modulo: return "Modulo";
        case Multiply: return "Multiply";
        case MultiplyI: return "MultiplyI";
        case Negate: return "Negate";
        case NegateI: return "NegateI";
        case NewActivation: return "NewActivation";
        case NextName: return "NextName";
        case NextValue: return "NextValue";
        case Nop: return "Nop";
        case Not: return "Not";
        case Pop: return "Pop";
        case PopScope: return "PopScope";
        case PushFalse: return "PushFalse";
        case PushNaN: return "PushNaN";
        case PushNull: return "PushNull";
        case PushScope: return "PushScope";
        case Throw: return "Throw";
        case Swap: return "Swap";
        case TypeOf: return "TypeOf";
        case Subtract: return "Subtract";
        case SubtractI: return "SubtractI";
        case StrictEquals: return "StrictEquals";
        case URightShift: return "URightShift";
        case ApplyType: return "ApplyType";
        default: AVM2_VERBOSE( "Dump::formatOpCode : unhandled opcode %x\n", opcode );
    }
    
    assert( false );
    
    return "unknown opcode";
}

const char* Dump::formatString( const AbcInfo* abc, int str )
{
    return abc->m_string[str].c_str();
}

} // namespace avm2