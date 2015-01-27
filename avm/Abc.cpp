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

#include "Abc.h"

namespace avm2
{

//	void MetadataInfo::read(stream* in, AbcInfo* abc)
//	{
//		assert(0&&"todo");
//	}

	//	traits_info
	//	{
	//		u30 name
	//		u8 kind
	//		u8 data[]
	//		u30 metadata_count
	//		u30 metadata[metadata_count]
	//	}
	void AbcInfo::readTraits(stream* in, TraitsInfo& traits)
	{
		// The value can not be zero, and the multiname entry specified must be a QName.
		traits.m_name = in->read_vu30();
		assert(traits.m_name != 0 && m_multiname[traits.m_name].isQualified());
	//	IF_VERBOSE_PARSE(log_msg("	traits: name='%s'\n", get_multiname(traits.m_name)));
		
		Uint8 b       = in->read_u8();
		traits.m_kind = b & 0x0F;
		traits.m_attr = b >> 4;

		switch (traits.m_kind)
		{
			case TraitsInfo::Slot :
			case TraitsInfo::Const :

				traits.m_slot.m_slot_id = in->read_vu30();

				traits.m_slot.m_type_name = in->read_vu30();
				assert(traits.m_slot.m_type_name < m_multiname.size());

				traits.m_slot.m_vindex = in->read_vu30();

				if (traits.m_slot.m_vindex != 0)
				{
					// This field exists only when vindex is non-zero.
					traits.m_slot.m_vkind = static_cast<ConstantKind>( in->read_u8() );
				}
				break;

			case TraitsInfo::Class :

				traits.m_cls.m_slot_id = in->read_vu30();

				traits.m_cls.m_classi = in->read_vu30();
				assert(traits.m_cls.m_classi < m_class.size());
				break;

			case TraitsInfo::Function :

				traits.m_function.m_slot_id = in->read_vu30();

				traits.m_function.m_function = in->read_vu30();
				assert(traits.m_function.m_function < m_method.size());
				break;

			case TraitsInfo::Method :
			case TraitsInfo::Getter :
			case TraitsInfo::Setter :

				traits.m_method.m_disp_id = in->read_vu30();

				traits.m_method.m_method = in->read_vu30();
				assert(traits.m_method.m_method < m_method.size());
				break;

			default:
				assert(false && "invalid kind of traits");
				break;
		}

		if (traits.m_attr & TraitsInfo::ATTR_Metadata)
		{
			int n = in->read_vu30();
			traits.m_metadata.resize(n);
			for (int i = 0; i < n; i++)
			{
				traits.m_metadata[i] = in->read_vu30();
			}
		}
	}

	//	instance_info
	//		{
	//		u30 name
	//		u30 super_name
	//		u8 flags
	//		u30 protectedNs
	//		u30 intrf_count
	//		u30 interface[intrf_count]
	//		u30 iinit
	//		u30 trait_count
	//		traits_info trait[trait_count]
	//	}
	void AbcInfo::readInstance(stream* in, InstanceInfo& instance)
	{
		instance.m_abc        = this;
		instance.m_name       = in->read_vu30();
		instance.m_super_name = in->read_vu30();

		instance.m_flags = in->read_u8();
		if (instance.m_flags & InstanceInfo::ClassProtectedNs)
		{
			instance.m_protectedNs = in->read_vu30();
		}

		int i, n;
		n = in->read_vu30();
		instance.m_interface.resize(n);
		for (i = 0; i < n; i++)
		{
			instance.m_interface[i] = in->read_vu30();
		}

		instance.m_iinit = in->read_vu30();

	//	IF_VERBOSE_PARSE(log_msg("  name='%s', supername='%s', ns='%s'\n",
	//		get_multiname(instance.m_name), get_multiname(instance.m_super_name), get_namespace(instance.m_protectedNs)));

		n = in->read_vu30();
		instance.m_trait.resize(n);
		for (i = 0; i < n; i++)
		{
			TraitsInfo* trait = new TraitsInfo();
            readTraits(in, *trait);
			instance.m_trait[i] = trait;
		}
	}

	void AbcInfo::readClass(stream* in, ClassInfo& cls)
	{
		// This is an index into the method array of the abcFile
        cls.m_abc   = this;
		cls.m_cinit = in->read_vu30();
		assert(cls.m_cinit < m_method.size());

		int n = in->read_vu30();
		cls.m_trait.resize(n);
		for (int i = 0; i < n; i++)
		{
			TraitsInfo* trait = new TraitsInfo();
			readTraits(in, *trait);
			cls.m_trait[i] = trait;
		}
	}

