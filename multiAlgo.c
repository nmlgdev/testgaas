/* Multi-Algo simple rote/reinforcement, inductive, random */

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <alloc.h>
#include <dos.h>

#define MAX_STAX 4
#define INITIAL_STACK_HEIGHT 7
#define SIZE_OF_RL_ARRAY 4097   /* = (7 + 2)^4 + 1 */
#define FALSE 0
#define TRUE 1
#define BAD 0
#define GOOD 1

unsigned _stklen =20000;

char reenforcement_on, display_on, another[20];
char player_A, player_B;
char Stack[MAX_STAX+1];
int No_of_Stacks=MAX_STAX;
int No_of_Games, Display_Freq, No_of_Trials, Trial;

/* RL variables */
int position[SIZE_OF_RL_ARRAY];
struct RL_moves {
        int m_pos;
        struct RL_moves *next_move_ptr;
        };
struct RL_moves *first_move; /* = pointer to RL move values */

/* ID3 variables */
int ID_rec[2000][11];
int Classifier[100][3];
int C_ptr;
struct Dtree_rec {
        int element;
        int value;
        int class;
        struct Dtree_rec *goodies;
        struct Dtree_rec *baddies;
        };
struct Dtree_rec *First_decision;


/* Function headers not given, functions are in reverse order as in Pascal */


void Sort_the_stacks(char Stack[MAX_STAX], int *No_of_Stacks)
/* sorts the stacks using insertion sort */

{
int A,B,Start_of_B,Val_of_A;

A=2;
while (A<=MAX_STAX) {
        if (Stack[A] > Stack[A-1]) {
                B=A-1;
                while ((B>0) && (Stack[B]<Stack[A])) B--;
                B++;
                Val_of_A=Stack[A];
                Start_of_B=B;
                while (B<A) {
                        Stack[B+1]=Stack[B];
                        B++;
                        }
                Stack[Start_of_B]=Val_of_A;
                }
        A++;
    }

/* now count the number of stacks left */

for(B=1, *No_of_Stacks=0;  B<=MAX_STAX;  B++)
        if (Stack[B]>0) *No_of_Stacks+=1;
}



void Draw_the_Stacks(void)
{
char s;
for(s=1;s<=No_of_Stacks;s++) printf("%d ",Stack[s]);
printf("\n");
}



void Random_players_go(void)
{
char Pile;

if (display_on) printf("Random player's move: ");
Pile=random(No_of_Stacks)+1;
Stack[Pile]-=random(Stack[Pile])+1;
}



int Total(char Stack[MAX_STAX], char cols)
{
/* function to find the integer total of the binary values of the
 numbers in Stack */
int num,tot;

tot=0;
for(num=1;num<=cols;num++) {
        switch (Stack[num]) {
                case 0 : break;
                case 1 : tot+=1; break;
                case 2 : tot+=10; break;
                case 3 : tot+=11; break;
                case 4 : tot+=100; break;
                case 5 : tot+=101; break;
                case 6 : tot+=110; break;
                case 7 : tot+=111; break;
                }
        }
return tot;
}



char odd(int num)
{
if ((num % 2)==0) return FALSE;
else return TRUE;
}



char Class(char Stack[MAX_STAX], char cols)
{
/* Evaluates a given game position to see if it is good or bad */
int tot,d,digit[3];

tot=Total(Stack,cols); /* now holds int total of binary values */
digit[0]=tot / 100;
tot=tot - (100*digit[0]);
digit[1]=tot / 10;
tot=tot - (10*digit[1]);
digit[2]=tot;
if ( (((odd(digit[0])) || (odd(digit[1])) || (odd(digit[2]))) && !((!digit[0]) && (!digit[1]) && (odd(digit[2])))) || ((!digit[0]) && (!digit[1]) && (!odd(digit[2])))) return BAD;
else return GOOD;
}


