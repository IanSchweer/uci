male(jim).
male(tom).
male(joe).
male(bob).
female(fay).
female(beth).
female(sue).
female(cami).
hobby(jim, sup).
hobby(fay, sup).
hobby(jim, fish).
hobby(tom, fish).
hobby(beth, fish).
hobby(jim, kayak).
hobby(cami, kayak).
hobby(tom, ski).
hobby(sue, ski).
hobby(joe, gamer).
hobby(cami, gamer).
hobby(joe, climb).
hobby(beth, climb).
hobby(bob, yoga).
hobby(sue, yoga).
hobby(bob, run).
hobby(cami, run).
hobby(beth, cycle).
hobby(bob, paint).
hobby(joe, chess).
hobby(tom, hike).
hobby(sue, skate).
seatingChart([P1, P2, P3, P4, P5, P6, P7, P8]) :-
	permutation([P1, P2, P3, P4, P5, P6, P7, P8], [jim, tom, joe, bob, fay, beth, sue, cami]),
	\+error(P1, P2),
	\+error(P2, P3),
	\+error(P3, P4),
	\+error(P4, P5),
	\+error(P5, P6),
	\+error(P7, P8),
	\+error(P8, P1).

error(X, Y) :- male(X), male(Y); female(X), female(Y).
error(X, Y) :-
	findall(Hobby, (hobby(X, Hobby), hobby(Y, Hobby)), Z),
	length(Z,0).

