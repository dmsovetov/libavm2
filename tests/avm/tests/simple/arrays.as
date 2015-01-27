include 'classes/Vegetable.as'
import classes.Vegetable

////////////////////////////////////////////////////////////////////////////////////////////////////

var oneArray:Array = new Array("a", "b", "c");
var twoArray:Array = oneArray; // Both array variables refer to the same array.
twoArray[0] = "z";             
trace(oneArray);               // Output: z,b,c.

////////////////////////////////////////////////////////////////////////////////////////////////////

var names:Array = new Array("Bill");
names.push("Kyle");
trace(names.length); // 2

names.push("Jeff");
trace(names.length); // 3

names.shift();
names.shift();
trace(names.length); // 1

////////////////////////////////////////////////////////////////////////////////////////////////////

var myArr:Array = new Array("one", "two", "three");
trace(myArr.length); // 3
trace(myArr);          // one,two,three

////////////////////////////////////////////////////////////////////////////////////////////////////

var myArr2:Array = new Array();
trace(myArr2.length); // 0

////////////////////////////////////////////////////////////////////////////////////////////////////

var myArr3:Array = new Array(5);
trace(myArr3.length); // 5
myArr3[0] = "one";
myArr3.push("six");
trace(myArr3);         // one,,,,,six
trace(myArr3.length); // 6

////////////////////////////////////////////////////////////////////////////////////////////////////

var numbers:Array = new Array(1, 2, 3);
var letters:Array = new Array("a", "b", "c");
var numbersAndLetters:Array = numbers.concat(letters);
var lettersAndNumbers:Array = letters.concat(numbers);

trace(numbers);       // 1,2,3
trace(letters);       // a,b,c
trace(numbersAndLetters); // 1,2,3,a,b,c
trace(lettersAndNumbers); // a,b,c,1,2,3

////////////////////////////////////////////////////////////////////////////////////////////////////

var arr1:Array = new Array(1, 2, 4);
var res1:Boolean = arr1.every(isNumeric);
trace("isNumeric:", res1); // true
 
var arr2:Array = new Array(1, 2, "ham");
var res2:Boolean = arr2.every(isNumeric);
trace("isNumeric:", res2); // false

