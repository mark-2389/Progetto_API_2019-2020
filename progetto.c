#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define M 1030				//buffer's limit

typedef struct _treeNode_ {
	int colour;				//colour red = 0, black = 1
	int num;				//number of elements into the left subtree
	char * row;				//text into the treeNode
	struct _treeNode_ * father;
	struct _treeNode_ * leftChild;
	struct _treeNode_ * rightChild;
} treeNode;

typedef treeNode * textTree;

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

textTree Cancella ( textTree tree, treeNode * canc );
photoList CercaProx ( photoList * backup, undoStack * ustack, undoStack * rstack, int unodi, int rnodi, int * mosse );
textTree CreaNodo ( char * testo, textTree Tnull );
photoList dPush ( photoList dlista, undoStack direct, int sum );
textTree eliminaNodoRosso ( textTree tree, textTree father, treeNode * elimina );
textTree LeftRotate ( textTree alto, textTree basso );
textTree pInserisci ( textTree tree, treeNode * pre, treeNode * neo, int t );
void print ( textTree rb, int min, int max, int nnodi, int * stampati );
undoStack Push ( undoStack stack, char op, int min, int max, int nnodi );
int RaccogliIstr ( int * ind1, int * ind2, int * mosse, int * unodi, int * rnodi );
textTree Rigenera ( textTree tree, textTree Tnull, textTree * max, undoStack stack );
textTree RightRotate ( textTree alto, textTree basso );
textTree Ripara ( textTree tree, textTree rb );
textTree Riparadx ( textTree tree, textTree nonno, textTree zio, textTree father, textTree insert );
textTree RiparaNerodx ( textTree tree, textTree father, textTree fratello, textTree sost );
textTree RiparaNerosx ( textTree tree, textTree father, textTree fratello, textTree sost );
textTree Riparasx ( textTree tree, textTree nonno, textTree zio, textTree father, textTree insert );
textTree trovak ( textTree tree, int x, int flap );
int VerificaFigli ( textTree rb );
void VisitaInOrdine ( textTree rb, int min, int max, int nnodi, undoStack * ustack, int * visitati, int tot );

							/*-----------------*/
							/* INZIO PROGRAMMA*/
							/*----------------*/

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

void print ( textTree rb, int min, int max, int nnodi, int * stampati ){
	int count;

	if ( rb == NULL || rb->num == -1 || *stampati == 0 )		//� stata stampata tutta la struttura o tutti i nodi necessari
		return;

    count = rb->num + 1;
	if ( count < min )						//il piu piccolo treeNode da stampare nell'tree rb � nel sottoalbero destro
		return print ( rb->rightChild, min-count, max-count, nnodi-count, stampati );

	if ( count > min ){						//il piu piccolo treeNode da stampare nell'tree rb � nel sottoalbero sinistro

		if ( rb->num < max )				//vanno stampati tutti i nodi photoNode sottoalbero sinistro
			print ( rb->leftChild, min, rb->num, rb->num, stampati );
		else								//vanno stampati tutti i nodi fino a max
			print ( rb->leftChild, min, max, rb->num, stampati );

		if ( *stampati == 0 || nnodi == 1 )	//tutti nodi sono stati stampati
			return;
	}

	if ( rb->row != NULL )
		fputs ( rb->row, stdout );			//stampo la row photoNode treeNode corrente
	*stampati = *stampati-1;				//diminuisco il numero di elementi da stampare
	if ( *stampati == 0 || nnodi == 1 )		//sono stati stampati tutti gli elementi
		return;
	return print ( rb->rightChild, 1, max-count, nnodi-count, stampati );	//bisogna stampare tutti i nodi da 1 fino a max-count photoNode sottoalbero destro
}

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

textTree CreaNodo ( char * testo, textTree Tnull ){

	treeNode * elem = (treeNode *) malloc ( sizeof ( treeNode ) );		//creo nuovo nodo
	elem->colour = 0;
	elem->row = testo;
	elem->father = Tnull;										//riferimenti di father
	elem->leftChild = Tnull;										//riferimento dei figli a Tnull
	elem->rightChild = Tnull;
	return elem;
}

int VerificaFigli ( textTree rb ){

	if ( ( rb->leftChild )->num == -1 && ( rb->rightChild )->num == -1 )	//il nodo non ha figli
		return 0;
	if ( ( rb->leftChild )->num != -1 && ( rb->rightChild )->num == -1 )	//il nodo ha solo il figlio sinistro
		return 1;
	if ( ( rb->leftChild )->num == -1 && ( rb->rightChild )->num != -1 )	//il nodo ha solo il figlio destro
		return 2;
	return 3;
}

