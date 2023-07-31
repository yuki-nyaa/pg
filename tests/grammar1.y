%nterm X
%nterm Y
%nterm Z
%term a
%term c
%term d

%%
X:
    Y
    |
    a
    ;
Y:
    c
    |
    %empty
    ;
Z:
    X Y Z
    |
    d
    ;
%%
//blah blah blah
