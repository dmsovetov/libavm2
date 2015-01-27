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

#ifndef avm2_Instructions_h
#define avm2_Instructions_h

#include    "Common.h"

namespace avm2
{
    // ** enum OpeCode
    enum OpCode {
        Add                 = 0xa0,
        AddI                = 0xc5,
        AsType              = 0x86,
        AsTypeLate          = 0x87,
        BitAnd              = 0xa8,
        BitNot              = 0x97,
        BitOr               = 0xa9,
        BitXOR              = 0xaa,
        Call                = 0x41, // arg_count : u30
        CallMethod          = 0x43, // index : u30, arg_count : u30
        CallProperty        = 0x46, // index : u30, arg_count : u30
        CallPropLex         = 0x4c, // index : u30, arg_count : u30
        CallPropVoid        = 0x4f, // index : u30, arg_count : u30
        CallStatic          = 0x44, // index : u30, arg_count : u30
        CallSuper           = 0x45, // index : u30, arg_count : u30
        CallSuperVoid       = 0x4e, // index : u30, arg_count : u30
        CheckFilter         = 0x78,
        Coerce              = 0x80, // index : u30
        CoerceToAny         = 0x82,
        CoerceToString      = 0x85,
        Construct           = 0x42, // arg_count : u30
        ConstructProp       = 0x4a, // index : u30, arg_count : u30
        ConstructSuper      = 0x49, // arg_count : u30
        ConvertToBool       = 0x76,
        ConvertToInt        = 0x73,
        ConvertToDouble     = 0x75,
        ConvertToObject     = 0x77,
        ConvertToUInt       = 0x74,
        ConvertToString     = 0x70,
        Debug               = 0xef, // debug_type : u8, index : u30, reg : u8, extra : u30
        DebugFile           = 0xf1, // index : u30
        DebugLine           = 0xf0, // line : u30
        DecLocal            = 0x94, // index : u30
        DecLocalI           = 0xc3,
        Decrement           = 0x93,
        DecrementI          = 0xc1,
        DeleteProperty      = 0x6a, // index : u30
        Divide              = 0xa3,
        Dup                 = 0x2a,
        DXNS                = 0x06, // index : u30
        DXNSLate            = 0x07,
        Equals              = 0xab,
        EscXAttr            = 0x72,
        EscXElem            = 0x71,
        FindProperty        = 0x5e, // index : u30
        FindPropertyStrict  = 0x5d, // index : u30
        GetDescendants      = 0x59, // index : u30
        GetGlobalScope      = 0x64,
        GetGlobalSlot       = 0x6e, // slot_index : u30
        GetLex              = 0x60, // index : u30
        GetLocal            = 0x62, // index : u30
        GetLocal0           = 0xd0,
        GetLocal1           = 0xd1,
        GetLocal2           = 0xd2,
        GetLocal3           = 0xd3,
        GetProperty         = 0x66, // index : u30
        GetScopeObject      = 0x65, // index : u8
        GetSlot             = 0x6c, // slot_index : u30
        GetSuper            = 0x04, // index : u30
        GreaterEquals       = 0xb0,
        GreaterThan         = 0xaf,
        HasNext             = 0x1f,
        HasNext2            = 0x32, // object_reg : uint, index_reg : uint
        IfEqual             = 0x13, // offset : s24
        IfFalse             = 0x12, // offset : s24
        IfGreaterEqual      = 0x18, // offset : s24
        IfGreater           = 0x17, // offset : s24
        IfLessEqual         = 0x16, // offset : s24
        IfLess              = 0x15, // offset : s24
        IfNotGreaterEqual   = 0x0f, // offset : s24
        IfNotGreater        = 0x0e, // offset : s24
        IfNotLessEqual      = 0x0d, // offset : s24
        IfNotLess           = 0x0c, // offset : s24
        IfNotEqual          = 0x14, // offset : s24
        IfStrictEqual       = 0x19, // offset : s24
        IfStictNotEqual     = 0x1a, // offset : s24
        IfTrue              = 0x11, // offset : s24
        In                  = 0xb4,
        IncLocal            = 0x92, // index : u30
        IncLocalI           = 0xc2, // index : u30
        Increment           = 0x91,
        IncrementI          = 0xc0,
        InitProperty        = 0x68, // index : u30
        InstanceOf          = 0xb1,
        IsType              = 0xb2, // index : u30
        IsTypeLate          = 0xb3,
        Jump                = 0x10, // offset : s24
        Kill                = 0x08, // index : u30
        Label               = 0x09,
        LessEquals          = 0xae,
        LessThan            = 0xad,
        LookupSwitch        = 0x1b, // default_offset : s24, case_count : u30, case_offsets... : u24
        LeftShift           = 0xa5,
        Modulo              = 0xa4,
        Multiply            = 0xa2,
        MultiplyI           = 0xc7,
        Negate              = 0x90,
        NegateI             = 0xc4,
        NewActivation       = 0x57,
        NewArray            = 0x56, // arg_count : u30
        NewCatch            = 0x5a, // index : u30
        NewClass            = 0x58, // index : u30
        NewFunction         = 0x40, // index : u30
        NewObject           = 0x55, // arg_count : u30
        NextName            = 0x1e,
        NextValue           = 0x23,
        Nop                 = 0x02,
        Not                 = 0x96,
        Pop                 = 0x29,
        PopScope            = 0x1d,
        PushByte            = 0x24, // value : u8
        PushDouble          = 0x2f, // index : u30
        PushFalse           = 0x27,
        PushInt             = 0x2d, // index : u30
        PushNamespace       = 0x31, // index : u30
        PushNaN             = 0x28,
        PushNull            = 0x20,
        PushScope           = 0x30,
        PushShort           = 0x25, // value : u30
        PushString          = 0x2c, // index : u30
        PushTrue            = 0x26,
        PushUInt            = 0x2e, // index : u30
        PushUndefined       = 0x21,
        PushWith            = 0x1c,
        ReturnValue         = 0x48,
        ReturnVoid          = 0x47,
        RightShift          = 0xa6,
        SetLocal            = 0x63, // index : u30
        SetLocal0           = 0xd4,
        SetLocal1           = 0xd5,
        SetLocal2           = 0xd6,
        SetLocal3           = 0xd7,
        SetGlobalSlot       = 0x6f, // slot_index : u30
        SetProperty         = 0x61, // index : u30
        SetSlot             = 0x6d, // slot_index : u30
        SetSuper            = 0x05, // index : u30
        StrictEquals        = 0xac,
        Subtract            = 0xa1,
        SubtractI           = 0xc6,
        Swap                = 0x2b,
        Throw               = 0x03,
        TypeOf              = 0x95,
        URightShift         = 0xa7,

        // ** Undcoumented
        ApplyType           = 0x53,
        
        OpCodeTotal
    };

    // ** struct Instruction
    struct Instruction
    {
        OpCode     opCode;
        
        union {
            struct {
                int     dbgType;
                int     dbgReg;
                int     dbgExtra;
                int     dbgIndex;
            };

            struct {
                int     defaultOffset;
                int     caseCount;
                int     *caseOffsets;
            };
            
            struct {
                int     objectReg;
                int     indexReg;
            };
            
            int             offset;
            int             line;
        };
        
        union {
            Name*               Identifier;
            Class*              Class;
            const String*       Str;
            const Function*     Function;
            const Exception*    Exception;
            double              Number;
            unsigned int        UInt;
        };
        
        union {
            int             ArgCount;
            int             Integer;
        };
    };
    
    typedef array<Instruction> Instructions;
}

#endif