textTree LeftRotate ( textTree alto, textTree basso ){	//ROTAZIONE IN SENSO ANTIORARIO

	alto->rightChild = basso->leftChild;				//aggancio sottoalbero sinistro
	if ( (basso->leftChild)->num != -1 )
		(basso->leftChild)->father = alto;			//sistemo father photoNode sottoalbero
	basso->leftChild = alto;							//ruoto in senso antiorario
	if ( ( alto->father )->num != -1 ){
		if ( alto == (alto->father)->leftChild )		//sistemo il figlio photoNode father
			(alto->father)->leftChild = basso;
		else
			(alto->father)->rightChild = basso;
	}
	basso->father = alto->father;						//sistemo i riferimenti al father
	alto->father = basso;
	basso->num = basso->num + alto->num + 1;

	return basso;
}

textTree RightRotate ( textTree alto, textTree basso ){	//ROTAZIONE IN SENSO ORARIO

	alto->leftChild = basso->rightChild;				//aggancio sottoalbero destro
	if ( (basso->rightChild)->num != -1 )
		(basso->rightChild)->father = alto;			//sistemo riferimento al father photoNode sottoalbero
	basso->rightChild = alto;							//ruoto in senso orario

	if ( ( alto->father )->num != -1 ){				//sistemo il figlio photoNode father
		if ( alto == (alto->father)->leftChild)
			(alto->father)->leftChild = basso;
		else
			(alto->father)->rightChild = basso;
	}
	basso->father = alto->father;						//sistemo i riferimenti al father
	alto->father = basso;
	alto->num = alto->num-basso->num-1;

	return basso;
}

textTree Riparasx ( textTree tree, textTree nonno, textTree zio, textTree father, textTree insert ){		//il father di insert � figlio sinistro photoNode nonno

	if ( zio->num == -1 ){										//lo zio � una NULL LEAF

		if ( father->rightChild == insert ){						//rendo i due nodi figli sinistri
			nonno->leftChild = LeftRotate ( father, insert );
			insert = father;
			father = father->father;
		}

		if ( ( nonno->father )->num == -1 )					//controllo se il nonno � la radice o meno
			tree = RightRotate ( nonno, father );				//se il nonno non � la radice la rotazione � locale
		else if ( (nonno->father)->leftChild == nonno )			//ruoto per riequilibrare
			(nonno->father)->leftChild = RightRotate ( nonno, father );
		else
			(nonno->father)->rightChild = RightRotate ( nonno, father );

		father->colour = 1;
		nonno->colour = 0;
		return tree;
	}

	if ( zio->colour == 0 ){								//lo zio � di colour red
		father->colour = 1;									//inverto i colori photoNode nonno e dei suoi due figli
		zio->colour = 1;									//invoco la riparazione sul nonno per eventuali rotture di invariante
		nonno->colour = 0;
		tree = Ripara ( tree, nonno );
		return tree;
	}

	if ( father->rightChild == insert ){						//rendo il nodo inserito figlio sinistro
		nonno->leftChild = LeftRotate ( father, insert );
		insert = father;
		father = father->father;
	}
	father->colour = 1;										//inverto i colori photoNode father e photoNode nonno
	nonno->colour = 0;

	if ( ( nonno->father )->num == -1 )						//controllo se il nonno � la radice o meno
		return RightRotate ( nonno, father );				//se il nonno non � la radice la rotazione � locale
	else if ( (nonno->father)->leftChild == nonno )			//ruoto per riequilibrare
		(nonno->father)->leftChild = RightRotate ( nonno, father );
	else
		(nonno->father)->rightChild = RightRotate ( nonno, father );

	return tree;
}