function isNumeric(element:*, index:int, arr:Array):Boolean {
	return (element is Number);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

var employees:Array = new Array();
employees.push({name:"Employee 1", manager:false});
employees.push({name:"Employee 2", manager:true});
employees.push({name:"Employee 3", manager:false});
trace("Employees:");
employees.forEach(traceEmployee);
            
var managers:Array = employees.filter(isManager);
trace("Managers:");
managers.forEach(traceEmployee);

function isManager(element:*, index:int, arr:Array):Boolean {
	return (element.manager == true);
}

function traceEmployee(element:*, index:int, arr:Array):void {
	trace("\t" + element.name + ((element.manager) ? " (manager)" : ""));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

var arr:Array = new Array(123,45,6789);
arr.push("123-45-6789");
arr.push("987-65-4321");
            
var index:int = arr.indexOf("123");
trace(index); // -1
            
var index2:int = arr.indexOf(123);
trace(index2); // 0

////////////////////////////////////////////////////////////////////////////////////////////////////

var myArr4:Array = new Array("one", "two", "three");
var myStr:String = myArr4.join(" and ");
trace(myArr4); // one,two,three
trace(myStr); // one and two and three

var phoneString:String = "(888) 867-5309";

var specialChars:Array = new Array("(", ")", "-", " ");
var myStr1:String = phoneString;

var ln:uint = specialChars.length;
for(var i:uint = 0; i < ln; i++) {
    myStr1 = myStr1.split(specialChars[i]).join("");
}

var phoneNumber:Number = new Number(myStr1);

trace(phoneString); // (888) 867-5309
trace(phoneNumber); // 8888675309

////////////////////////////////////////////////////////////////////////////////////////////////////

var arr3:Array = new Array(123,45,6789,123,984,323,123,32);
            
var index1:int = arr3.indexOf(123);
trace(index1); // 0
            
var index3:int = arr3.lastIndexOf(123);
trace(index3); // 6

////////////////////////////////////////////////////////////////////////////////////////////////////

var arrMap:Array = new Array("one", "two", "Three");
trace(arrMap); // one,two,Three

var upperArr:Array = arrMap.map(toUpper);
trace(upperArr); // ONE,TWO,THREE

function toUpper(element:*, index:int, arr:Array):String {
	return String(element).toUpperCase();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

var letters1:Array = new Array("a", "b", "c");
trace(letters1); // a,b,c
var letter:String = letters1.pop();
trace(letters1); // a,b
trace(letter);     // c

////////////////////////////////////////////////////////////////////////////////////////////////////

var letters2:Array = new Array();

letters2.push("a");
letters2.push("b");
letters2.push("c");

trace(letters2.toString()); // a,b,c

var letters3:Array = new Array("a");
var count:uint = letters3.push("b", "c");

trace(letters3); // a,b,c
trace(count);   // 3

////////////////////////////////////////////////////////////////////////////////////////////////////

var letters4:Array = new Array("a", "b", "c");
trace(letters4); // a,b,c
letters4.reverse();
trace(letters4); // c,b,a

////////////////////////////////////////////////////////////////////////////////////////////////////

var letters5:Array = new Array("a", "b", "c");
var firstLetter:String = letters5.shift();
trace(letters5);     // b,c
trace(firstLetter); // a

////////////////////////////////////////////////////////////////////////////////////////////////////

var letters6:Array = new Array("a", "b", "c", "d", "e", "f");
var someLetters:Array = letters6.slice(1,3);

trace(letters6);     // a,b,c,d,e,f
trace(someLetters); // b,c

var letters7:Array = new Array("a", "b", "c", "d", "e", "f");
var someLetters7:Array = letters7.slice(2);

trace(letters7);     // a,b,c,d,e,f
trace(someLetters7); // c,d,e,f

var letters8:Array = new Array("a", "b", "c", "d", "e", "f");
var someLetters8:Array = letters8.slice(-2);

trace(letters8);     // a,b,c,d,e,f
trace(someLetters8); // e,f

////////////////////////////////////////////////////////////////////////////////////////////////////

var arr4:Array = new Array();
arr4[0] = "one";
arr4[1] = "two";
arr4[3] = "four";
var isUndef:Boolean = arr.some(isUndefined);
if (isUndef) {
	trace("array contains undefined values: " + arr4);
} else {
	trace("array contains no undefined values.");
}

function isUndefined(element:*, index:int, arr:Array):Boolean {
	return (element == undefined);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

var vegetables:Array = new Array("spinach",
                 "green pepper",
                 "cilantro",
                 "onion",
                 "avocado");

trace(vegetables); // spinach,green pepper,cilantro,onion,avocado
vegetables.sort();
trace(vegetables); // avocado,cilantro,green pepper,onion,spinach

////////////////////////////////////////////////////////////////////////////////////////////////////

vegetables = new Array("spinach",
                 "green pepper",
                 "Cilantro",
                 "Onion",
                 "Avocado");

vegetables.sort();
trace(vegetables); // Avocado,Cilantro,Onion,green pepper,spinach
vegetables.sort(Array.CASEINSENSITIVE);
trace(vegetables); // Avocado,Cilantro,green pepper,Onion,spinach

////////////////////////////////////////////////////////////////////////////////////////////////////

vegetables = new Array();
vegetables.push(new Vegetable("lettuce", 1.49));
vegetables.push(new Vegetable("spinach", 1.89));
vegetables.push(new Vegetable("asparagus", 3.99));
vegetables.push(new Vegetable("celery", 1.29));
vegetables.push(new Vegetable("squash", 1.44));

trace(vegetables);
// lettuce:1.49, spinach:1.89, asparagus:3.99, celery:1.29, squash:1.44

vegetables.sort();

trace(vegetables);
// asparagus:3.99, celery:1.29, lettuce:1.49, spinach:1.89, squash:1.44

////////////////////////////////////////////////////////////////////////////////////////////////////

vegetables = new Array();
vegetables.push(new Vegetable("lettuce", 1.49));
vegetables.push(new Vegetable("spinach", 1.89));
vegetables.push(new Vegetable("asparagus", 3.99));
vegetables.push(new Vegetable("celery", 1.29));
vegetables.push(new Vegetable("squash", 1.44));

trace(vegetables);
// lettuce:1.49, spinach:1.89, asparagus:3.99, celery:1.29, squash:1.44

vegetables.sort(sortOnPrice);

trace(vegetables);
// celery:1.29, squash:1.44, lettuce:1.49, spinach:1.89, asparagus:3.99

function sortOnPrice(a:Vegetable, b:Vegetable):Number {
    var aPrice:Number = a.getPrice();
    var bPrice:Number = b.getPrice();
	
    if(aPrice > bPrice) {
        return 1;
    } else if(aPrice < bPrice) {
        return -1;
    } else  {
        //aPrice == bPrice
        return 0;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

numbers = new Array(3,5,100,34,10);

trace(numbers); // 3,5,100,34,10
numbers.sort();
trace(numbers); // 10,100,3,34,5
numbers.sort(Array.NUMERIC);
trace(numbers); // 3,5,10,34,100

////////////////////////////////////////////////////////////////////////////////////////////////////

vegetables = new Array();
vegetables.push(new Vegetable("lettuce", 1.49));
vegetables.push(new Vegetable("spinach", 1.89));
vegetables.push(new Vegetable("asparagus", 3.99));
vegetables.push(new Vegetable("celery", 1.29));
vegetables.push(new Vegetable("squash", 1.44));

trace(vegetables);
// lettuce:1.49, spinach:1.89, asparagus:3.99, celery:1.29, squash:1.44

vegetables.sortOn("_name");
trace( 'Sorted on _name' );
trace(vegetables);
// asparagus:3.99, celery:1.29, lettuce:1.49, spinach:1.89, squash:1.44

vegetables.sortOn("_price", Array.NUMERIC | Array.DESCENDING);
trace( 'Sorted on _price' )
trace(vegetables);
// asparagus:3.99, spinach:1.89, lettuce:1.49, squash:1.44, celery:1.29

////////////////////////////////////////////////////////////////////////////////////////////////////

var it:Array = new Array();
it.push( { name: 'A', year: 2009 } )
it.push( { name: 'B', year: 2008 } )
it.push( { name: 'C', year: 2007 } )
it.push( { name: 'B', year: 2006 } )
for(var zz:uint = 0; zz < it.length; zz++) {
    trace(it[zz].name + ", " + it[zz].year);
}

trace( 'Sorted:' )
it.sortOn( ['name', 'year'], [Array.DESCENDING, Array.NUMERIC] )
for(var zz1:uint = 0; zz1 < it.length; zz1++) {
    trace(it[zz1].name + ", " + it[zz1].year);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

var records:Array = new Array();
records.push({name:"john", city:"omaha", zip:68144});
records.push({name:"john", city:"kansas city", zip:72345});
records.push({name:"bob", city:"omaha", zip:94010});

for(var k:uint = 0; k < records.length; k++) {
    trace(records[k].name + ", " + records[k].city);
}
// Results:
// john, omaha
// john, kansas city
// bob, omaha

trace("records.sortOn('name', 'city');");
records.sortOn(["name", "city"]);
for(var l:uint = 0; l < records.length; l++) {
    trace(records[l].name + ", " + records[l].city);
}
// Results:
// bob, omaha
// john, kansas city
// john, omaha

trace("records.sortOn('city', 'name');");
records.sortOn(["city", "name"]);
for(var m:uint = 0; m < records.length; m++) {
    trace(records[m].name + ", " + records[m].city);
}
// Results:
// john, kansas city
// bob, omaha
// john, omaha


/*
var myArray : Array = null

myArray = new Array("Flash", "ActionScript", "Republic of Code");
myArray = ["Flash", "ActionScript", "Republic of Code"];

trace(myArray);
trace(myArray[0]);
trace(myArray[2]);

myArray.x = 23
trace(myArray.x)

myArray[3] = "Tutorial";
trace(myArray);

myArray = ["Flash", "ActionScript", "Republic of Code"];
myArray[6] = "Tutorial";
trace(myArray);

myArray = ["Flash", "ActionScript", "Republic of Code"];
myArray.push("Tutorials");
trace(myArray);

myArray = ["Flash", "ActionScript", "Republic of Code"];
myArray.splice(2,1);
trace(myArray);

myArray = ["Flash", "ActionScript", "Republic of Code"];
myArray.splice(1);
trace(myArray);

myArray = ["Flash", "ActionScript", "Republic of Code"];
myArray.pop();
trace(myArray);
trace(myArray.length);

myArray = ["Flash", "ActionScript", "Republic of Code"];
myArray.reverse();
trace(myArray);
*/