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


#ifndef avm2_VALUE_H
#define avm2_VALUE_H

//#include "base/container.h"
#include "Common.h"	// for ref_counted
//#include <wchar.h>

namespace avm2
{
	struct fn_call;
	struct as_environment;

	bool string_to_number(int* result, const char* str, int base = 10);
	bool string_to_number(double* result, const char* str);

	// helper, used in Value
	struct as_property : public ref_counted
	{
		gc_ptr<Function>	m_getter;
		gc_ptr<Function>	m_setter;

		as_property(const Value& getter,	const Value& setter);
		~as_property();
	
		void	set(Object* target, const Value& val);
		void	get(Object* target, Value* val) const;
		void	get(const Value& primitive, Value* val) const;
        void    setSetter( Function* value );
        void    setGetter( Function* value );
	};

    // ** class Value
	class Value {
    public:

        // ** enum eType
        enum eType {
			Undefined,
			Boolean,
			Number,
			String,
			Object,
			Property
		};

        static Value                null;
        static Value                undefined;

	public:

                                    Value( void );
                                    Value( const Value& value );
                                    Value( const char* value );
                                    Value( const wchar_t* value );
                                    Value( bool value );
                                    Value( int value );
                                    Value( float value );
                                    Value( double value );
                                    Value( unsigned int value );
                                    Value( ::avm2::Object* obj );
                                    Value( FunctionNative* func );
                                    Value( Function* func );
                                    Value( const Value& getter, const Value& setter );
                                    ~Value( void ) { dispose(); }

		//! Copies the Value.
        void                        operator =  ( const Value& v );
		//! Compares to Values and returns true if they are equal.
		bool                        operator == ( const Value& v ) const;
		//! Compares to Values and returns true if they are NOT equal.
		bool                        operator != ( const Value& v ) const;

        //! Returns a value of this Value (used for Objects).
        Value                       valueOf( void ) const;
		//! Returns a string representation of this Value type.
        const char*                 type( void ) const;
        //! Returns the Value type id.
        eType                       typeId( void ) const;
		//! Returns true if this Value is of a given type.
        bool                        is( const Class* type ) const;
		//! Returns true if this Value is a Function.
        bool                        isFunction( void ) const;
		//! Returns true if this Value is an Array.
        bool                        isArray( void ) const;
		//! Returns true if this Value is a String object.
		// !!!: Think of intrinsic object types
        bool                        isStringObject( void ) const;
		//! Returns true if this value is a Boolean.
		inline bool                 isBool( void ) const;
		//! Returns true if this Value is a String.
		inline bool                 isString( void ) const;
		//! Returns true if this Value is a Number.
		inline bool                 isNumber( void ) const;
		//! Returns true if this Value is an Object.
		inline bool                 isObject( void ) const;
		//! Returns true if this Value is a Property.
		// ???: Do we actually need this?
		inline bool                 isProperty( void ) const;
		//! Returns true if this Value is null.
		inline bool                 isNull( void ) const;
		//! Returns true if this Value is null or undefined.
        inline bool                 isNullOrUndefined( void ) const;
		//! Returns true if this Value is undefined.
		inline bool                 isUndefined( void ) const;

