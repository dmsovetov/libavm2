package classes {
	public class Test {
		private var m_name : String = ""
		
		public function Test( name : String ) {
			trace( 'Test:Test : ' + name )
			m_name = name
		}
		
		public function callback( e ) {
			trace( 'Test:callback : ' + m_name )
		}

	}
}
