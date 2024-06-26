#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "redblacktree.h"
#define M 1030				//buffer's limit

typedef struct _uNode_ {
	char operation;			//operation type
	int min;				//min number (ind1)
	int max;				//max number (ind2)
	int nodip;				//numero di nodi appena prima la mossa
	char ** oldTexts;		//vector of pointers to old texts
	char ** newTexts;		//vector of pointers to nex texts
	textTree * subtree;		//vettore di puntatori al treeNode modificato
	struct _uNode_ * rsucc;
	struct _uNode_ * unext;
} uNode;

typedef struct _photoNode_ {
	int pos;				//position into the undoStack or redoStack
	char type;
	uNode * direct;			//direct access to the state to be restored
	struct _photoNode_ * rdel;
	struct _photoNode_ * udel;
} photoNode;

typedef photoNode * photoList;
typedef uNode * undoStack;		//this stack keeps track of all the modifying operation to the main structure (i.e. textTree)
typedef uNode * redoStack;		//this stack keeps track of all the operations that con be re-executed after an undo

						/*-------------------------*/
						/* PROTOTIPI DELLE FUNZIONI*/
						/*-------------------------*/

photoList CercaProx ( photoList * backup, undoStack * ustack, undoStack * rstack, int unodi, int rnodi, int * mosse );
photoList dPush ( photoList dlista, undoStack direct, int sum );
undoStack Push ( undoStack stack, char op, int min, int max, int nnodi );
int RaccogliIstr ( int * ind1, int * ind2, int * mosse, int * unodi, int * rnodi );
textTree Rigenera ( textTree tree, textTree Tnull, textTree * max, undoStack stack );
void VisitaInOrdine ( textTree rb, int min, int max, int nnodi, undoStack * ustack, int * visitati, int tot );

							/*-----------------*/
							/* INZIO PROGRAMMA*/
							/*----------------*/
							
photoList CercaProx ( photoList * backup, undoStack * ustack, undoStack * rstack, int unodi, int rnodi, int * mosse ){
	int a, b, tot=unodi+rnodi+1;
	photoList ptr, temp = *backup;

	if ( *mosse == 0 || temp == NULL )
		return NULL;

	if ( *mosse > 0 ) {
		while ( temp != NULL && temp->type != 'r' ){
			temp = temp->rdel;
			if ( temp == NULL )
				return temp;
		}
		ptr = temp->rdel;
		while ( temp->pos > rnodi ) {
			temp->pos = tot - temp->pos;
			temp->type = 'u';
			if ( ptr == NULL ) {
				*backup = temp;
				*rstack = temp->direct;
				*ustack = (*rstack)->unext;
				*mosse = unodi - temp->pos + 1;
				return temp;
			}
			temp = ptr;
			ptr = ptr->rdel;
		}

		*backup = temp;
		b = rnodi - temp->pos;
		if ( temp->udel == NULL ) {
			if ( *mosse <= b )
				return NULL;
			else {
				*rstack = temp->direct;
				*ustack = (*rstack)->unext;
				*mosse = -b;
				return temp;
			}
		}
		else {
			a = unodi - temp->udel->pos + 1;
			if ( a <= b ) {
				*rstack = temp->udel->direct;
				*ustack = (*rstack)->unext;
				*mosse = a;
				return temp->udel;
			}
			else {
				*rstack = temp->direct;
				*ustack = (*rstack)->unext;
				*mosse = -b;
				return temp;
			}
		}
	}

	while ( temp != NULL && temp->type != 'u' ){
		temp = temp->udel;
		if ( temp == NULL )
			return temp;
	}
	ptr = temp->udel;
	while ( temp->pos > unodi ){
		temp->pos = tot - temp->pos;
		temp->type = 'r';
		if ( ptr == NULL ){
			*backup = temp;
			*rstack = temp->direct;
			*ustack = (*rstack)->unext;
			*mosse = temp->pos - rnodi;
			return temp;
		}
		temp = ptr;
		ptr = ptr->udel;
	}

	*backup = temp;
	b = unodi - temp->pos + 1;
	if ( temp->rdel == NULL ){
		if ( -( *mosse ) <= b )
			return NULL;
		else {
			*rstack = temp->direct;
			*ustack = (*rstack)->unext;
			*mosse = b;
			return temp;
		}
	}
	else {
		a = rnodi - temp->rdel->pos;

		if ( a <= b ){
			*rstack = temp->rdel->direct;
			*ustack = (*rstack)->unext;
			*mosse = -a;
			return temp->rdel;
		}
		else {
			*rstack = temp->direct;
			*ustack = (*rstack)->unext;
            *mosse = b;
			return temp;
		}
	}
}