void Expert_players_go(void)
{
char col, tiles, best_col, best_tiles, move;

if (display_on) printf("Expert player's move: ");
if (No_of_Stacks<2)
         if (Stack[1]==1) Stack[1]=0;
         else Stack[1]=1;
else {
        move=BAD;
        col=1;
        tiles=0;
        do {
                tiles++;
                if (tiles>Stack[col]) {
                        tiles=1;
                        col++;
                        }
                if (col<=(No_of_Stacks-1)) {
                        Stack[col]-=tiles; /* initial move */
                        move=Class(Stack,No_of_Stacks);  /* evaluate */
                        if (move==BAD) Stack[col]+=tiles;   /* restore position if bad */
                        }
                } while ((move!=GOOD) && (col<=No_of_Stacks));
        if (col>No_of_Stacks) Stack[No_of_Stacks]--;
         /* - hopeless position so make a small move */
         }
}



void Human_players_go(void)
{
int col=0,tiles=0;

printf("Column/Tiles taken: (e.g. 12Enter):");
col=getchar()-48;
tiles=getchar()-48;

while ((col<1) || (col>No_of_Stacks) || (tiles<1) || (tiles>Stack[col])) {
        printf("\nColumn/Tiles taken: (e.g. 12Enter):");
        col=getchar()-48;
        tiles=getchar()-48;
        }
Stack[col]-=tiles;
printf("Your move:      ");
}



int Position_value(char Stack[MAX_STAX])
/* Finds the position in the RL array of a game positions reenforcement value */
{
int move,power,height,c;

height=INITIAL_STACK_HEIGHT+1;
move=1;
power=1;
for (c=1; c<No_of_Stacks; c++) {
        move+=power*Stack[c];
        power*=height;
        }
return move;
}


void Initialise_rote_learning(void)
{
int p;
position[0]=-32768; /* forces it to make another move */
for (p=1; p<SIZE_OF_RL_ARRAY; p++) position[p]=1; /* = halfway */
first_move=NULL;
}


void RL_update_database(char result)
/* updates the RL algorithm database */
{
int current_pos;
struct RL_moves *last_move;

last_move=first_move;
while (first_move!=NULL) {
        current_pos=first_move->m_pos;
        if (result==GOOD) position[current_pos]+=1;
        else position[current_pos]-=1;
        first_move=first_move->next_move_ptr;
        farfree(last_move);
        last_move=first_move;
        }
}


void Rote_learnings_go(void)
{
int move,best_so_far,b_c,b_t,col,tiles,c,power;
struct RL_moves *chosen;

if (display_on) printf("Rote learnings move:   ");
best_so_far=0;
col=1;
while (col<=No_of_Stacks) {
        tiles=1;
        while (tiles<=Stack[col]) {
                Stack[col]-=tiles;
                /* find position for this state */
                move=Position_value(Stack);
                if (position[move] > position[best_so_far]) {
                        best_so_far=move;
                        b_c=col;
                        b_t=tiles;
                        }
                Stack[col]+=tiles;
                tiles++;
                }
        col++;
        }
if (best_so_far==0) {  /* make a random move because all of the others were hopeless */
        if (display_on) printf("- random move - ");
        b_c=random(No_of_Stacks+1);
        b_t=random(Stack[b_c]+1);
        Stack[b_c]-=b_t; /* temporarily */
        best_so_far=Position_value(Stack);
        Stack[b_c]+=b_t; /* reset */
        }
/* now make the move */
Stack[b_c]-=b_t;
if (reenforcement_on) {  /* record the move made */
        if (!(chosen=(struct RL_moves *)farmalloc(sizeof(struct RL_moves)))) {
                printf("\nDynamic memory allocation error (RL)- aborting\n");
                exit(0);
                }
        chosen->m_pos=best_so_far;
        chosen->next_move_ptr=first_move;
        first_move=chosen;
        }
}






void Partition(struct Dtree_rec *node, int first, int last)

