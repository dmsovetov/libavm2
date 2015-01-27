package classes {
	
	// ** class Foo
	public class Foo {
		public 	  var _public 	 : String = 'public string'
		protected var _protected : String = 'protected string'
		private   var _private   : String = 'private string'
		
		public function write() : void {
			trace( '\tFoo:write' )
			setProperty( '_public', 'Set from Foo:write' )
			setProperty( '_protected', 'Set from Foo:write' )
			setProperty( '_private', 'Set from Foo:write' )
		}
		
		public function read() : void {
			trace( '\tFoo:read' )
			getProperty( '_public' )
			getProperty( '_protected' )
			getProperty( '_private' )
		}
		
		public function writeFromAnonymous() : void {
			trace( 'Writing properties from inside Foo (anonymous)' )
			var thiz  = this
			
			var _write = function( name, value ) {
				try {
					thiz[name] = value
					trace( "\tFoo:writeFromAnonymous : property '" + name + "' is set to " + value )
				} catch( e ) {
					trace( "\tFoo:writeFromAnonymous : write '" + name + "' access error" )
				}
			}
			
			_write( '_public', 'Set from Foo:writeFromAnonymous' )
			_write( '_protected', 'Set from Foo:writeFromAnonymous' )
			_write( '_private', 'Set from Foo:writeFromAnonymous' )
		}
		
		public function readFromAnonymous() : void {
			trace( 'Reading properties from inside Foo (anonymous)' )
			var thiz  = this
			
			var _read = function( name ) {
				try {
					var value = thiz[name]
					trace( "\tFoo:readFromAnonymous : property '" + name + "' is read, value=" + value )
				} catch( e ) {
					trace( "\tFoo:readFromAnonymous : read '" + name + "' access error" )
				}
			}
			
			_read( '_public' )
			_read( '_protected' )
			_read( '_private' )
		}
		
		private function setProperty( name, value )
		{
			try {
				this[name] = value
				trace( '\t\t[x] ' + name )
			} catch( e ) {
				trace( '\t\t[ ] ' + name )
			}
		}
		
		private function getProperty( name )
		{
			try {
				var value = this[name]
				trace( '\t\t[x] ' + name )
			} catch( e ) {
				trace( '\t\t[ ] ' + name )
			}
		}
	}
}
