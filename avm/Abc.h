/**************************************************************************

 The MIT License (MIT)

 Copyright (c) 2008 Vitaly Alexeev <tishka92@yahoo.com>
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

#ifndef avm2_ABC_H
#define avm2_ABC_H

#include "Common.h"

namespace avm2 {

    class Domain;

	class  AbcInfo;
	struct movie_definition_sub;

    // ** enum ConstantKind
    enum ConstantKind {
        ConstTypeInt                 = 0x03,
        ConstTypeUInt                = 0x04,
        ConstTypeDouble              = 0x06,
        ConstTypeUtf8                = 0x01,
        ConstTypeTrue                = 0x0B,
        ConstTypeFalse               = 0x0A,
        ConstTypeNull                = 0x0C,
        ConstTypeUndefined           = 0x00,
        ConstTypeNamespace           = 0x08,
        ConstTypePackageNamespace    = 0x16,
        ConstTypePackageInternalNs   = 0x17,
        ConstTypeProtectedNamespace  = 0x18,
        ConstTypeExplicitNamespace   = 0x19,
        ConstTypeStaticProtectedNs   = 0x1A,
        ConstTypePrivateNs           = 0x05
    };

    // ** struct MultinameInfo
	struct MultinameInfo {
        // ** enum Kind
		enum Kind {
			Undefined      = 0,
			QName          = 0x07,
			QNameA         = 0x0D,
			RTQName        = 0x0F,
			RTQNameA       = 0x10,
			RTQNameL       = 0x11,
			RTQNameLA      = 0x12,
			Multiname      = 0x09,
			MultinameA     = 0x0E,
			MultinameL     = 0x1B,
			MultinameLA    = 0x1C,
            Typename       = 0x1D
		};

        int     m_index;
		Kind    m_kind;
		int     m_flags;
		int     m_ns;
		int     m_ns_set;
		int     m_name;

        int     m_type_name;
        int     m_type_arg;
        int     m_type_arg_count;

		MultinameInfo( int index = 0 ) : m_index( index ), m_kind( Undefined ), m_flags( 0 ), m_ns( 0 ), m_name( 0 )
		{
		}

        // ** isQualified
        bool isQualified( void ) const
		{
			return m_kind == QName || m_kind == Multiname;
		}
	};

    // ** struct NamespaceInfo
	struct NamespaceInfo {
        // ** enum Kind
		enum Kind
		{
			Undefined          = 0,
			Namespace          = 0x08,
			PackageNamespace   = 0x16,
			PackageInternalNs  = 0x17,
			ProtectedNamespace = 0x18,
			ExplicitNamespace  = 0x19,
			StaticProtectedNs  = 0x1A,
			PrivateNs          = 0x05
		};

		Kind    m_kind;
		int     m_name;

		NamespaceInfo() : m_kind( Undefined ), m_name(0)
		{
		}
	};

    // ** struct TraitsInfo
	struct TraitsInfo : public ref_counted {
        // ** enum Kind
		enum Kind {
			Slot      = 0,
			Method    = 1,
			Getter    = 2,
			Setter    = 3,
			Class     = 4,
			Function  = 5,
			Const     = 6
		};

        // ** enum Attrib
		enum Attrib {
			ATTR_Final      = 0x1,
			ATTR_Override   = 0x2,
			ATTR_Metadata   = 0x4
		};

		int     m_name;
		Uint8   m_kind;
		Uint8   m_attr;

		// data
		union {
			struct {
				int             m_slot_id;
				int             m_type_name;
				int             m_vindex;
				ConstantKind    m_vkind;
			} m_slot;

			struct {
				int m_slot_id;
				int m_classi;
			} m_cls;

			struct {
				int m_slot_id;
				int m_function;
			} m_function;

			struct {
				int m_disp_id;
				int m_method;
			} m_method;
		};


		array<int> m_metadata;
	};

    // ** struct ExceptInfo
    struct ExceptInfo : public ref_counted {
		int     m_from;
		int     m_to;
		int     m_target;
		int     m_exc_type;
		int     m_var_name;
	};
    
    typedef gc_array<gc_ptr<TraitsInfo> >   TraitsArray;
    typedef gc_array<gc_ptr<ExceptInfo> >   ExceptArray;
    typedef array<int>                      NsSetInfo;

    // ** struct MetadataItem
    struct MetadataItem {
        int     m_key;
        int     m_value;
    };

    // ** struct MetadataInfo
	struct MetadataInfo : public ref_counted
	{
        int                 m_name;
        array<MetadataItem> m_items;
	};

    // ** struct BodyInfo
    struct BodyInfo : public ref_counted {
		int             m_init_scope_depth;
		int             m_max_scope_depth;
		int             m_max_stack;
		int             m_local_count;
		membuf          m_code;
		ExceptArray     m_exception;
		TraitsArray     m_trait;
    };

    // ** struct MethodInfo
    struct MethodInfo : public ref_counted {
        // ** enum Flags
		enum Flags {
			NEED_ARGUMENTS  = 0x01,
			NEED_ACTIVATION = 0x02,
			NEED_REST       = 0x04,
			HAS_OPTIONAL    = 0x08,
			SET_DXNS        = 0x40,
			HAS_PARAM_NAMES = 0x80
		};

        // ** struct OptionDetail
        struct OptionDetail {
            int             m_index;
            ConstantKind    m_kind;
        };

        int                     m_index;
		int                     m_return_type;
		array<int>              m_param_type;
        array<int>              m_param_name;
		int                     m_name;
		Uint8                   m_flags;
		array<OptionDetail>     m_options;
		BodyInfo                m_body;

        MethodInfo( int index ) : m_index( index )
        {
        }
    };

    // ** struct InstanceInfo
	struct InstanceInfo : public ref_counted {
        // ** enum Flags
		enum Flags
		{
			ClassSealed        = 0x01,
			ClassFinal         = 0x02,
			ClassInterface     = 0x04,
			ClassProtectedNs   = 0x08
		};

        int                 m_index;
		int                 m_name;
		int                 m_super_name;
		Uint8               m_flags;
		int                 m_protectedNs;
		array<int>          m_interface;
		int                 m_iinit;
		TraitsArray         m_trait;
		weak_ptr<AbcInfo>   m_abc;

		InstanceInfo( int index ) : m_index( index ), m_name( 0 ), m_super_name( 0 ), m_flags( 0 ), m_protectedNs( 0 ), m_iinit( 0 )
		{
		}
	};

    // ** struct ClassInfo
	struct ClassInfo : public ref_counted {
		weak_ptr<AbcInfo>   m_abc;
		int                 m_cinit;
		TraitsArray         m_trait;
	};

    // ** struct ScriptInfo
	struct ScriptInfo : public ref_counted {
		int             m_init;
		TraitsArray     m_trait;
	};

    // ** struct AbcInfo
	class AbcInfo : public ref_counted {
	public:

    //    weak_ptr<player>                    m_player;
        
		// constant pool
		array<int>                          m_integer;
		array<Uint32>                       m_uinteger;
		array<double>                       m_double;
		array<tu_string>                    m_string;
		array<NamespaceInfo>                m_namespace;
		array<NsSetInfo>                    m_ns_set;
		array<MultinameInfo>                m_multiname;

        gc_array<gc_ptr<MethodInfo> >       m_method;
        gc_array<gc_ptr<BodyInfo> >         m_body;
		gc_array<gc_ptr<MetadataInfo> >     m_metadata;
		gc_array<gc_ptr<InstanceInfo> >     m_instance;
		gc_array<gc_ptr<ClassInfo> >        m_class;
		gc_array<gc_ptr<ScriptInfo> >       m_script;
    /*
		inline const char* get_string(int index) const
		{
			return m_string[index].c_str(); 
		}

		inline int get_integer(int index) const
		{
			return m_integer[index]; 
		}

		inline double get_double(int index) const
		{
			return m_double[index]; 
		}

		inline const char* get_namespace(int index) const
		{
			return get_string(m_namespace[index].m_name); 
		}

		inline const char* get_multiname(int index) const
		{
			return get_string(m_multiname[index].m_name); 
		}

		inline MultinameInfo::Kind get_multiname_type(int index) const
		{
			return m_multiname[index].m_kind;
		}

		inline const char* get_multiname_namespace(int index) const
		{
			const MultinameInfo & mn = m_multiname[index];

			switch( mn.m_kind )
			{
				case MultinameInfo::CONSTANT_QName:
					return get_namespace(mn.m_ns);
				case MultinameInfo::CONSTANT_Multiname:
				case MultinameInfo::CONSTANT_MultinameA:
					return get_namespace( mn.m_ns );
				default:
					log_msg( "implement get_multiname_namespace for this kind %i\n", mn.m_kind );
					return NULL;
			} 
		}

		as_function* get_script_function( const tu_string & name = "" ) const;

		inline as_function* get_class_function( const int class_index ) const
		{
            assert(false);
            return NULL;
		//	return m_method[m_class[class_index]->m_cinit].get();
		}*/

                            AbcInfo();
		virtual             ~AbcInfo();

		void                read(stream* in, movie_definition_sub* m);
		void                readCPool(stream* in);
        void                readTraits(stream* in, TraitsInfo& info);
        void                readInstance(stream* in, InstanceInfo& info);
        void                readClass(stream* in, ClassInfo& info);
        void                readScript(stream* in, ScriptInfo& info);
        void                readBody(stream* in, BodyInfo& info);
        void                readMethod(stream* in, MethodInfo& info);
        void                readExcept(stream* in, ExceptInfo& info);
        void                readMetadata(stream* in, MetadataInfo& info);

/*
		inline const char * get_super_class(tu_string& name) const
		{
			return get_multiname( get_instance_info( name )->m_super_name );
		}

		const char * get_class_from_constructor(int method);

		// get class constructor
		as_function* get_class_constructor(const tu_string& name) const;

		// find instance info by name
		InstanceInfo* get_instance_info(const tu_string& class_name) const;
		ClassInfo* get_class_info(const tu_string& full_class_name) const;
		ClassInfo* get_class_info(int class_index) const;
*/
	};
}


#endif // avm2_ABC_H
