package {
	import flash.display.MovieClip
	import flash.events.Event
	import classes.Test
	
	public class Closures extends MovieClip {
		public function Closures() {
			var added 		 = new Test( "ADDED" )
			var addedToStage = new Test( "ADDED_TO_STAGE" )
			
			addEventListener( Event.ADDED, added.callback )
			addEventListener( Event.ADDED_TO_STAGE, addedToStage.callback )
			addEventListener( Event.ENTER_FRAME, onEnterFrame )
		}
		
		function onEnterFrame( e ) {
			trace( 'onEnterFrame!' )
			removeEventListener( Event.ENTER_FRAME, onEnterFrame )
		}
	}
	
}
