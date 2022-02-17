#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _treeNode_ {
	int colour;				//colour red = 0, black = 1
	int num;				//number of elements into the left subtree
	char * row;				//text into the treeNode
	struct _treeNode_ * father;
	struct _treeNode_ * leftChild;
	struct _treeNode_ * rightChild;
} treeNode;

typedef treeNode * textTree;


						/*-------------------------*/
						/* PROTOTIPI DELLE FUNZIONI*/
						/*-------------------------*/
						
						

textTree Cancella ( textTree tree, treeNode * canc );
textTree CreaNodo ( char * testo, textTree Tnull );
textTree eliminaNodoRosso ( textTree tree, textTree father, treeNode * elimina );
textTree LeftRotate ( textTree alto, textTree basso );
textTree pInserisci ( textTree tree, treeNode * pre, treeNode * neo, int num );
void print ( textTree rb, int min, int max, int nnodi, int * stampati );
textTree RightRotate ( textTree alto, textTree basso );
textTree Ripara ( textTree tree, textTree rb );
textTree Riparadx ( textTree tree, textTree nonno, textTree zio, textTree father, textTree insert );
textTree RiparaNerodx ( textTree tree, textTree father, textTree fratello, textTree sost );
textTree RiparaNerosx ( textTree tree, textTree father, textTree fratello, textTree sost );
textTree Riparasx ( textTree tree, textTree nonno, textTree zio, textTree father, textTree insert );
textTree trovak ( textTree tree, int x, int flap );
int VerificaFigli ( textTree rb );

							/*-----------------*/
							/* INZIO PROGRAMMA*/
							/*----------------*/

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
