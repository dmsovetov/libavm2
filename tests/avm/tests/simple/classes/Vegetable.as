package classes {
	
	//The following code defines the Vegetable class
	public class Vegetable {
		public var _name : String;
		public var _price: Number;
	
		public function Vegetable(name:String, price:Number) {
			this._name  = name;
			this._price = price;
		}
		
		public function getPrice():Number {
        	return _price;
    	}
	
		public function toString():String {
			return " " + _name + ":" + _price;
		}
	}
		
}
