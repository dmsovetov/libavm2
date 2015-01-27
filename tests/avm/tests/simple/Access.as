import classes.Foo
import classes.Bar

var foo : * = new Foo
var bar : * = new Bar

function setProperty( object, name, value )
{
	try {
		object[name] = value
		trace( '\t\t[x] ' + name )
	} catch( e ) {
		trace( '\t\t[ ] ' + name )
	}
}

function getProperty( object, name )
{
	try {
		var value = object[name]
		trace( '\t\t[x] ' + name )
	} catch( e ) {
		trace( '\t\t[ ] ' + name )
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////

trace( 'Checking access for class Foo...' )

trace( '\tWrite from outside' )
setProperty( foo, '_public', 'set from root' )
setProperty( foo, '_protected', 'set from root' )
setProperty( foo, '_private', 'set from root' )
setProperty( foo, 'boolean', true )

trace( '\tRead from outside' )
getProperty( foo, '_public' )
getProperty( foo, '_protected' )
getProperty( foo, '_private' )
getProperty( foo, 'boolean' )

////////////////////////////////////////////////////////////////////////////////////////////////

foo.write()
foo.read()
//foo.writeFromAnonymous()
//foo.readFromAnonymous()

////////////////////////////////////////////////////////////////////////////////////////////////

trace( 'Checking access for class Bar...' )

trace( '\tWrite from outside' )
setProperty( bar, '_public', 'set from root' )
setProperty( bar, '_protected', 'set from root' )
setProperty( bar, '_private', 'set from root' )
setProperty( bar, 'boolean', true )
setProperty( bar, '_public_bar', 'set from root' )

trace( '\tRead from outside' )
getProperty( bar, '_public' )
getProperty( bar, '_protected' )
getProperty( bar, '_private' )
getProperty( bar, 'boolean' )
getProperty( bar, '_public_bar' )

////////////////////////////////////////////////////////////////////////////////////////////////

bar.write()
bar.read()
//foo.writeFromAnonymous()
//foo.readFromAnonymous()

////////////////////////////////////////////////////////////////////////////////////////////////
