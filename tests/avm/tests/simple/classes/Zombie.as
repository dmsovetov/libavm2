include 'Entity.as'

// ** class Zombie
dynamic class Zombie extends Entity {
	public var   m_health : Number  = 666
	public const m_speed  : int		= 0

	public function Zombie( name : String = 'zombie', health : Number = 100 ) {
		super( name )

		m_health = health
		m_speed  = 12

		trace( 'Zombie ' + name + ' created!' )
	}
}