/* recursively partitions the ID_rec array according to the best discriminatory value */
{
int best_element,best_value,already_used,attr_value;
int CP,col,pos,goods,p,attrs,best_attrs;
int new_f,new_l, temp;
struct Dtree_rec *sub_node_goodies;
struct Dtree_rec *sub_node_baddies;

/* Firstly see if partitioning is necessary by counting the number of identical classifications */


goods=0;
for (pos=first;pos<last;pos++) if (ID_rec[pos][10]==GOOD) goods++;

/* initially assume that this node is a leaf */
node->element=0;
node->value=0;
node->goodies=NULL;
node->baddies=NULL;

if (goods > (last-first)) {
        node->class=GOOD;
        printf("C=*%d\n",node->class);
        return; /* all class good so exit now */
        }
else if (goods==0) {
        node->class=BAD;
        printf("C=*%d\n",node->class);
        return; /* all class bad so exit now */
        }
else node->class=random(2);


/* 5% cutoff - assigns the result to the majority of the remaining
   attribute records:
if ((last-first+1) < (No_of_Games/20)) {
        printf("C=%d\n",node->class);
        return;
        }
*/

/* partition this set further */
  best_element=0;
  best_value=0;
  best_attrs=0;
  /* find the most discriminatory variables = best_... */
  for(col=1;col<=9;col++) {   /* 9 attributes in table */
        for(pos=first;pos<=last;pos++) {
                /* determine whether this has already been used */
                already_used=FALSE;
                CP=1;
                while ((!already_used) && (CP<=C_ptr)) {
                        if (Classifier[CP][1]==col) if  (Classifier[CP][2]==ID_rec[pos][col]) already_used=TRUE;
                        CP++;
                        }
        /*?     if ((ID_rec[pos][5]<col) && (col<5)) already_used=TRUE;  cuts out columns of zero (efficiency)*/
                if (already_used==FALSE) {
                        attr_value=ID_rec[pos][col];
                        attrs=0; /* = no of lines which have this attribute */
                        for(p=first;p<=last;p++) if (ID_rec[p][10]==GOOD) if (ID_rec[p][col]==attr_value) attrs++;
                        if (attrs>best_attrs) {
                                best_attrs=attrs;
                                best_element=col;
                best_value=attr_value;
                                }
                        }
                }
        }
  if ((best_attrs > 0) && (best_attrs < (last-first+1))) { /* data partition has been found */
        /* store the attributes chosen */
        C_ptr++;
        Classifier[C_ptr][1]=best_element;
        Classifier[C_ptr][2]=best_value;
        CP=C_ptr;
        /* temporarily store the C_ptr so that each subtree can use the same attributes */
        printf("%d=%d",best_element,best_value);
        /* partition ID_rec into two, GOODIES at bottom, BADDIES at top */
        pos=first;
        new_f=first-1;
        while (pos<last) {
                if (ID_rec[pos][best_element]==best_value) /* swap pos with new_f */
                        new_f++;
                        for(col=1;col<=10;col++) {
                                temp=ID_rec[pos][col];
                                ID_rec[pos][col]=ID_rec[new_f][col];
                                ID_rec[new_f][col]=temp;
                                }
                pos++;
                }
        new_l=new_f+1;
        /* Now insert partitioning attributes into the node */
        node->element=best_element;
        node->value=best_value;
        /* ignore node->class since we haven't reached the end yet */

        if (!(sub_node_goodies=(struct Dtree_rec *)farmalloc(sizeof(struct Dtree_rec)))) {
                printf("\nID3 Dynamic memory allocation error - aborting\n");
                exit(0);
                }
        if (!(sub_node_baddies=(struct Dtree_rec *)farmalloc(sizeof(struct Dtree_rec)))) {
                printf("\nID3 Dynamic memory allocation error - aborting\n");
                exit(0);
                }

        node->goodies=sub_node_goodies;
        node->baddies=sub_node_baddies;
        printf("---");
        Partition(sub_node_goodies,first,new_f);
        C_ptr=CP; /*restore C_ptr again for other branch */
        for(col=1;col<=CP;col++) printf("---");
        Partition(sub_node_baddies,new_l,last);
        }
for(col=1;col<CP;col++) printf("   ");
printf("C=%d\n",node->class);
}