textTree Riparadx ( textTree tree, textTree nonno, textTree zio, textTree father, textTree insert ){		//il father di insert � figlio destro photoNode nonno

	if ( zio->num == -1 ){										//lo zio � una NULL LEAF

		if ( father->leftChild == insert ){						//rendo i due nodi figli destri
			nonno->rightChild = RightRotate ( father, insert );
			insert = father;
			father = father->father;
		}

		if ( ( nonno->father )->num == -1 )						//controllo se il nonno � la radice o meno
			tree = LeftRotate ( nonno, father );					//se il nonno non � la radice la rotazione � locale
		else if ( (nonno->father)->leftChild == nonno )			//ruoto per riequilibrare
			(nonno->father)->leftChild = LeftRotate ( nonno, father );
		else
			(nonno->father)->rightChild = LeftRotate ( nonno, father );

		father->colour = 1;					//inverto i colori per mantenere invariante
		nonno->colour = 0;
		return tree;
	}

	if ( zio->colour == 0 ){				//lo zio � red
		father->colour = 1;					//inverto i colori photoNode nonno e dei suoi due figli
		zio->colour = 1;					//invoco la riparazione sul nonno per trovare eventuali rotture di invarianti
		nonno->colour = 0;
		tree = Ripara ( tree, nonno );
		return tree;
	}

	if ( father->leftChild == insert ){		//rendo il nodo inserito figlio destro
		nonno->rightChild = RightRotate ( father, insert );
		insert = father;
		father = father->father;
	}
	father->colour = 1;										//inverto i colori photoNode father e photoNode nonno
	nonno->colour = 0;
	if ( ( nonno->father )->num == -1 )
		return LeftRotate ( nonno, father );
	else if ( (nonno->father)->leftChild == nonno )			// poi ruoto per riequilibrare
		(nonno->father)->leftChild = LeftRotate ( nonno, father );
	else
		(nonno->father)->rightChild = LeftRotate ( nonno, father );

	return tree;
}

textTree Ripara ( textTree tree, textTree rb ){
	textTree zio, nonno, papa = rb->father;

	if ( papa->num == -1 ){						//l'elemento � la radice
		rb->colour = 1;
		return tree;
	}

	if ( papa->colour == 0 ){					//poich� il papa ha colour red, sicuramente esiste il nonno

		nonno = papa->father;					//determino il nonno, necessario per riparare l'tree
		if ( nonno->leftChild == papa  ){
			zio = nonno->rightChild;				//il father di rb � figlio sinistro photoNode nonno
			return Riparasx ( tree, nonno, zio, papa, rb );
		}
		else{
			zio = nonno->leftChild;				//il father di rb � figlio destro photoNode nonno
			return Riparadx ( tree, nonno, zio, papa, rb );
		}
	}
	return tree;
}

textTree pInserisci ( textTree tree, treeNode * pre, treeNode * neo, int num ){		//inserimento particolare, solo per u e r
	textTree curr;
	neo->num = num;

	if ( tree == NULL ){					//devo inserire un nodo in una struttra vuota
		neo->colour = 1;
		return neo;
	}

	if ( pre == NULL ){						//il nodo predecessore non � presente
		pre = tree;							//allora inserisco neo nel sottoalbero sinistro della radice
		curr = pre->leftChild;				//in qualit� di minimo
	}
	else {
		curr = pre->rightChild;				//il nodo predecessore � presente
		if ( curr->num == -1 ){				//se predecessore non ha il figlio destro
			pre->rightChild = neo;			//inserisco neo come figlio destro
			neo->father = pre;
			return Ripara ( tree, neo );
		}
	}

	while ( curr->num != -1 ){				//scorro il sottoalbero opportuno fino a trovare Tnull
		pre = curr;
		curr = curr->leftChild;
	}

	neo->father = pre;
	pre->leftChild = neo;					//inserisco neo come minimo photoNode sottoalbero destro photoNode predecessore
	return Ripara ( tree, neo );
}

textTree trovak ( textTree tree, int x, int flap ){				//trova il k-esimo elemento della struttura
	int k;

	if ( x <= 0 || tree == NULL )							//l'elemento cercato non � presente
		return NULL;
	k = tree->num+1;										//determino la posizione photoNode treeNode corrente
	if ( k == x )											//� il treeNode cercato
		return tree;
	if ( k > x ){											//il treeNode cercato � nel sottoalbero sinistro
		tree->num += flap;
		return trovak ( tree->leftChild, x, flap );
	}
	return trovak ( tree->rightChild, x-k, flap );			//il treeNode cercato � nel sottoalbero destro

}

