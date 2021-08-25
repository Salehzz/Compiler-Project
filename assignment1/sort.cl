
class List {

   isNil() : Bool { true };

   head()  : Int { { abort(); 0; } };

   tail()  : List { { abort(); self; } };

   cons(i : Int) : List {
      (new Cons).init(i, self)
   };

};

class Cons inherits List {

   car : Int;	
   cdr : List;

   isNil() : Bool { false };

   head()  : Int { car };

   tail()  : List { cdr };

   init(i : Int, rest : List) : List {
      {
	 car <- i;
	 cdr <- rest;
	 self;
      }
   };

};



class Main inherits IO {

   mylist : List;
   mylist1 : List;
   print_list(l : List) : Object {
      if l.isNil() then out_string("\n")
                   else {
			   out_int(l.head());
			   out_string(" ");
			   print_list(l.tail());
		        }
      fi
   };
    i:Int;
    j:Int;
    k:Int;
    l1:List;
    l2:List;
   sort(l : List) : List
	{
        {
        mylist1<-new List;
        while(not l.isNil())  loop
        {
        i<-l.head();j<-0;k<-0;l1<-l;l2<-l;
            while(not l1.isNil()) loop {
                if(i < l1.head()) then {i<-l1.head();j<-1;}
                else if (i = l1.head()) then j<-j+1
                else{i<-i;}
                fi fi;
                l1<-l1.tail();
            }pool;
            while(k<j) loop
            {
                k<-k+1;
                mylist1<-mylist1.cons(i);
            }pool;
            l<-new List;
            while(not l2.isNil()) loop{
                
                (if(l2.head()<i) then {l<-l.cons(l2.head());l2<-l2.tail();}else
                    {l2<-l2.tail();}
                    fi);
            }pool;
        }pool;
        mylist1;
        }
    };
    
   main() : Object {
      {
	 mylist<-new List;
     (let i:Int<-0,n:Int in{
         n<-(new IO).in_int();
         while (i < n) loop {
            mylist<-mylist.cons(in_int());
             i<-i+1;
	    } pool;
          }
          );
          
          print_list(sort(mylist));
	 
      }
   };

};