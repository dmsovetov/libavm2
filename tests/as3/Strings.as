////////////////////////////////////////////////////////////////////////

var str:String = "Adobe"; 
trace(str.length); // output: 5

var str1:String = new String(); 
trace(str1.length);           // output: 0 
 
var str2:String = ''; 
trace(str2.length);           // output: 0

////////////////////////////////////////////////////////////////////////

str = "hello world!"; 
for (var i:int = 0; i < str.length; i++) 
{ 
    trace(str.charAt(i), "-", str.charCodeAt(i)); 
}

////////////////////////////////////////////////////////////////////////


var myStr:String = String.fromCharCode(104,101,108,108,111,32,119,111,114,108,100,33); 
trace(myStr)

////////////////////////////////////////////////////////////////////////
str1 = "green"; 
str2 = "ish"; 
var str3:String = str1 + str2; // str3 == "greenish"
trace( str3 )

str = "green"; 
str += "ish"; // str == "greenish"
trace(str)

str1 = "Bonjour"; 
str2 = "from"; 
str3 = "Paris"; 
var str4 = str1.concat(" ", str2, " ", str3); 
trace(str4)

str = "Area = "; 
var area:Number = Math.PI * Math.pow(3, 2); 
str = str + area; // str == "Area = 28.274333882308138"
//trace( str )

trace("Total: $" + 4.55 + 1.45); // output: Total: $4.551.45 
trace("Total: $" + (4.55 + 1.45)); // output: Total: $6

////////////////////////////////////////////////////////////////////////

str = "Hello from Paris, Texas!!!"; 
trace(str.substr(11,15)); // output: Paris, Texas!!! 
trace(str.substring(11,15)); // output: Pari

str = "Hello from Paris, Texas!!!"; 
trace(str.slice(11,15)); // output: Pari 
trace(str.slice(-3,-1)); // output: !! 
trace(str.slice(-3,26)); // output: !!! 
trace(str.slice(-3,str.length)); // output: !!! 
trace(str.slice(-8,-3)); // output: Texas

////////////////////////////////////////////////////////////////////////

str = "The moon, the stars, the sea, the land"; 
trace(str.indexOf("the")); // output: 10

str = "The moon, the stars, the sea, the land" 
trace(str.indexOf("the", 11)); // output: 21

str = "The moon, the stars, the sea, the land" 
trace(str.lastIndexOf("the")); // output: 30

str = "The moon, the stars, the sea, the land" 
trace(str.lastIndexOf("the", 29)); // output: 21

////////////////////////////////////////////////////////////////////////

var queryStr:String = "first=joe&last=cheng&title=manager&StartDate=3/6/65"; 
var params:Array = queryStr.split("&", 2); // params == ["first=joe","last=cheng"]
trace(params)