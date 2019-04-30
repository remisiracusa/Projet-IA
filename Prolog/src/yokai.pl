/* -*- Mode:Prolog; coding:iso-8859-1; indent-tabs-mode:nil; prolog-indent-width:8; prolog-paren-indent:3; tab-width:8; -*- */

:- set_prolog_flag(toplevel_print_options,[max_depth(0)]).
:-use_module(library(plunit)).
:-use_module(library(lists)).
:-use_module(library(clpfd)).

% Déplacements : N, NE, E, SE, S, SO, O, NO

% Pions sud
% 1 = Kodama => N
% 2 = Kodama Samourai => N, NE, E, S, O, NO
% 3 = Kirin => N, NE, E, S, O, NO
% 4 = Koropokkuru => N, NE, E, SE, S, SO, O, NO
% 5 = Oni => N, NE, SE, SO, NO
% 6 = Super Oni => N, NE, E, S, O, NO

% Pions nord
% 7  = Kodama => S
% 8  = Kodama Samourai => N, E, SE, S, SO, O
% 9  = Kirin => N, E, SE, S, SO, O
% 10 = Koropokkuru => N, NE, E, SE, S, SO, O, NO
% 11 = Oni => NE, SE, S, SO, NO
% 12 = Super Oni => N, E, SE, S, SO, O

initialisationGrille([11,9,10,9,11,
                       0,0,0 ,0,0,
                       0,7,7 ,7,0,
                       0,1,1 ,1,0,
                       0,0,0 ,0,0,
                       5,3,4 ,3,5]).

memberDif(X,[X|LVal],LVal).
memberDif(X,[Y|LVal],[Y|NLVal]):-
        memberDif(X,LVal,NLVal).

deplacementCarte(1,['N']).
deplacementCarte(2,['N', 'NE', 'E', 'S', 'O', 'NO']).
deplacementCarte(3,['N', 'NE', 'E', 'S', 'O', 'NO']).
deplacementCarte(4,['N', 'NE', 'E', 'SE', 'S', 'SO', 'O', 'NO']).
deplacementCarte(5,['N', 'NE', 'SE', 'SO', 'NO']).
deplacementCarte(6,['N', 'NE', 'E', 'S', 'O', 'NO']).

deplacementCarte(7,['S']).
deplacementCarte(8,['N', 'E', 'SE', 'S', 'SO', 'O']).
deplacementCarte(9,['N', 'E', 'SE', 'S', 'SO', 'O']).
deplacementCarte(10,['N', 'NE', 'E', 'SE', 'S', 'SO', 'O', 'NO']).
deplacementCarte(11,['NE', 'SE', 'S', 'SO', 'NO']).
deplacementCarte(12,['N', 'E', 'SE', 'S', 'SO', 'O']).

%Retourne l'indice en fonction de la Carte
indiceCarte(NumC, Ind, Grille):-
        nth1(Ind, Grille, NumC).

%Retourne une direction possible de la carte
directionPossible(NumC, Ind, NewInd, Grille):-
        deplacementCarte(NumC, LDepl),
        memberDif(Dir, LDepl,_),
        indiceCarte(NumC, Ind, Grille),
        directionGrilleValide(Ind, Dir),
        directionToInd(Ind,Dir,NewInd).

directionToInd(Ind,'N',NewInd):-NewInd is Ind-5.
directionToInd(Ind,'NE',NewInd):-NewInd is Ind-4.
directionToInd(Ind,'E',NewInd):-NewInd is Ind+1.
directionToInd(Ind,'SE',NewInd):-NewInd is Ind+6.
directionToInd(Ind,'S',NewInd):-NewInd is Ind+5.
directionToInd(Ind,'SO',NewInd):-NewInd is Ind+4.
directionToInd(Ind,'O',NewInd):-NewInd is Ind-1.
directionToInd(Ind,'NO',NewInd):-NewInd is Ind-6.

/*
deplacementCarteValide(_, NewInd, Grille):-
        indiceCarte(NumC, NewInd, Grille),
        NumC == 0.
*/

deplacementCarteValide(Joueur, NewInd, Grille):-
        indiceCarte(NumC, NewInd, Grille),
        cartesJoueur(Joueur, LCartes),
        \+member(NumC, LCartes).
        
        
joueurAdverse(top, bottom).
joueurAdverse(bottom,top).

% Controler déplacement valide en fonction des cartes
cartesJoueur(bottom,[1,2,3,4,5,6]).
cartesJoueur(top,[7,8,9,10,11,12]).

deplacerCarte(Ind, NewInd, Grille, NewGrille, CarteCapturee):-
        nth1(NewInd, Grille, CarteCapturee),
        nth1(Ind, Grille, Carte),
        editValeur(Ind, 0, Grille, NewGrille1),
        editValeur(NewInd, Carte, NewGrille1, NewGrille).

editValeur(1, NewVal, [_|Grille], [NewVal|Grille]).

editValeur(Ind, NewVal, [E|Grille], [E|NewGrille]):-
        NewInd is Ind-1,
        editValeur(NewInd, NewVal, Grille, NewGrille).
        

deplacement(Joueur, Grille, NewGrille, CarteCapturee):-
        cartesJoueur(Joueur, LCartes),
        memberDif(NumC, LCartes, _),
        directionPossible(NumC, Ind, NewInd, Grille),      % Retourne Ind & Dir
        deplacementCarteValide(Joueur, NewInd, Grille),
        deplacerCarte(Ind, NewInd, Grille, NewGrille, CarteCapturee).
        

directionGrilleValide(Ind, 'N'):- Ind > 5.

directionGrilleValide(Ind, 'NE'):-
        Ind > 5,
        Mod is mod(Ind,5),
        Mod \= 0.

directionGrilleValide(Ind, 'E'):- 
        Mod is mod(Ind,5),
        Mod \= 0.

directionGrilleValide(Ind, 'SE'):-
        Ind < 25,
        Mod is mod(Ind,5),
        Mod \= 0.

directionGrilleValide(Ind, 'S'):- Ind < 25.

directionGrilleValide(Ind, 'SO'):-
        Ind < 25,
        Mod is mod(Ind,5),
        Mod \= 1.

directionGrilleValide(Ind, 'O'):-
        Mod is mod(Ind,5),
        Mod \= 1.

directionGrilleValide(Ind, 'NO'):-
        Ind > 5,
        Mod is mod(Ind,5),
        Mod \= 1.

/* Pour tester
deplacement(top, [11,9,10,9,11,0,0,0,0,0,0,7,7,7,0,0,1,1,1,0,0,0,0,0,0,5,3,4,3,5], NewGrille, CarteCapturee).
*/
