
(*
 *  execute "coolc bad.cl" to see the error messages that the coolc parser
 *  generates
 *
 *  execute "myparser bad.cl" to see the error messages that your parser
 *  generates
 *)

(* no error *)
class A {
};

(* error:  b is not a type identifier *)
Class b inherits A {
};

(* error:  a is not a type identifier *)
Class C inherits a {
};

(* error:  keyword inherits is misspelled *)
Class D inherts A {
};

(* error:  closing brace is missing *)
Class E inherits A {
;

(*error : harf aval isnil bozorge , A bozorge , int bayad Int bashe , list ke harf avalesh kochike*)
(*formal error , feature error*)
Class List inherits A{

   IsNil()  Bool { true };

   head( A : int )  : Int {1;  };

   tail( a : Int ) :  list { { abort(); self; } };

};
Class L {
(* expr error*)
	a : Int <- '232';

};

Class L {
(* expr error*)
	isNil() : Bool {if a=b then c<-a else {a<-b+a;}f};

};

Class L {
(*pool : expr error*)
	isNil() : Bool {while {a<b} loop{ a<-b ; b<-a+b ; {123;}} pol};

};

Class L {
(*pool : expr error*)
	isNil() : Bool {let a : Int , b : int , c : Int in {0; }};

};


