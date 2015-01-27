import classes.*

var bob : Zombie = new Zombie

trace( bob.m_health )
//trace( bob.m_name )	// runtime error
trace( bob.m_speed )

Entity.dump( bob, 'before' )

bob.m_health = 1
//bob.m_name   = 'bob'	// runtime error
//bob.m_target = bob	// runtime error
//bob.m_speed  = 22		// runtime error

trace( bob.m_health )
//trace( bob.m_name )	// runtime error
trace( bob.m_speed )

Entity.dump( bob, 'after' )