import classes.CustomError

////////////////////////////////////////////////////////////////////////////////////////////////////

try {
	throw new Error
}
catch( e : Error ) {
	trace( e )
}

////////////////////////////////////////////////////////////////////////////////////////////////////

try {
	throw new CustomError( 'Error with finally' )
}
catch( e : Error ) {
	trace( e )
}
finally {
	trace( 'Finally' )
}

////////////////////////////////////////////////////////////////////////////////////////////////////

try {
	throw new CustomError( "Something went wrong" )
}
catch( e : CustomError ) {
	trace( '[Custom] ' + e )
}
catch( e : Error ) {
	trace( e )
}
finally {
	trace( 'Done!' )
}

////////////////////////////////////////////////////////////////////////////////////////////////////


try {
	throw new Error( "Something went wrong" )
}
catch( e : CustomError ) {
	trace( '[Custom] ' + e )
}
catch( e : Error ) {
	trace( e )
}
finally {
	trace( 'Done!' )
}

////////////////////////////////////////////////////////////////////////////////////////////////////

function error( err ) {
	try {
		throw err
	}
	catch( e : String ) {
		trace( 'String: ' + e )
	}
	catch( e : Number ) {
		trace( 'Number:' + e )
	}
	catch( e ) {
		trace( 'Unknown: ' + e )
	}
	finally {
		trace( 'Error ' + err + ' thrown!' )
	}
}

error( 1 )
error( 'hello' )
error( false )

////////////////////////////////////////////////////////////////////////////////////////////////////

function errorStrict( err ) {
	try {
		throw err
	}
	catch( e : String ) {
		trace( 'String: ' + e )
	}
	catch( e : Number ) {
		trace( 'Number:' + e )
	}
	finally {
		trace( 'Error ' + err + ' thrown!' )
	}
}

try {
	errorStrict( 1 )
	errorStrict( 'hello' )
	errorStrict( false )
} catch( e ) {
	trace( 'Unhandled error caught: ' + e )
}