	void AbcInfo::readScript(stream* in, ScriptInfo& script)
	{
		// The init field is an index into the method array of the abcFile
		script.m_init = in->read_vu30();
		assert(script.m_init < m_method.size());

		int n = in->read_vu30();
		script.m_trait.resize(n);
		for (int i = 0; i < n; i++)
		{
			TraitsInfo* trait = new TraitsInfo();
			readTraits(in, *trait);
			script.m_trait[i] = trait;
		}
	}
    
	// exception_info
	// {
	//		u30 from
	//		u30 to
	//		u30 target
	//		u30 exc_type
	//		u30 var_name
	// }
	void AbcInfo::readExcept(stream *in, ExceptInfo& info)
	{
		info.m_from     = in->read_vu30();
		info.m_to       = in->read_vu30();
		info.m_target   = in->read_vu30();
		info.m_exc_type = in->read_vu30();
		info.m_var_name = in->read_vu30();
	}

	AbcInfo::AbcInfo(/*player* player*/) /*: m_player( player )*/
	{
	}

	AbcInfo::~AbcInfo()
	{
	}

	//	abcFile
	//	{
	//		u16 minor_version
	//		u16 major_version
	//		cpool_info cpool_info
	//		u30 method_count
	//		method_info method[method_count]
	//		u30 metadata_count
	//		metadata_info metadata[metadata_count]
	//		u30 class_count
	//		instance_info instance[class_count]
	//		class_info class[class_count]
	//		u30 script_count
	//		script_info script[script_count]
	//		u30 method_body_count
	//		method_body_info method_body[method_body_count]
	//	}
	void	AbcInfo::read(stream* in, movie_definition_sub* m)
	{
		int eof = m ? in->get_tag_end_position() : 0;
		int i, n;

		Uint16 minor_version = in->read_u16();
		Uint16 major_version = in->read_u16();
		assert(minor_version == 16 && major_version == 46);

		// read constant pool
		readCPool(in);
		assert(m == NULL || in->get_position() < eof);

		// read method_info
		n = in->read_vu30();
		m_method.resize(n);
		IF_VERBOSE_PARSE(logger::msg("method_info count: %d\n", n));
		for (i = 0; i < n; i++)
		{
            MethodInfo* info = new MethodInfo( i );
            readMethod(in, *info);
            m_method[i] = info;
		//	as_3_function* info = new as_3_function(this, i, m->get_player());
		//	info->read(in);
		//	m_method[i] = info;
		}

		assert(m == NULL || in->get_position() < eof);

		// read metadata_info
		n = in->read_vu30();
		m_metadata.resize(n);
		IF_VERBOSE_PARSE(logger::msg("metadata_info count: %d\n", n));
		for (i = 0; i < n; i++)
		{
			MetadataInfo* info = new MetadataInfo();
            readMetadata(in, *info);
			m_metadata[i] = info;
		}

		assert(m == NULL || in->get_position() < eof);

		// read instance_info & class_info
		n = in->read_vu30();
		m_instance.resize(n);
		IF_VERBOSE_PARSE(logger::msg("instance_info count: %d\n", n));
		for (i = 0; i < n; i++)
		{
			IF_VERBOSE_PARSE(logger::msg("instance_info[%d]:\n", i));

			InstanceInfo* info = new InstanceInfo( i );
			readInstance(in, *info);
			m_instance[i] = info;
		}

		assert(m == NULL || in->get_position() < eof);

		// class_info
		m_class.resize(n);
		IF_VERBOSE_PARSE(logger::msg("class_info count: %d\n", n));
		for (i = 0; i < n; i++)
		{
			IF_VERBOSE_PARSE(logger::msg("class_info[%d]\n", i));
			ClassInfo* info = new ClassInfo();
            readClass(in, *info);
			m_class[i] = info;
		}

		assert(m == NULL || in->get_position() < eof);

		// read script_info
		n = in->read_vu30();
		m_script.resize(n);
		IF_VERBOSE_PARSE(logger::msg("script_info count: %d\n", n));
		for (i = 0; i < n; i++)
		{
			IF_VERBOSE_PARSE(logger::msg("script_info[%d]\n", i));
			ScriptInfo* info = new ScriptInfo();
			readScript(in, *info);
			m_script[i] = info;
		}

		assert(m == NULL || in->get_position() < eof);

		// read body_info
		n = in->read_vu30();
		for (i = 0; i < n; i++)
		{
			int method_index = in->read_vu30();
            readBody(in, m_method[method_index]->m_body);
		}

		assert(m == NULL || in->get_position() == eof);
	}