void Find_CT_attr(char Stack[MAX_STAX], char No_of_Stacks, int attr[13])
/* Finds the set of contingency table attributes from a given game partition */
{
int temp,tot,col;

for(col=1; col<=MAX_STAX; col++) attr[col]=Stack[col];
attr[5]=No_of_Stacks;
temp=0;
for(col=1;col<=No_of_Stacks;col++) temp+=Stack[col];
attr[6]=temp;
attr[7]=abs(Stack[2]-Stack[1]);
attr[8]=abs(Stack[3]-Stack[2]);
attr[9]=abs(Stack[4]-Stack[3]);
attr[10]=Class(Stack,No_of_Stacks);
}



void Initialise_induction(void)
/* Forms the training set and creates the decision tree from a
   series of positions classified according to C.L.Bouton's algorithm */
{
int pos,col,b;
struct Dtree_rec *Dtree_ptr;

No_of_Stacks=0;
for(pos=1;pos<=No_of_Games;pos++) {
        if (No_of_Stacks==0) { /* reset stacks to initial height */
                for(col=1;col<=MAX_STAX;col++) Stack[col]=INITIAL_STACK_HEIGHT;
                No_of_Stacks=MAX_STAX;
                }
        Random_players_go();
        Sort_the_stacks(Stack,&No_of_Stacks);
        Find_CT_attr(Stack,No_of_Stacks,ID_rec[pos]);
        }
/* now create the decision tree */
if (!(Dtree_ptr=(struct Dtree_rec *) farmalloc(sizeof(struct Dtree_rec)))) {
                printf("\nID3 dynamic memory allocation error - aborting\n");
                exit(0);
                }
First_decision=Dtree_ptr;
C_ptr=0;
Partition(First_decision,1,No_of_Games);
}




int ID_find_outcome(struct Dtree_rec *node,int attr[13])
/* finds the class of a game position by traversing the decision tree */
{

if(node->goodies==NULL) return node->class; /* stopping case */
else if (attr[node->element]==node->value)
                return ID_find_outcome(node->goodies,attr);
                else return ID_find_outcome(node->baddies,attr);
}




void Inductions_go(void)
/* ID3's move */
{
int move_type=BAD,col,tiles,NOS,s;
struct Dtree_rec *node;
char move[MAX_STAX+1];
int attr[13];

if (display_on) printf("Inductions move:      ");
/* evaluate all possible moves until a good move is found */
col=1;
tiles=0;
while ((move_type==BAD) && (col<=No_of_Stacks)) {
        tiles++;
        if (tiles>Stack[col]) {
                tiles=1;
                col++;
                }
        Stack[col]-=tiles;
        for(s=1;s<No_of_Stacks;s++) move[s]=Stack[s];
        Sort_the_stacks(move,&NOS);
        Find_CT_attr(move,NOS,attr);
        move_type=ID_find_outcome(First_decision,attr);
        if (move_type==BAD) Stack[col]+=tiles;
        }

if (move_type==BAD) { /* no good move found so make a random move */
        col=random(No_of_Stacks)+1;
        Stack[col]-=random(Stack[col])+1;
        if (display_on) printf(" - random move - ");
        }
}




void Stats_go(void)
/* generates moves using a static evaluation function */
{
char col,tiles,class=BAD;


if (display_on) printf("SEF's move:        ");
/* evaluate all possible moves until a good move is found */
col=1;
tiles=0;
while ((class==BAD) && (col<=No_of_Stacks)) {
        tiles++;
        if (tiles>Stack[col]) {
                tiles=1;
                col++;
                }
        if (col<=No_of_Stacks) {
                Stack[col]-=tiles;
                /* classify this position */
                if (((Stack[1]==Stack[2]) && (Stack[1]>1)) || ((Stack[2]==0) && (Stack[1]==1))) class=GOOD;
                                else class=BAD;
                if (class==BAD) Stack[col]+=tiles;
                }
        }
if (class==BAD) { /* no good move found so make a random move */
    col-=random(No_of_Stacks)+1;
        Stack[col]-=random(Stack[col])+1;
        if (display_on) printf(" - random move - ");
        }
}




