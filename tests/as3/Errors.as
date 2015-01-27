import flash.display.MovieClip
import classes.Test

////////////////////////////////////////////////////////////////////////////////////////////////////////

try {
	var np
	np()
}
catch( e ) {
	trace( 'Caught: value is not a function.' )
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

try {
	var big:* = new Test('')
	var error:int = big.length()
}
catch( e ) {
	trace( 'Caught: Property length not found on Test and there is no default value.' )
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

try {
	var sprite1:* = undefined
	var q:String = sprite1.stage.quality
}
catch( e ) {
	trace( 'Caught: A term is undefined and has no properties.' )
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

try {
	var sprite2:* = null
	var q2:String = sprite2.stage.quality
}
catch( e ) {
	trace( 'Caught: Cannot access a property or method of a null object reference.' )
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

function xyz( a : MovieClip ) {
	trace( a )
}

try {
	var t = new Object
	xyz( t )
} catch( e ) {
	trace( 'Caught: Type Coercion failed' )
}

try {
	var t1 = xyz
	t1( new MovieClip, new MovieClip )
} catch( e ) {
	trace( 'Caught: Argument count mismatch' )
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
function one() {
	two()
}

function two() {
	three()
}

function three() {
	var e = new Error( 'Stack trace:' )
	trace( e.getStackTrace() )
	
	throw e
}

try {
	one()
} catch( e ) {
	trace( '\nException caught:' )
	trace( e.getStackTrace() )
}
*/