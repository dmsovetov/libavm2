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

#ifndef avm2_OBJECT_H
#define avm2_OBJECT_H

#include "Value.h"

#define AvmDeclareObjectType( id )  enum { m_class_id = id };   \
                                    virtual bool is( int classId ) const { return m_class_id == classId; }

#define AvmDeclareType( id, super ) enum { m_class_id = id };   \
                                    virtual bool is( int classId ) const { return m_class_id == classId ? true : super::is( classId ); }

namespace avm2
{
    typedef string_hash<Value> Members;

    // ** class Iterator
    class Iterator : public ref_counted {
    public:

        virtual             ~Iterator( void ) {}

        virtual bool        hasNext( void ) const   = 0;
        virtual void        next( void )            = 0;
        virtual Value       key( void ) const       = 0;
    };

    // ** class ObjectIterator
    class ObjectIterator : public Iterator {
    public:

                            ObjectIterator( Members* members, const Value& key );

        // ** Iterator
        virtual bool        hasNext( void ) const;
        virtual void        next( void );
        virtual Value       key( void ) const;

    private:

        Members*            m_members;
        Members::iterator   m_iterator;
    };

    // ** class Object
	class Object : public ObjectInterface {
    friend class Avm;
    friend class Value;
    public:

                                    AvmDeclareObjectType( AS_OBJECT );

                                    Object( Domain* domain );
		virtual                     ~Object( void );
		
		virtual const char*         to_string( void );
		virtual double              to_number( void );
		virtual bool                to_bool( void ) { return true; }

        //! Returns a value of this object.
        virtual Value               valueOf( void ) const;
        //! Returns a parent domain.
        Domain*                     domain( void ) const;
		//! Returns a closure of a given Function that is associated with this Object.
        FunctionClosure*            enclose( Function* function ) const;
		//! Returns a Value of a slot at a given index.
        const Value&                slot( int index ) const;
		//! Sets a Value of a slot at a given index.
        void                        setSlot( int index, const Value& value );
		//! Returns Traits object associated with this Object.
        const Traits*               traits( void ) const;
		//! Sets a Traits of this object.
        void                        setTraits( const Traits* value );
		//! Returns a Class of this object.
        Class*                      type( void ) const;
        //! Returns a prototype object.
        Object*                     prototype( void ) const;
		//! Sets an Object Class.
        void                        setType( Class* value );
		//! Returns true if this Object has a public property with a given name.
        bool                        hasOwnProperty( const Str& name ) const;
		//! Resolves a property inside this object by a given name and access scope.
        bool                        resolveProperty( const Str& name, Value* value ) const;
		//! Resolves a property inside this object by a given multiname and access scope.
        bool                        resolveProperty( const Name* name, Value* value ) const;
		//! Sets a property inside this object with a given name and access scope.
        bool                        setProperty( const Str& name, const Value& value );
        //! Sets a property inside this object with a given name and access scope.
        bool                        setProperty( const Name* name, const Value& value );
        //! Creates a new iterator instance by a given property key.
        virtual IteratorPtr         createIterator( const Value& value );
        //! Returns a next key for iterator.
        virtual Value               nextKey( const Value& key ) const;
        //! Returns a value of dynamic object property by a given key.
        virtual Value               getPropertyByKey( const Value& key ) const;
        //! Deletes the dynamic object property by a given key.
        bool                        deletePropertyByName( const Str& name );

		void                        builtin_member( const Str& name, const Value& value );
		virtual bool                set_member( const Str& name, const Value& value );
		virtual bool                get_member( const Str& name, Value* val );
		virtual void                clear_refs( hash<Object*, bool>* visited_objects, Object* this_ptr );
		virtual void                this_alive( void );
		virtual void                alive( void ) {}
		virtual void                copy_to( Object* target );
		Object*                     find_target( const Value& target );

    protected:

        //! Parent domain.
        Domain*                     m_domain;
		//! Dynamic object properties.
        Members                     m_members;
		//! Associated object traits.
        TraitsWeak                  m_traits;
		//! Object class.
        ClassWeak                   m_class;
		//! Function closure cache.
        mutable FunctionClosures    m_closures;
		//! The boolean flag that indicates that we are running the toString method.
		// !!!: Is this ok?
        mutable bool                m_isInsideToString;
		//! An array of a fixed object slots.
        ValueArray                  m_slots;
        //! Prototype object.
        mutable ObjectPtr           m_prototype;
	};

    class String : public Object {
    public:

                            AvmDeclareType( AS_STRING, Object )

                            String( Domain* domain, const Str& string = "" );

        // ** Object
        virtual const char* to_string( void ) { return m_string.c_str(); }