void Play_a_series(char Player_A, char Player_B, int No_of_Games)
/* playes 'No_of_Games' games of Player_A against Player_B */
{
int games_played,game_number,player_A_wins,game_over;
char winner;

printf("\n\n****** Playing %c against %c\n\n",Player_A,Player_B);
player_A_wins=0;
for(games_played=1;games_played<=No_of_Games;games_played++) {
        game_number=games_played;
        if ((game_number % Display_Freq)==0) display_on=TRUE;
        else display_on=FALSE;
        /* play_a_game */
        if (display_on) printf("Game %d.\n",game_number);
        /* reset stacks to max height */
        for(No_of_Stacks=1;No_of_Stacks<=MAX_STAX;No_of_Stacks++) Stack[No_of_Stacks]=INITIAL_STACK_HEIGHT;
        No_of_Stacks=MAX_STAX;
        game_over=FALSE;
        winner=' ';
        /* make the first move */
        Random_players_go();
        Sort_the_stacks(Stack,&No_of_Stacks);
        if (display_on) Draw_the_Stacks();
        /* now take subsequent turns */
        while (!game_over) {
                switch (Player_A) {
                        case 'r' : Rote_learnings_go(); break;
                        case '*' : Random_players_go(); break;
                        case 'e' : Expert_players_go(); break;
                        case 'h' : Human_players_go(); break;
                        case 'i' : Inductions_go(); break;
                        case 's' : Stats_go();break;
                        default : Random_players_go();
                        }
                Sort_the_stacks(Stack,&No_of_Stacks);
                if (display_on) Draw_the_Stacks();
                if (No_of_Stacks==0) {
                        game_over=TRUE;
                        winner=Player_B;
                        }
                if (!game_over) {
                        switch (Player_B) {
                                case 'r' : Rote_learnings_go(); break;
                                case '*' : Random_players_go(); break;
                                case 'e' : Expert_players_go(); break;
                                case 'h' : Human_players_go(); break;
                                case 'i' : Inductions_go(); break;
                                case 's' : Stats_go();break;
                                default : Random_players_go();
                                }
                        Sort_the_stacks(Stack,&No_of_Stacks);
                        if (display_on) Draw_the_Stacks();
                        if (No_of_Stacks==0) {
                                game_over=TRUE;
                                winner=Player_A;
                                }
                        }
                }
        /* update database if game is finished */
        if ((Player_A=='r') || (Player_B=='r'))
                if (winner=='r') RL_update_database(GOOD);
                else RL_update_database(BAD);
        if (winner==Player_A) player_A_wins++;
        if (display_on) printf("Won by %c.  Player %c has won %d / %d.", winner,Player_A,player_A_wins,game_number);
        }
}





void main(void)

{
Initialise_rote_learning();
do {
        printf("\n\nMachine learning algorithm tester on game of NIM.\n\n");
        printf("Players available:\nrote learning, inductive, human, static evaluation function, expert, *random\n");
        printf("Please choose which two are to play (type first letters, * for random)\n");
        while ((strchr("hreis*",(player_A=getchar())))==NULL);
        while ((strchr("hreis*",(player_B=getchar())))==NULL);
        No_of_Games=0;
        getch();
        while ((No_of_Games<1) || (No_of_Games>2000)) {
                printf("\nHow many games do you want played?\n (also=ID3 training set size) (range 1-2000) ");
                gets(another);
                No_of_Games=atoi(another);
                }
        Display_Freq=0;
        while ((Display_Freq<1) || (Display_Freq>2000) && !((player_A=='h') || (player_B=='h'))) {
                printf("\nHow often do you want the games displayed? (1-2000) ");
                gets(another);
                Display_Freq=atoi(another);
                }
        reenforcement_on=TRUE;
        display_on=FALSE;
        if ((player_A=='i') || (player_B=='i')) {
                printf("\nCreating ID3 decision tree\n\n");
                Initialise_induction();
                }
        Play_a_series(player_A,player_B,No_of_Games);
        printf("\nAnother game? ");
        gets(another);
        } while (strcmp(another,"n"));
}



