:- module(main, [sum_age/2, max_age_of_hobby/3, person_in_range/4]).
:- [kb].

% DO NOT CHANGE THE UPPER CONTENT, WRITE YOUR CODE AFTER THIS LINE

one_age(Name, OneAge) :-
    person(Name, Age, _), OneAge is Age.

sum_age([], 0).
sum_age([Name|B], TotalAge) :- 
    one_age(Name, OneAge),
    sum_age(B, Remaining),
    TotalAge is Remaining + OneAge.

% --------------------------------------------------

find_person(Name, Hobby, Aop) :-
    person(Name, Age, Hobby), Aop is Age.
find_person(_, _, 0).

helper2([] , _, Acc, Acc).
helper2([First_Name|Name_Tail] , Hobby, Acc, MaxAge):- 
    find_person(First_Name, Hobby, HowOld),
    HowOld >= Acc,
    helper2(Name_Tail, Hobby, HowOld, MaxAge),
    !.
helper2([First_Name|Name_Tail] , Hobby, Acc, MaxAge):- 
    find_person(First_Name, Hobby, HowOld),
    HowOld =< Acc,
    helper2(Name_Tail, Hobby, Acc, MaxAge),
    !.

max_age_of_hobby(NameList , Hobby, MaxAge) :- helper2(NameList , Hobby, 0, MaxAge).
    
% --------------------------------------------------

is_in_range(Min, Max, Value):-
    Value =< Max,
    Value >= Min,
    !.
    
person_in_range([], _, _, []).
person_in_range([First_Name|Tail], Min, Max, ResultList):-
    person(First_Name, HowOld, _),
    is_in_range(Min, Max, HowOld),
    person_in_range(Tail, Min, Max, Bum),
    append([First_Name], Bum, ResultList),
    !.
person_in_range([_|Tail], Min, Max, ResultList):-
    person_in_range(Tail, Min, Max, ResultList).
