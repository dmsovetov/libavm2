var myObject:Object = new Object();
myObject.myName = "John Doe";
myObject.myAge = 30;

trace( 'For each loop:' )
for each (var prop in myObject){
	trace(prop);
}

trace( 'For in loop:' )
for (var propName in myObject){
	trace(propName);
}