include 'classes/Test.as'
include 'classes/ClassWithArgs.as'

import classes.Test
import classes.ClassWithArgs

var test = new Test( 'a' )
trace( test )
var cls = new ClassWithArgs( 123, 'hello world' )
trace( cls )