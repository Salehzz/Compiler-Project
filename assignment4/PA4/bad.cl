(*
    errors for classes
    undefined parents
    cycle inherit
    redefine Classes
    redefine basic classes
    inherits from int or string or bool
*)
(*
		Class class0 inherits class_und {};

		Class class1 {};
		Class class1 {};

		Class Object {};
		Class IO {};
		Class Int {};
		Class String {};
		Class Bool {};

		Class class2 inherits Int {};
		Class class3 inherits String {};
		Class class4 inherits Bool {};

    Class cycle0 inherits cycle1 {};
		Class cycle1 inherits cycle2 {};
		Class cycle2 inherits cycle0 {};
*)
(*
    redefine attributes
    redefine methods(number of arguments or type of arguments error)
*)
  class Red0 {
    a0 : String <- "1";
  };

  class Red1 inherits Red0 {
    a1 : String <- "1";
  };

  class Red2 inherits Red1 {
    a0 : String <- "1";
    a0 : String <- "1";
    a1 : String <- "1";
    a2 : String <- "1";
    a2 : String <- "1";
  };

  class Redm0 {
    method0 (formal : Int) : Int {{ 0; }};
  };

  class Redm1 inherits Redm0 {
    method1 (formal0 : Int, formal2 : Int) : Int {{ 0; }};
  };

  class Redm2 inherits Redm1 {
    method0 (formal : String) : Int { {0;} };
    method1 (formal0 : Int, formal1: String) : Int { { 1;} };
  };
(*
    undefined TYPE for feature , formals
    self formal , self attributes
    mismatch type in methods
    Undefined feature type, undefined formal type, duplicate formals, self formal, and self attribute
*)
(*
class mis1 {
    attr0 : nottype;
    method1 (formal1 : nottype) : String { {formal1;} };
    method2 (self : nottype) : Int { {0;} };
    self : String <- "salam";
};

  class mis2 inherits mis1{
    attr0 : Int <- True;
    method0 () : SELF_TYPE { attr0 };
};
*)
(*
    type checking error
*)
    class Expressions {
        attr : String <- "salam";
        method (s1 : String, s2 : String) : String { "salam" };

        m0() : Object{{
          notattr <- 1;
          attr <- "salam";
          self <- 1;
        }};

        staticDispatch () : Object {{
          attr@Int.concat("asf");

          attr@SELF_TYPE.concat("khodafez");

          attr@String.notMethod("");

          attr@String.concat(0, 1);

          (new Object)@String.concat("salam");
        }};

        dispatch () : Object {{

          notattr.concat("");

          attr.notMethod("");

        }};

        cond () : Object {{

          if "salam" then 21 else 0 fi;

          if true then attr else notattr fi;

        }};

        loop_ () : Object {{
          while("salam") loop 0 pool;

          while true loop notattr pool;

        }};

        block () : Object {{
            attr;
            notattr;
        }};

        let_ () : Object {{

          let x : String in {
           x+3;
          };

          let x : String <- 5 in {
          x;
          };

          let x : String <- "salam" in {
          x+2;
          };

        }};

        new_ () : Object {{
          new String;
        }};

        isvoid_ () : Object {{
          isvoid notattr;
        }};

        plus_() : Object {{
          notattr + 5;

          true + false;

          "salam" + 42;
        }};

        neg_ () : Object {{
          ~ notattr;
        }};

        eq_ () : Object {{
          noattr = noattr;

          noattr = 5;

          5 = "salam";

        }};

        not_ () : Object {{

          not notattr;

          not attr;
        }};

        object_() : Object {{
          notattr;
        }};
    };
