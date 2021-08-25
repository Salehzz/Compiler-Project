class A {
ana(): Int {
(let x:Int <- 1 in 2)+3
};
};

Class BB__ inherits A {
};
Class List inherits A{

   isNil() : Bool { true };

   head( a : Int )  : Int {{abort();}};

   tail( a : Int ) :  List { { abort(); self; } };

};
Class L {
	a : Int <- 232;

};

Class L {
	isNil() : Bool {if a=b then c<-a else {a<-b+a;} fi };

};

Class L inherits L{
	isNil() : Bool { while a<=b loop {a<-b ; b<-a+b;} pool };

};

Class L {
	isNil() : Bool {let a : Int , b : Int , c : Int in {0; }};

};