    void AbcInfo::readMetadata(stream *in, MetadataInfo& info)
    {
        info.m_name = in->read_vu30();
        int count = in->read_vu30();

        for( int j = 0; j < count; j++ ) {
            MetadataItem item;

            item.m_key   = in->read_vu30();
            item.m_value = in->read_vu30();

            info.m_items.push_back( item );
        }
    }

    void AbcInfo::readMethod(stream *in, MethodInfo& info)
    {
        int param_count = in->read_vu30();

		// The return_type field is an index into the multiname
		info.m_return_type = in->read_vu30();

		info.m_param_type.resize(param_count);
		for (int i = 0; i < param_count; i++)
		{
			info.m_param_type[i] = in->read_vu30();
		}

		info.m_name  = in->read_vu30();
		info.m_flags = in->read_u8();

		if (info.m_flags & MethodInfo::HAS_OPTIONAL)
		{
			int option_count = in->read_vu30();
			info.m_options.resize(option_count);

			for (int o = 0; o < option_count; ++o)
			{
				info.m_options[o].m_index = in->read_vu30();
				info.m_options[o].m_kind  = static_cast<ConstantKind>( in->read_u8() );
			}
		}

		if (info.m_flags & MethodInfo::HAS_PARAM_NAMES)
		{
            for( int i = 0; i < info.m_param_type.size(); i++ ) {
                info.m_param_name.push_back( in->read_vu30() );
            }
		}

	//	IF_VERBOSE_PARSE(log_msg("method_info: name='%s', type='%s', params=%d\n",
    //                             get_string(info.m_name), get_multiname(info.m_return_type), info.m_param_type.size()));
    }

    // ** AbcInfo::readBody
    void AbcInfo::readBody(stream *in, BodyInfo& info)
    {
	//	IF_VERBOSE_PARSE(log_msg("body_info[%d]\n", info.m_method));

		info.m_max_stack        = in->read_vu30();
		info.m_local_count      = in->read_vu30();
		info.m_init_scope_depth = in->read_vu30();
		info.m_max_scope_depth  = in->read_vu30();

		int i, n;
		n = in->read_vu30();	// code_length
		info.m_code.resize(n);
		for (i = 0; i < n; i++)
		{
			info.m_code[i] = in->read_u8();
		}

		n = in->read_vu30();	// exception_count
		info.m_exception.resize(n);
		for (i = 0; i < n; i++)
		{
			ExceptInfo* e = new ExceptInfo();
            readExcept(in, *e);
			info.m_exception[i] = e;
		}

		n = in->read_vu30();	// trait_count
		info.m_trait.resize(n);
		for (int i = 0; i < n; i++)
		{
			TraitsInfo* trait = new TraitsInfo();
            readTraits(in, *trait);
			info.m_trait[i] = trait;
		}

	//	IF_VERBOSE_PARSE(log_msg("method	%i\n", info.m_method));
	//	IF_VERBOSE_PARSE(log_disasm_avm2(info.m_code, this));
    }

