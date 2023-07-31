%nterm X
%nterm Y
%nterm Z
%nterm T
%nterm W
%nterm V
%term a
%term b
%term c
%term d
%term e
%term t
%term u

%lalr1

%%
X:
    a Y d
    |
    a Z c
    |
    a T
    |
    b Y e
    |
    b Z d
    |
    b T
    ;
Y:
    t W
    |
    u X
    ;
Z:
    t u
    ;
T:
    u X a
    ;
W:
    u V
    ;
V:
    %empty
    ;
%%
//blah blah blah
