:- module(main, [bigram/2, num_hobbies/2, sentence_match/2]).
:- [kb].

% DO NOT CHANGE THE UPPER CONTENT, WRITE YOUR CODE AFTER THIS LINE

helperv1([A|[B|C]], Rslt) :-
    atom_concat(A, B, Rslt).
helperv1([A|[B|C]], Rslt) :-
    helperv1([B|C], Rslt).

bigram(Attom, Result) :-
    atom_chars(Attom, Weirdoo),
    helperv1(Weirdoo, Result).

% ----------------------------------------

incr(X, X1) :-
    X1 is X+1.

increment(Hobby, [hobby(Hobby, X)|B], ResultList) :- incr(X, X1), append([hobby(Hobby, X1)|B], [], ResultList).
increment(Hobby, [A|B], ResultList) :- increment(Hobby, B, Bumbe), append([A], Bumbe, ResultList).


num_hobbies([], []).
num_hobbies([First_name|Others], ResultList) :-
    person(First_name, _, Hobby),
    num_hobbies(Others, Bum),
    increment(Hobby, Bum, ResultList),
    !.

num_hobbies([First_name|Others], ResultList) :-
    person(First_name, _, Hobby),
    num_hobbies(Others, Bum),
    append([hobby(Hobby, 1)], Bum, ResultList),
    !.
    
% ----------------------------------------

word_match([] , _, Acc, Acc).
word_match(CharacterList1, CharacterList2, InputMap, OutputMap).


helper([], [], OutMap).

sentence_match([], []).
sentence_match([Firts_word|Others], ResultList) :-
    atom_chars(Firts_word, Dabbe).
    