photoList dPush ( photoList dlista, undoStack direct, int num ) {

	photoList temp = ( photoNode * ) malloc ( sizeof ( photoNode ) );
	temp->direct = direct;
	temp->pos = num;
    temp->type = 'u';

	if ( dlista != NULL )
		dlista->rdel = temp;

	temp->rdel = NULL;
	temp->udel = dlista;
	return temp;
}

undoStack Push ( undoStack stack, char op, int min, int max, int nnodi ){

	uNode * temp = ( uNode * ) calloc ( 1, sizeof ( uNode ) );			//alloco il nuovo uNode
	temp->operation = op;												//type of operation
	temp->min = min;													//indirizzo minimo (ind1)
	temp->max = max;													//indirizzo massimo (ind2)
	temp->nodip = nnodi;												//numero di nodi prima dell'esecuzione della mossa

	if ( op == 'c' )
		temp->newTexts = ( char ** ) malloc ( ( max-min+1 ) * sizeof ( char * ) );
	if ( stack != NULL )
		stack->rsucc = temp;

	temp->unext = stack;
	return temp;
}

int RaccogliIstr ( int * ind1, int * ind2, int * mosse, int * unodi, int * rnodi ){
	char istruzione[20000], *t;
	int dist=0, i=0;

    t = fgets ( istruzione, 20000, stdin );			//prelevo la row da stdin (comprende '\n')
	dist = strlen(istruzione)-2;					//determino la posizione della lettera

	if ( istruzione[dist] == 'q' )					//se q, termino il programma
		return 0;

	if ( istruzione[dist] == 'u'){					//se u o r determino con atoi il numero di mosse
		if ( *unodi == 0 )
			return 4;
		i = atoi ( istruzione );

		if ( i > *unodi )
            i = *unodi;

        *mosse = *mosse - i;
        *unodi = *unodi - i;
        *rnodi = *rnodi + i;
		return 4;
	}
	if ( istruzione[dist] == 'r'){
		if ( *rnodi == 0 )
			return 5;
		i = atoi ( istruzione );
		if ( i > *rnodi )
            i = *rnodi;

		*mosse = *mosse + i;
        *unodi = *unodi + i;
        *rnodi = *rnodi - i;
		return 5;
	}

	*ind1 = atoi ( istruzione );					//se d, c, p determino i due indirizzi
	while ( istruzione[i] != ',')
		i++;
	*ind2 = atoi ( istruzione+i+1 );
	if ( istruzione[dist] == 'c' )					//e poi restituisco i valori corrispondenti la scelta
		return 1;
	if ( istruzione[dist] == 'd' )
		return 2;
	if ( istruzione[dist] == 'p' )
		return 3;
	return -1;
}

textTree Rigenera ( textTree tree, textTree Tnull, textTree * max, undoStack stack ) {
	int i;

	for ( i=0; i<stack->nodip; i++ ) {
		stack->subtree[i]->colour = 0;
		stack->subtree[i]->row = stack->oldTexts[i];
		stack->subtree[i]->father = Tnull;
		stack->subtree[i]->leftChild = Tnull;
		stack->subtree[i]->rightChild = Tnull;
		tree = pInserisci ( tree, *max, stack->subtree[i], 0 );
		*max = stack->subtree[i];
	}

	return tree;
}

void VisitaInOrdine ( textTree rb, int min, int max, int nnodi, undoStack * ustack, int * visitati, int tot ){
	int count, a;
	textTree temp;

	if ( rb == NULL || rb->num == -1 || *visitati == tot )
		return;

	count = rb->num + 1;
	if ( count < min )
		return VisitaInOrdine ( rb->rightChild, min-count, max-count, nnodi-count, ustack, visitati, tot );

	if ( count > min ) {

		if ( rb->num < max ) {
			VisitaInOrdine ( rb->leftChild, min, rb->num, rb->num, ustack, visitati, tot );
        }
		else {
			VisitaInOrdine ( rb->leftChild, min, max, rb->num, ustack, visitati, tot );
        }
		if ( *visitati == tot || nnodi == 1 )
			return;
	}

	if ( (*ustack)->operation == 'c' ) {
		(*ustack)->oldTexts[*visitati] = rb->row;
		(*ustack)->subtree[*visitati] = rb;
		a = *visitati + 1;
		if ( a >= (*ustack)->min && a <= (*ustack)->max )
			rb->row = (*ustack)->newTexts[a-(*ustack)->min];
	}
	else if ( (*ustack)->operation == 'd' ) {
		(*ustack)->oldTexts[*visitati] = rb->row;
		(*ustack)->subtree[*visitati] = rb;
	}
	else if ( (*ustack)->operation == 'b' )
		rb->row = (*ustack)->oldTexts[(*ustack)->min+*visitati-1];


	*visitati += 1;
	if ( *visitati == tot || nnodi == 1 )
		return;

	return VisitaInOrdine ( rb->rightChild, 1, max-count, nnodi-count, ustack, visitati, tot );
}

