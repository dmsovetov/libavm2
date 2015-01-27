package classes {
	import flash.display.MovieClip
	import flash.events.Event
	
	public class DocumentEvents extends MovieClip {
		public function DocumentEvents() {
			trace( 'DocumentEvents:DocumentEvents - begin' )
		
			trace( 'addEventListener : Event.ADDED' )
			addEventListener( Event.ADDED, onAdded )
			
			trace( 'addEventListener : Event.ADDED_TO_STAGE' )
			addEventListener( Event.ADDED_TO_STAGE, onAddedToStage )
			
			trace( 'addEventListener : Event.ENTER_FRAME' )
			addEventListener( Event.ENTER_FRAME, onEnterFrame )
			
			trace( 'DocumentEvents:DocumentEvents - end' )
		}
		
		public function onAdded( e ) {
			trace( 'onAdded' )
		}
		
		private function onAddedToStage( e ) {
			trace( 'onAddedToStage' )
		}
		
		function onEnterFrame( e ) {
			trace( 'onEnterFrame' )
			
			trace( 'removeEventListener : Event.ENTER_FRAME' )
			removeEventListener( Event.ENTER_FRAME, onEnterFrame )
		}
	}
}