	//	cpool_info
	//	{
	//		u30 int_count
	//		s32 integer[int_count]
	//		u30 uint_count
	//		u32 uinteger[uint_count]
	//		u30 double_count
	//		d64 double[double_count]
	//		u30 string_count
	//		string_info string[string_count]
	//		u30 namespace_count
	//		namespace_info namespace[namespace_count]
	//		u30 ns_set_count
	//		ns_set_info ns_set[ns_set_count]
	//		u30 multiname_count
	//		multiname_info multiname[multiname_count]
	//	}
	void AbcInfo::readCPool(stream* in)
	{
		int n;

		// integer pool
		n = in->read_vu30();
		if (n > 0)
		{
			m_integer.resize(n);
			m_integer[0] = 0;	// default value
			for (int i = 1; i < n; i++)
			{
				m_integer[i] = in->read_vs32();
				IF_VERBOSE_PARSE(logger::msg("cpool_info: integer[%d]=%d\n", i, m_integer[i]));
			}
		}
		else
		{
			IF_VERBOSE_PARSE(logger::msg("cpool_info: no integer pool\n"));
		}

		// uinteger pool
		n = in->read_vu30();
		if (n > 0)
		{
			m_uinteger.resize(n);
			m_uinteger[0] = 0;	// default value
			for (int i = 1; i < n; i++)
			{
				m_uinteger[i] = in->read_vu32();
				IF_VERBOSE_PARSE(logger::msg("cpool_info: uinteger[%d]=%d\n", i, m_uinteger[i]));
			}
		}
		else
		{
			IF_VERBOSE_PARSE(logger::msg("cpool_info: no uinteger pool\n"));
		}

		// double pool
		n = in->read_vu30();
		if (n > 0)
		{
			m_double.resize(n);
			m_double[0] = 0;	// default value
			for (int i = 1; i < n; i++)
			{
				m_double[i] = in->read_double();
				IF_VERBOSE_PARSE(logger::msg("cpool_info: double[%d]=%f\n", i, m_double[i]));
			}
		}
		else
		{
			IF_VERBOSE_PARSE(logger::msg("cpool_info: no double pool\n"));
		}

		// string pool
		n = in->read_vu30();
		if (n > 0)
		{
			m_string.resize(n);
			m_string[0] = "";	// default value
			for (int i = 1; i < n; i++)
			{
				int len = in->read_vs32();
				in->read_string_with_length(len, &m_string[i]);
				IF_VERBOSE_PARSE(logger::msg("cpool_info: string[%d]='%s'\n", i, m_string[i].c_str()));
			}
		}
		else
		{
			IF_VERBOSE_PARSE(logger::msg("cpool_info: no string pool\n"));
		}
		
		// namespace pool
		n = in->read_vu30();
		if (n > 0)
		{
			m_namespace.resize(n);
			NamespaceInfo ns;
			m_namespace[0] = ns;	// default value

			for (int i = 1; i < n; i++)
			{
				ns.m_kind = static_cast<NamespaceInfo::Kind>(in->read_u8());
				ns.m_name = in->read_vu30();
				m_namespace[i] = ns;

				// User-defined namespaces have kind CONSTANT_Namespace or
				// CONSTANT_ExplicitNamespace and a non-empty name. 
				// System namespaces have empty names and one of the other kinds
				switch (ns.m_kind)
				{
					case NamespaceInfo::Namespace:
					case NamespaceInfo::ExplicitNamespace:
						//assert(*get_string(ns.m_name) != 0);
						break;
					case NamespaceInfo::PackageNamespace:
					case NamespaceInfo::PackageInternalNs:
					case NamespaceInfo::ProtectedNamespace:
					case NamespaceInfo::StaticProtectedNs:
					case NamespaceInfo::PrivateNs:
						//assert(*get_string(ns.m_name) == 0);
						break;
					default:
						assert(0);
				}
			//	IF_VERBOSE_PARSE(log_msg("cpool_info: namespace[%d]='%s', kind=0x%02X\n",
			//		i, get_string(ns.m_name), ns.m_kind));
			}
		}
		else
		{
			IF_VERBOSE_PARSE(logger::msg("cpool_info: no namespace pool\n"));
		}

		// namespace sets pool
		n = in->read_vu30();
		if (n > 0)
		{
			m_ns_set.resize(n);
			array<int> ns;
			m_ns_set[0] = ns;	// default value
			for (int i = 1; i < n; i++)
			{
				int count = in->read_vu30();
				ns.resize(count);
				for (int j = 0; j < count; j++)
				{
					ns[j] = in->read_vu30();
				}
				m_ns_set[i] = ns;
			}
		}
		else
		{
			IF_VERBOSE_PARSE(logger::msg("cpool_info: no namespace sets\n"));
		}

		// multiname pool
		n = in->read_vu30();
		if (n > 0)
		{
			m_multiname.resize(n);
			MultinameInfo mn( 0 );
			m_multiname[0] = mn;	// default value
			for (int i = 1; i < n; i++)
			{
                mn.m_index = i;
				mn.m_kind  = static_cast<MultinameInfo::Kind>(in->read_u8());
				switch (mn.m_kind)
				{
					case MultinameInfo::QName:
					case MultinameInfo::QNameA:
					{
						mn.m_ns = in->read_vu30();
						mn.m_name = in->read_vu30();
					//	IF_VERBOSE_PARSE(log_msg("cpool_info: multiname[%d]='%s', ns='%s'\n",
					//		i, get_string(mn.m_name), get_namespace(mn.m_ns)));
						break;
					}

					case MultinameInfo::RTQName:
						assert(0&&"todo");
						break;

					case MultinameInfo::RTQNameA:
						assert(0&&"todo");
						break;

					case MultinameInfo::RTQNameL:
						assert(0&&"todo");
						break;

					case MultinameInfo::RTQNameLA:
						assert(0&&"todo");
						break;

					case MultinameInfo::Multiname:
					case MultinameInfo::MultinameA:
						mn.m_name = in->read_vu30();
						mn.m_ns_set = in->read_vu30();
					//	IF_VERBOSE_PARSE(log_msg("cpool_info: multiname[%d]='%s', ns_set='%s'\n",
					//		i, get_string(mn.m_name), "todo"));
						break;

					case MultinameInfo::MultinameL:
					case MultinameInfo::MultinameLA:
						mn.m_ns_set = in->read_vu30();
						mn.m_name = -1;
						IF_VERBOSE_PARSE(logger::msg("cpool_info: multiname[%d]=MultinameL, ns_set='%s'\n",
							i, "todo"));
						break;

                    case MultinameInfo::Typename:
                        mn.m_type_name      = in->read_vu30();
                        mn.m_type_arg_count = in->read_vu30();
                        mn.m_type_arg       = in->read_vu30();
                        break;

					default:
						assert(0);

				}
				m_multiname[i] = mn;
			}
		}
		else
		{
			IF_VERBOSE_PARSE(logger::msg("cpool_info: no multiname pool\n"));
		}


	}
/*
	const char * AbcInfo::get_class_from_constructor( int method )
	{
		for( int instance_index = 0; instance_index < m_instance.size(); ++instance_index )
		{
			if( m_instance[ instance_index ]->m_iinit == method )
			{
				return get_multiname( m_instance[ instance_index ]->m_name );
			}
		}

		return NULL;
	}


	// get class constructor
	// 'name' is the fully-qualified name of the ActionScript 3.0 class 
	// with which to associate this symbol.
	// The class must have already been declared by a DoABC tag.
	as_function* AbcInfo::get_class_constructor(const tu_string& name) const
	{
    / *
		// find instance_info by name
		InstanceInfo* ii = get_instance_info(name);
		if (ii != NULL)
		{
			// 'ii->m_iinit' is an index into the method array of the abcFile; 
			// it references the method that is invoked whenever 
			// an object of this class is constructed.
			return m_method[ii->m_iinit].get();
		}
		return NULL;
    * /
        assert(false);
	}

	InstanceInfo* AbcInfo::get_instance_info(const tu_string& full_class_name) const
	{
		//TODO: implement namespace

		// find name
		tu_string class_name = full_class_name;
		const char* dot = strrchr(full_class_name.c_str(), '.');
		if (dot)
		{
			class_name = dot + 1;
		}

		// maybe use hash instead of array for m_instance ?
		for (int i = 0; i < m_instance.size(); i++)
		{
			const tu_string& name = get_multiname(m_instance[i]->m_name);
			if (class_name == name)
			{
				return m_instance[i].get();
			}
		}
		return NULL;
	}

	ClassInfo* AbcInfo::get_class_info(const tu_string& full_class_name) const
	{
		//TODO: implement namespace

		// find name
		tu_string class_name = full_class_name;
		const char* dot = strrchr(full_class_name.c_str(), '.');
		if (dot)
		{
			class_name = dot + 1;
		}

		// maybe use hash instead of array for m_instance ?
		for (int i = 0; i < m_instance.size(); i++)
		{
			const tu_string& name = get_multiname(m_instance[i]->m_name);
			if (class_name == name)
			{
				return m_class[i].get();
			}
		}
		return NULL;
	}

	ClassInfo* AbcInfo::get_class_info(int class_index) const
	{
		return m_class[class_index].get();
	}

	as_function* AbcInfo::get_script_function( const tu_string & name ) const
	{
		if( name == "" )
			return m_method[ m_script.back()->m_init ].get();
		else
		{
			for( int script_index = 0; script_index < m_script.size(); ++script_index )
			{
				const ScriptInfo & info = *m_script[ script_index ].get();
				for( int trait_index = 0; trait_index < info.m_trait.size(); ++trait_index )
				{
					if( m_string[ m_multiname[info.m_trait[ trait_index ]->m_name].m_name ] == name && info.m_trait[ trait_index ]->m_kind == TraitsInfo::Trait_Class )
					{
						return m_method[ info.m_init ].get();
					}
				}
			}
		}

		return NULL;
	}
 */
};	// end namespace avm2

// Local Variables:
// mode: C++
// c-basic-offset: 8 
// tab-width: 8
// indent-tabs-mode: t
// End:
