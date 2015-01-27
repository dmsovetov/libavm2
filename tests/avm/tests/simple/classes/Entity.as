
// ** class Entity
dynamic class Entity {
	private var m_name : String = null

	public function Entity( name : String ) {
		m_name = name

		trace( 'Entity ' + name + ' created!' )
	}

	static public function dump( entity, param ) : void {
		trace( '-----------' + entity + '-----------' )
		trace( 'Entity::dump : ' + entity.m_name + ' /' + param + '/' )
		entity.checkProperty( 'm_name' )
		entity.checkProperty( 'm_health' )
		entity.checkProperty( 'm_speed' )
		entity.checkProperty( 'm_target' )
		entity.checkProperty( 'dump' )
		entity.checkProperty( 'checkProperty' )
		trace( '----------------------------------' )
	}

	
	function checkProperty( name ) {
		var result = hasOwnProperty( name )
		if( result ) {
			trace( this + " '" + name + "' = " + this[name] )
		} else {
			trace( this + " has no property '" + name + "'" )
		}
	}
}
