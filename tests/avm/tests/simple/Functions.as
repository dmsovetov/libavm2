include 'classes/ArrayMultiplier.as'

import classes.ArrayMultiplier

////////////////////////////////////////////////////////////////////////////////

function test() {
	var result = add( 1, 2 )
	trace( 'Test (1 + 2) = ' + result )
}

function add( a, b ) : Number {
	return a + b
}

test()

////////////////////////////////////////////////////////////////////////////////

var func = function( a, b ) {
	var xyz = 123
	trace( a, b, xyz )
}

func( 1, 'hello' )

////////////////////////////////////////////////////////////////////////////////

function allDefaults( a : Number = 1.5, b : int = 2, c : String = 'default arg', d : Boolean = false )
{
	trace( a, b, c, d )
}

allDefaults()
allDefaults( 44.4 )
allDefaults( 44.4, -22 )
allDefaults( 44.4, -22, 'hello world' )
allDefaults( 44.4, -22, 'hello world', true )

////////////////////////////////////////////////////////////////////////////////

function withDefaults( a : Number, b : int, c : String = 'default arg', d : Boolean = false, e : Number = 5.75, f : int = 444 )
{
	trace( a, b, c, d, e, f )
}

withDefaults( 1.1, 2 )
withDefaults( 1.1, 2, 'hello' )
withDefaults( 1.1, 2, 'hello', true )
withDefaults( 1.1, 2, 'hello', true, 6.99 )
withDefaults( 1.1, 2, 'hello', true, 6.99, -123 )

////////////////////////////////////////////////////////////////////////////////

var outer1  = 'toplevel'
var global = 'global'

var func1 = function() {
	var outer2 = 'func1'
	
	return function() {
		return outer1 + ' ' + outer2 + ' ' + global
	}
}

trace( func1()() )

////////////////////////////////////////////////////////////////////////////////

function f(x) {
	return function () { return x }
}

trace( f(123)() )

////////////////////////////////////////////////////////////////////////////////

var list : Array = [ 1, 2, 3, 4, 5, 6, 7, 8, 9 ]
trace( list )

list = list.map( multiplyBy( 2 ) )
// list = [ 2,4,6,8,10,12,14,16,18 ]
trace( list )

list = list.map( multiplyBy( 3 ) )
// list = [ 3,6,9,12,15,18,21,24,27 ]
trace( list )

function multiplyBy( value : Number ) : Function
{
    return function( x : int, index : int, array : Array ) : Number
    {
         return x * value
    }
}

////////////////////////////////////////////////////////////////////////////////

list = [1, 2, 3, 4, 5]
trace( list )

list = ArrayMultiplier.multiplyBy( list, 10 )
trace( list )

var multiply = new ArrayMultiplier( 2.5 )
list = list.map( multiply.thunk )
trace( list )

multiply.amount = 0.01
list = list.map( multiply.thunk )
trace( list )