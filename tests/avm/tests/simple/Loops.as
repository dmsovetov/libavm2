trace( 'Basic for loop:' )
for( var i = 0; i < 10; i++ ) {
	trace( i )
}

trace( 'For loop with a local var:' )
var j : Number;
for( j = 0; j < 10; j++ ){
	trace( j )
}

trace( 'While loop:' )

j = 0
while ( j < 10 ) {
	trace( j )
	j++
}

trace( 'Do while loop:' )
j = 0
do { 
	trace( j )
	j++
} while ( j < 10);