        // ** String
        const Str&          toString( void ) const { return m_string; }
        const char*         toCString( void ) const { return m_string.c_str(); }

        void                setString( const Str& value ) { m_string = value; }
        int                 length( void ) const { return m_string.size(); }
        Value               split( const Str& delimiter, int limit = -1 );
        Value               concat( const ValueArray& strings ) const;
        Value               slice( int startIndex = 0, int endIndex = -1 ) const;
        Value               substr( int startIndex = 0, int length = -1 ) const;
        Value               substring( int startIndex = 0, int endIndex = -1 ) const;
        Value               toUpperCase( void ) const;
        Value               toLowerCase( void ) const;
        Str                 charAt( int index = 0 ) const;
        int                 charCodeAt( int index = 0 ) const;
        int                 indexOf( const Str& value, int startIndex = 0 ) const;
        int                 lastIndexOf( const Str& value, int startIndex = -1 ) const;

        static Value        fromCharCode( Domain* domain, const ValueArray& codes );

    private:

        Str                 m_string;
    };

    // ** class XML
    class XML : public Object {
    public:

                            AvmDeclareType( AS_XML, Object )

                            XML( Domain* domain );

        const Str&          text( void ) const;
        void                setText( const Str& value );

    private:

        Str                 m_text;
    };

    // ** class Int
    class Int : public Object {
    public:

                            AvmDeclareType( AS_INT, Object );

                            Int( Domain* domain );

        // ** Object

        //! Returns a value of this int.
        virtual Value       valueOf( void ) const;
        virtual const char* to_string( void );

        // ** Int
        int                 value( void ) const;
        void                setValue( int value );

    private:

        int                 m_value;
    };

    // ** class UInt
    class UInt : public Object {
    public:

                            AvmDeclareType( AS_UINT, Object );

                            UInt( Domain* domain );

        // ** Object

        //! Returns a value of this int.
        virtual Value       valueOf( void ) const;
        virtual const char* to_string( void );

        // ** UInt
        unsigned int        value( void ) const;
        void                setValue( unsigned int value );

    private:

        unsigned int        m_value;
    };

    // ** class Boolean
    class Boolean : public Object {
    public:

                            AvmDeclareType( AS_BOOLEAN, Object )

                            Boolean( Domain* domain );

        // ** Object

        //! Returns a value of this Boolean.
        virtual Value       valueOf( void ) const;
        virtual const char* to_string( void );

        // ** Boolean
        bool                value( void ) const;
        void                setValue( bool value );

    private:

        bool                m_value;
    };

    // ** class Number
    class Number : public Object {
    public:

                            AvmDeclareType( AS_NUMBER, Object );

                            Number( Domain* domain );

        // ** Object

        //! Returns a value of this Number.
        virtual Value       valueOf( void ) const;
        virtual const char* to_string( void );

        // ** Number
        double              value( void ) const;
        void                setValue( double value );

    private:

        double              m_value;
    };

    AvmBeginClass( Object )
        AvmDeclareMethod( hasOwnProperty )
        AvmDeclareReadonly( prototype )
        AvmDeclareMethod( toString )
    AvmEndClass

    AvmBeginClass( Int )
        AvmDeclareMethod( init )
    AvmEndClass

    AvmBeginClass( UInt )
        AvmDeclareMethod( init )
    AvmEndClass

    AvmBeginClass( Boolean )
        AvmDeclareMethod( init )
    AvmEndClass

    AvmBeginClass( Number )
        AvmDeclareMethod( init )
    AvmEndClass

    AvmBeginClass( String )
        AvmDeclareMethod( init )
        AvmDeclareMethod( length )
        AvmDeclareMethod( split )
        AvmDeclareMethod( concat )
        AvmDeclareMethod( charAt )
        AvmDeclareMethod( slice )
        AvmDeclareMethod( substr )
        AvmDeclareMethod( substring )
        AvmDeclareMethod( indexOf )
        AvmDeclareMethod( lastIndexOf )
        AvmDeclareMethod( charCodeAt )
        AvmDeclareMethod( fromCharCode )
        AvmDeclareMethod( toUpperCase )
        AvmDeclareMethod( toLowerCase )
    AvmEndClass

    AvmBeginStaticClass( Math )
        AvmDeclareMethod( pow )
        AvmDeclareMethod( cos )
        AvmDeclareMethod( sin )
        AvmDeclareMethod( sqrt )
        AvmDeclareMethod( round )
    AvmEndClass

    AvmBeginClass( XML )
        AvmDeclareMethod( init )
        AvmDeclareMethod( text )
    AvmEndClass

    typedef gc_ptr<String>      StringPtr;
    typedef array<StringPtr>    Strings;

    typedef gc_ptr<Object>      ObjectPtr;

}

#endif
