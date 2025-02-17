team(connecticut, 22, 2, east).
team(duke, 24, 1, east).
team(villanova, 20, 2, east).
team(georgeWashington, 20, 1, east).
team(florida, 21, 3, east).
team(westVirginia, 18, 6, east).
team(bostoncollege, 20, 5, east).
team(georgetown, 17, 5, east).
team(northCarolinaState, 19, 5, east).
team(northCarolina, 15, 6, east).
team(bucknell, 20, 3, east).

zone(X, Y) :-
	team(X, _, _, Z),
	team(Y, _, _, Z).

schedule(_).

maxInZone(Zone, MaxTeam, TeamName) :-
	findall(X, team(_, X, _, Zone), Y),
	max_list(Y, MaxTeam),
	team(TeamName, MaxTeam, _, Zone).

generateTeamPairs(Zone,Pairs) :-
	findall(TeamName, team(TeamName, _, _, Zone), TeamNames),
	permutation(Pairs, TeamNames).

sortedByWins(TeamNames, Wins, Zone) :-

