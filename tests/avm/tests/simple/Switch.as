var lang = "Spanish"

traceLanguage( 'Spanish' )
traceLanguage( 'Arabic' )
traceLanguage( 'English' )
traceLanguage( 'trololo' )

function traceLanguage( lang ) {
	switch (lang){
		case "English":
		trace("Hi!");
		break;
		
		case "Arabic":
		trace("Ahlan!");
		break;
		
		case "Spanish":
		trace("Hola!");
		break;
		
		default: trace( 'Unknown language ' + lang )
	}
}