package classes {
	
	public class ArrayMultiplier {
		private var m_amount : Number = 0
		
		public function ArrayMultiplier( amount : Number ) {
			m_amount = amount
		}
		
		public function get thunk() : Function { return createFunction() }
		public function set amount( value : Number ) : void { m_amount = value }
		
		public function createFunction() : Function {
			return function( x : int, index : int, array : Array ) : Number {
				return x * m_amount
			}
		}

		static public function multiplyBy( target : Array, by : Number ) : Array {
			var multiplier = new ArrayMultiplier( by )
			return target.map( multiplier.createFunction() )
		}

	}
	
}
