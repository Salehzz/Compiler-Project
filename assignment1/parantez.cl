class Main{
main():Object{
(new IO).out_string(parantez((new IO).in_string()))
};
parantez(s:String):String{
(let i:Int<-0 , j:Int<-0, k:Int <-0 in
{
(while(i<s.length()) loop
{

(if(j < k) then "NO\n" else
if(s.substr(i,1)= ")") then k<-k+1 else
{j<-j+1;}
fi fi);
i <- i+1;
}
pool);
(if(j=k) then "YES\n" else
{"NO\n";}
fi);
}
)

};
};