textTree eliminaNodoRosso ( textTree tree, textTree father, treeNode * elimina ){
	textTree temp;

	if ( (elimina->leftChild)->num == -1 && (elimina->rightChild)->num == -1 ){			//elimino un treeNode che non ha figli

		if ( father->leftChild == elimina )									//sistemo il riferimento photoNode father
			father->leftChild = elimina->leftChild;
		else
			father->rightChild = elimina->rightChild;

		return tree;
	}
	if ( (elimina->leftChild)->num != -1 && (elimina->rightChild)->num == -1 ){			//elimino un treeNode che ha il solo figlio sinistro

		if ( father->leftChild == elimina )
			father->leftChild = elimina->leftChild;							//aggancio il sottoalbero opportuno
		else
			father->rightChild = elimina->leftChild;

		( elimina->leftChild )->father = father;								//sistemo il riferimento al father
		return tree;
	}
	if ( (elimina->leftChild)->num == -1 && (elimina->rightChild)->num != -1 ){			//elimino un treeNode che ha il solo figlio destro

		if ( father->leftChild == elimina )
			father->leftChild = elimina->rightChild;							//aggancio il sottoalbero opportuno
		else
			father->rightChild = elimina->rightChild;

		( elimina->rightChild )->father = father;								//sistemo il riferimento al father
		return tree;
	}

	temp = elimina->rightChild;												//elimino un treeNode che ha due figli
	while ( (temp->leftChild)->num != -1 ){									//trovo il successore
		temp->num--;
		temp = temp->leftChild;
	}

	elimina->row = temp->row;												//copio numero di row e testo
	return Cancella ( tree, temp );								//e poi elimino il successore
}

textTree RiparaNerosx ( textTree tree, textTree father, textTree fratello, textTree sost ){				//sost � figlio sinistro

	if ( sost->colour == 1 )					//sost era red, eredita blackness photoNode father
		return tree;

	if ( fratello->colour == 0 ){				//fratello � red
		fratello->colour = 1;					//inverto father e fratello e mi riconduco ai casi successivi
		father->colour = 0;
		if ( tree == father )
			tree = LeftRotate ( father, fratello );
		else if ( ( father->father )->leftChild == father )
			( father->father )->leftChild = LeftRotate ( father, fratello );
		else
			( father->father )->rightChild = LeftRotate ( father, fratello );
		fratello = father->rightChild;
	}

	if ( fratello->num == -1 || ( ( fratello->leftChild )->colour >= 1 &&  ( fratello->rightChild )->colour >= 1 ) ){	//fratello black
		sost->colour = 1;																//nipoti entrambi neri
		fratello->colour = 0;
		father->colour += 1;
		if ( ( father->father )->num == -1 ){
			father->colour = 1;
			return tree;
		}
		else if ( ( father->father )->leftChild == father )
			return RiparaNerosx ( tree, father->father, ( father->father )->rightChild, father );
		else
			return RiparaNerodx ( tree, father->father, ( father->father )->leftChild, father );
	}

	if ( ( fratello->rightChild )->colour >= 1 ){										//fratello black
		( fratello->leftChild )->colour = 1;											//nipote destro black
		fratello->colour = 0;														//nipote sinistro red
		father->rightChild = RightRotate ( fratello, fratello->leftChild );
		fratello = father->rightChild;
	}

	sost->colour = 1;										//fratello black
	fratello->colour = father->colour;						//nipote destro red
	father->colour = 1;
	( fratello->rightChild )->colour = 1;
	if ( ( father->father )->num == -1 )
		tree = LeftRotate ( father, fratello );
	else if ( ( father->father )->leftChild == father )
		( father->father )->leftChild = LeftRotate ( father, fratello );
	else
		( father->father )->rightChild = LeftRotate ( father, fratello );

	return tree;
}