int main (){
	textTree testo = NULL, max = NULL, Tnull, temp;
	char *c, buff[M];
	int flag=0, choice=0, start=0, finish=0, mosse=0,  nnodi=0, unodi=0, rnodi=0, i=0, t;
	undoStack ustack = NULL, rstack = NULL;
	photoList photo=NULL, prova;

	Tnull = ( treeNode * ) calloc ( 1, sizeof ( treeNode ) );
	Tnull->colour = 1;
	Tnull->num = -1;

	do{
		if ( flag == 0 )
			choice = RaccogliIstr ( &start, &finish, &mosse, &unodi, &rnodi );

		if ( choice == 0 )
			break;
		else if ( choice == 1 ){											//change

           if ( rstack != NULL ){
				rstack = NULL;
                if ( photo != NULL ) {
                    if ( photo->type == 'r' && photo->udel == NULL ) {
                        photo = NULL;
                    }
                    else if ( photo->type == 'r' && photo->udel != NULL ) {
                    	photo = photo->udel;
                    	photo->rdel = NULL;
					}
                    else
                        photo->rdel = NULL;
                }
            }
			ustack = Push ( ustack, 'c', start, finish, nnodi );
			unodi++;

			t = finish-start+1;
			i = 0;
			while ( i < t ){
				c = fgets ( buff, M, stdin );
				ustack->newTexts[i] = ( char * ) malloc ( strlen ( buff ) + 1 );
				strcpy ( ustack->newTexts[i], buff );
				if ( start > nnodi ){
					temp = CreaNodo ( ustack->newTexts[i], Tnull );
					testo = pInserisci ( testo, max, temp, 0 );
					max = temp;
					start++;
				}
				i++;
			}
			c = fgets ( buff, M, stdin );

			if ( start <= finish ) {
				photo = dPush ( photo, ustack, unodi );
				i = 0;
				ustack->oldTexts = ( char ** ) malloc ( nnodi * sizeof ( char * ) );
				ustack->subtree = ( textTree * ) malloc ( nnodi * sizeof ( textTree ) );
				VisitaInOrdine ( testo, 1, nnodi, nnodi, &ustack, &i, nnodi );
				while ( nnodi < finish ) {
					temp = CreaNodo ( ustack->newTexts[nnodi-start+1], Tnull );
					testo = pInserisci ( testo, max, temp, 0 );
					max = temp;
					nnodi++;
				}
			}
			else
				nnodi = nnodi + t;

			rnodi = 0;
			flag = 0;
		}
		else if ( choice == 2 ){											//delete

            if ( rstack != NULL ){
				rstack = NULL;
                if ( photo != NULL ) {
                    if ( photo->type == 'r' && photo->udel == NULL ) {
                        photo = NULL;
                    }
                    else if ( photo->type == 'r' && photo->udel != NULL ) {
                    	photo = photo->udel;
                    	photo->rdel = NULL;
					}
                    else
                        photo->rdel = NULL;
                }
            }

			ustack = Push ( ustack, 'd', start, finish, nnodi );
			unodi++;
			photo = dPush ( photo, ustack, unodi );
			ustack->oldTexts =  ( char ** ) malloc ( nnodi * sizeof ( char * ) );
			ustack->subtree = ( textTree * ) malloc ( nnodi * sizeof ( textTree ) );
			i = 0;
            VisitaInOrdine ( testo, 1, nnodi, nnodi, &ustack, &i, nnodi );

			if ( start == 0 && finish == 0 || start > nnodi || nnodi == 0 );
			else {

                if ( start == 0 ) {
                	start++;
                	ustack->min = 1;
				}

				testo = NULL;
				max = NULL;
				nnodi = 0;

				if ( start != 1 || finish < ustack->nodip ) {
					i = 1;
					t = 0;
					while ( i <= ustack->nodip ) {

						if ( i < start || i > finish ) {
							ustack->subtree[t]->colour = 0;
							ustack->subtree[t]->father = Tnull;
							ustack->subtree[t]->leftChild = Tnull;
							ustack->subtree[t]->rightChild = Tnull;
							testo = pInserisci ( testo, max, ustack->subtree[t], 0 );
							max = ustack->subtree[t];
							nnodi++;
						}
						else {
							i = finish;
							t = i - 1;
						}
						i++;
						t++;
					}
	            }
            }

			rnodi = 0;
			flag = 0;
		}
		else if ( choice == 3 ){									//print
			if ( start > nnodi || start == 0 ) 						//gli indirizzi da stampare sono oltre quelli presenti
				for ( ; start <= finish; start ++ )
					fputs ( ".\n", stdout );
			else{
				i = finish-start+1;									//devo stampare i nodi ed eventualmente dei '.'
				print ( testo, start, finish, nnodi, &i );
				if ( finish > nnodi )
					for ( i=nnodi; i<finish; i++ )
						fputs ( ".\n", stdout );;
			}
			flag = 0;
		}
		else {			//undo o redo

			while ( choice > 3 ){

				if ( flag == 0 && ( ( choice == 4 && ustack == NULL ) || ( choice == 5 && rstack == NULL ) ) )
					mosse = 0;
				if ( flag == 0 && ( ( choice == 4 && ustack != NULL ) || ( choice == 5 && rstack != NULL ) ) )
					flag = 1;
				choice = RaccogliIstr ( &start, &finish, &mosse, &unodi, &rnodi );
			}
			flag = 1;
            prova = CercaProx ( &photo, &ustack, &rstack, unodi, rnodi, &mosse );
            if ( prova != NULL ) {
            	testo = NULL;
            	max = NULL;
            	nnodi = prova->direct->nodip;
            	testo = Rigenera ( testo, Tnull, &max, prova->direct );
			}

			while ( mosse < 0 && ustack != NULL ) {								//undo

                if ( ustack->operation == 'c' ) {								//undo di una change
                	t = ustack->max;

                	if ( t > ustack->nodip )
                		for ( ; t>ustack->nodip; t-- )
							testo = Cancella ( testo, trovak ( testo, t, -1 ) );

					nnodi = ustack->nodip;
					i = 0;
					if ( ustack->min <= ustack->nodip ) {
						ustack->operation = 'b';
						VisitaInOrdine ( testo, ustack->min, t, nnodi, &ustack, &i, t-ustack->min+1 );
						ustack->operation = 'c';
					}
                }
                else{												//undo di una delete

                	if ( ustack->min != 0 && ustack->min <= ustack->nodip ) {
                		testo = NULL;
                		max = NULL;
                		nnodi = ustack->nodip;
                		testo = Rigenera ( testo, Tnull, &max, ustack );
					}
                }
				rstack = ustack;								//aggiorno la undoStack e redoStack
				ustack = ustack->unext;
				mosse++;
			}

			while ( mosse > 0 && rstack!=NULL ){		//redo

				if ( rstack->operation == 'c' ){		//devo rieseguire una change

					if ( rstack->min <= rstack->nodip ) {
						i = 0;
						if ( rstack->max > rstack->nodip )
							VisitaInOrdine ( testo, rstack->min, rstack->nodip, rstack->nodip, &rstack, &i, rstack->nodip-rstack->min+1 );
						else
							VisitaInOrdine ( testo, rstack->min, rstack->max, rstack->nodip, &rstack, &i, rstack->max-rstack->min+1 );
					}
					if ( rstack->max > rstack->nodip ) {

						if ( rstack->min <= rstack->nodip ) {
                            i = rstack->nodip - rstack->min + 1;
                        }
                        else
                            i = 0;

                        max = trovak ( testo, rstack->nodip, 0 );
                        for ( ; i<=rstack->max-rstack->min; i++ ) {
                            temp = CreaNodo ( rstack->newTexts[i], Tnull );
                            testo = pInserisci ( testo, max, temp, 0 );
                            max = temp;
                            nnodi++;
                        }
					}
				}
				else{															//devo rieseguire una delete

					if ( ( rstack->min == 0 && rstack->max == 0 ) || rstack->min > nnodi );
					else {
						if ( ( rstack->max >= nnodi && rstack->min == 1 ) || nnodi == 0 ) {
							testo = NULL;
							max = NULL;
							nnodi = 0;
						}
						else {

							testo = NULL;
							max = NULL;
							nnodi = 0;

							if ( rstack->min != 1 || rstack->max < rstack->nodip ) {
								i = 1;
								t = 0;
								while ( i <= rstack->nodip ) {

									if ( i < rstack->min || i > rstack->max ) {
										rstack->subtree[t]->colour = 0;
										rstack->subtree[t]->father = Tnull;
										rstack->subtree[t]->leftChild = Tnull;
										rstack->subtree[t]->rightChild = Tnull;
										testo = pInserisci ( testo, max, rstack->subtree[t], 0 );
										max = rstack->subtree[t];
										nnodi++;
									}
									else {
										i = rstack->max;
										t = i - 1;
									}
									i++;
									t++;
								}
				            }
						}
					}
				}
				ustack = rstack;						//aggiorno la undoStack e la redoStack
                rstack = rstack->rsucc;
                mosse--;
            }

            max = trovak ( testo, nnodi, 0 );
            prova = NULL;
            mosse = 0;
		}

	} while ( choice > 0 );			//se choice == 0 quit

	return 0;
}