		//! Returns the string representation of a Value as a C string.
        const char*                 asCString( void ) const;
		//! Returns the string representation of a Value.
        const Str&                  asString( void ) const;
		//! Returns the double representation of a Value.
        double                      asNumber( void ) const;
		//! Returns the integer representation of a Value.
        int                         asInt( void ) const;
		//! Returns the unsigned integer representation of a Value.
        Uint32                      asUInt( void ) const { return ( Uint32 )asNumber(); }
		//! Returns the float representation of a Value.
		float                       asFloat( void ) const { return ( float )asNumber(); };
		//! Returns the boolean representation of a Value.
		bool                        asBool( void ) const;
		//! Returns the pointer to Function if this Value stores a Function object, otherwise NULL.
		Function*                   asFunction( void ) const;
		//! Returns the pointer to Array if this Value stores a Array object, otherwise NULL.
        Array*                      asArray( void ) const;
		//! Returns the pointer to object if this Value stores an object, otherwise NULL.
		::avm2::Object*          asObject( void ) const;
        //! Returns the pointer to Class if this Value stores a Class, otherwise NULL.
        ::avm2::Class*           asClass( void ) const;
		//! Returns the pointer to property if this Value stores a Property object, otherwise NULL.
		// ???: Do we actually need this?
		::avm2::Property*        asProperty( void ) const;
		//! Returns an int vector representation of this Value.
		/*! This function will convert an Array value to an vector of int,
		 *	or just return an array with a single int that is converted
		 *	from this Value.
		*/
        IntegerArray                asIntegerArray( void ) const;
		//! Returns a Str vector representation of this Value.
		/*! This function will convert an Array value to a vector of Str,
		 *	or just return an array with a single Str that is converted
		 *	from this Value.
		*/
        StrArray                    asStrArray( void ) const;

		//! Sets this Value to a String.
        void                        setString( const Str& value );
		//! Sets this Value to a String.
        void                        setString( const char* value );
		//! Sets this Value to a Number.
        void                        setNumber( double value );
		//! Sets this Value to a Boolean.
        void                        setBool( bool value );
		//! Sets this Value to a Number from a given int value.
        void                        setInt( int value ) { setNumber( value ); }
		//! Sets this Value to NaN.
        void                        setNaN( void ) { setNumber( get_nan() ); }
		//! Sets this Value to an Object.
        void                        setObject( ::avm2::Object* value );
		//! Sets this Value to a Function.
        void                        setFunction( FunctionNative* func );
		//! Sets this Value to an undefined.
        void                        setUndefined( void ) { dispose(); m_type = Undefined; }
		//! Sets this Value to a null.
        void                        setNull( void ) { setObject( NULL ); }

		//! Disposes all stored objects.
        void                        dispose( void );

        //! Adds two given values.
        static Value                add( Domain* domain, const Value& a, const Value& b, bool useValueOf = true );
        //! Compares two given values.
        static bool                 compare( const Value& a, const Value& b );
        //! Does an inplace type coercion of a given value, returns true on success.
        static bool                 coerceInPlace( Value& value, const Class* type );
        //! Coerce the value to a fiven type.
        static Value                coerce( const Value& value, const Class* type );
        //! Returns a numeric NaN value.
        static Value                NaN( void );
        //! Returns a numeric Infinity value.
        static Value                Infinity( void );

    private:

		//! Value type.
		eType                       m_type;
		union {
			double  m_number;
			bool    m_bool;
		};

        mutable Str                 m_string;	//! String value.
		gc_ptr<class Object>        m_object;	//! Object value.
		gc_ptr<as_property>         m_property;	//! Property value.
	};

    // ** Value::isBool
    bool Value::isBool( void ) const {
        return m_type == Boolean;
    }

    // ** Value::isString
    bool Value::isString( void ) const {
        return m_type == String;
    }

    // ** Value::isNumber
    bool Value::isNumber( void ) const {
        return m_type == Number && isnan(m_number) == false;
    }

    // ** Value::isObject
    bool Value::isObject( void ) const {
        return m_type == Object;
    }

    // ** Value::isProperty
    bool Value::isProperty( void ) const {
        return m_type == Property;
    }

    // ** Value::isNull
    bool Value::isNull( void ) const {
        return m_type == Object && m_object == NULL;
    }

    // ** Value::isUndefined
    bool Value::isUndefined( void ) const {
        return m_type == Undefined;
    }

    // ** Value::isNullOrUndefined
    bool Value::isNullOrUndefined( void ) const {
        return m_type == Undefined || (m_type == Object && m_object == NULL);
    }

    typedef std::vector<Value> ValueArray;
}

#endif