textTree RiparaNerodx ( textTree tree, textTree father, textTree fratello, textTree sost ){		//sost � figlio destro

	if ( sost->colour == 1 )					//sost era red, eredita blackness photoNode father
		return tree;

	if ( fratello->colour == 0 ){				//fratello � red
		fratello->colour = 1;					//inverto father e fratello e mi riconduco ai casi successivi
		father->colour = 0;
		if ( tree == father )
			tree = RightRotate ( father, fratello );
		else if ( ( father->father )->leftChild == father )
			( father->father )->leftChild = RightRotate ( father, fratello );
		else
			( father->father )->rightChild = RightRotate ( father, fratello );
		fratello = father->leftChild;
	}

	if ( fratello->num == -1 || ( ( fratello->leftChild )->colour >= 1 &&  ( fratello->rightChild )->colour >= 1  ) ){	//fratello black
		sost->colour = 1;																//nipoti entrambi neri o nulli
		fratello->colour = 0;
		father->colour += 1;
		if ( ( father->father )->num == -1 ){
			father->colour = 1;
			return tree;
		}
		else if ( ( father->father )->leftChild == father )
			return RiparaNerosx ( tree, father->father, ( father->father )->rightChild, father );
		else
			return RiparaNerodx ( tree, father->father, ( father->father )->leftChild, father );
	}

	if ( ( fratello->leftChild )->colour >= 1 ){										//fratello black
		( fratello->rightChild )->colour = 1;											//nipote sinistro black
		fratello->colour = 0;														//nipote destro red
		father->leftChild = LeftRotate ( fratello, fratello->rightChild );
		fratello = father->leftChild;
	}

	sost->colour = 1;										//fratello black
	fratello->colour = father->colour;						//nipote sinistro red
	father->colour = 1;
	( fratello->leftChild )->colour = 1;

	if ( ( father->father )->num == -1 )
		tree = RightRotate ( father, fratello );
	else if ( ( father->father )->leftChild == father )
		( father->father )->leftChild = RightRotate ( father, fratello );
	else
		( father->father )->rightChild = RightRotate ( father, fratello );

	return tree;
}

textTree Cancella ( textTree tree, treeNode * canc ){
	textTree temp, succ;
	int sign;

	if ( canc->colour == 0 )
        return eliminaNodoRosso ( tree, canc->father, canc );		//se elimino un treeNode red nessun invariante da riparare

	temp = canc->father;
	sign = VerificaFigli( canc );

	if ( temp->num != -1){

		if ( temp->leftChild == canc ){					//canc � figlio sinistro photoNode father

			if ( sign == 0 )						//non ha nessun figlio
				temp->leftChild = canc->leftChild;
			else if ( sign == 1 )						//ha solo il figlio sinistro
				temp->leftChild = canc->leftChild;
			else if ( sign == 2 )						//ha solo il figlio dx
				temp->leftChild = canc->rightChild;
			else{										//ha entrambi i figli
				succ = canc->rightChild;					//determino il successore
				while ( ( succ->leftChild )->num != -1 ){
					succ->num--;
					succ = succ->leftChild;
				}
				canc->row = succ->row;							//copio row e testo
				return Cancella ( tree, succ );			//cancello il successore
			}

			if ( ( temp->leftChild )->num != -1 )
				( temp->leftChild )->father = temp;
			( temp->leftChild )->colour += 1;
			return RiparaNerosx ( tree, temp, temp->rightChild, temp->leftChild );
		}
		else{											//canc � figlio destro

			if ( sign == 0 )							//non ha nessun figlio
				temp->rightChild = canc->leftChild;
			else if ( sign == 1 )						//ha solo il figlio sinistro
				temp->rightChild = canc->leftChild;
			else if ( sign == 2 )						//ha solo il figlio dx
				temp->rightChild = canc->rightChild;
			else{										//ha entrambi i figli
				succ = canc->rightChild;					//determino il successore
				while ( ( succ->leftChild )->num != -1 ){
					succ->num--;
					succ = succ->leftChild;
				}
				canc->row = succ->row;							//copio row e testo
				return Cancella ( tree, succ );			//cancello il successore
			}

			if ( ( temp->rightChild )->num != -1 )
				( temp->rightChild )->father = temp;
			( temp->rightChild )->colour += 1;
			return RiparaNerodx ( tree, temp, temp->leftChild, temp->rightChild );
		}
	}

	if ( sign == 0 )				//elimino la radice
		return NULL;

	if ( sign == 3 ){				//la radice ha entrambi i figli
		succ = canc->rightChild;		//determino il successore
		while ( ( succ->leftChild )->num != -1 ){
			succ->num--;
			succ = succ->leftChild;
		}
        canc->row = succ->row;	//copio il successore
		return Cancella ( tree, succ );		//e poi lo elimino
	}

	if ( sign == 1 ){				//la radice ha solo il figlio sinistro
		tree->num--;
		tree = canc->leftChild;		//aggancio il sottoalbero sinistro
	}
	else							//la radice ha il solo figlio destro
		tree = canc->rightChild;		//aggancio il sottoalbero destro

	tree->father = canc->father;		//sistemo il riferimento al father
	tree->colour = 1;				//sistemo il colour della radice
	return tree;
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
