%nterm Pair{
     size_t : count;
     std::list<size_t> : lst;
};
%nterm List{
     size_t : count;
     std::list<size_t> : lst;
};
%term LB '('{};
%term RB ')'{};
%%
Goal :
    List {
         std::cout<<$1.count<<'\n';
         std::cout<<'(';
         for(const size_t e : $1.lst){
              std::cout<<e<<' ';
         }
         std::cout<<')'<<std::endl;
    }
List :
    List Pair {
         $$.count=$1.count+$2.count;
         $$.lst.merge($1.lst);
         $$.lst.merge($2.lst);
         $$.lst.push_back(1);
    }|
    Pair {
         $$.count=$1.count;
         $$.lst=std::move($1.lst);
         $$.lst.push_back(2);
    };

Pair :
    '(' Pair ')' {
         $$.count+=$2.count;
         $$.lst=std::move($2.lst);
         $$.lst.push_back(3);
    }|
    '(' ')' {
         ++$$.count;
         $$.lst.push_back(4);
    };
