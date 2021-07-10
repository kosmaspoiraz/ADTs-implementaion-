///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Vector μέσω ADT Map.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>

#include <stdio.h>

#include "ADTVector.h"
#include "ADTMap.h"

// Το αρχικό μέγεθος που δεσμεύουμε
#define VECTOR_MIN_CAPACITY 10

//Compare Pointers
int compare_keys(Pointer a, Pointer b)
{
	return *(Pointer *)a - *(Pointer *)b;
}

// Επιστρέφει έναν ακέραιο σε νέα μνήμη με τιμή value
int *create_int(int value)
{
	int *p = malloc(sizeof(int));
	*p = value;
	return p;
}

// Ένα VectorNode είναι pointer σε αυτό το struct.
struct vector_node
{
	Pointer key;
	Pointer value;
	DestroyFunc destroy_value;
};

static void node_destroy(Pointer value)
{
	VectorNode node = value;
	if (node == NULL)
		return;

	if (node->destroy_value != NULL)
	{
		if (node->value != NULL)
			node->destroy_value(node->value);
		// if (node->key != NULL)
		// 	node->destroy_value(node->key);
	}

	free(node);
}
// Ενα Vector είναι pointer σε αυτό το struct
struct vector
{
	Map map;				   //Tα δεδομένα, map απο VectorNodes
	int size;				   //Πόσα στοιχεία έχουμε προσθέσει
	int capacity;			   //Πόσο χώρο έχουμε δεσμεύσει(το μέγεθος του map). Πάντα capacity >=size
	DestroyFunc destroy_value; //Συνάρτηση που καταστρέφει ένα στοιχείο του vector
};

Vector vector_create(int size, DestroyFunc destroy_value)
{
	// Αρχικά το vector περιέχει size μη-αρχικοποιημένα στοιχεία, αλλά εμείς δεσμεύουμε xώρο για
	// τουλάχιστον VECTOR_MIN_CAPACITY για να αποφύγουμε τα πολλαπλά resizes
	int capacity = size < VECTOR_MIN_CAPACITY ? VECTOR_MIN_CAPACITY : size;

	// Δέσμευση μνήμης, για το struct και το map
	Vector vec = malloc(sizeof(*vec));

	// Είναι γενικά καλή πρακτική (ειδικά σε modules γενικής χρήσης), να ελέγχουμε αν η μνήμη δεσμεύτηκε με επιτυχία
	// LCOV_EXCL_START (αγνοούμε από το coverage report, είναι δύσκολο να τεστάρουμε αποτυχίες της malloc)
	if (vec == NULL)
	{
		free(vec); // free αν καταφέραμε να δεσμεύσουμε κάποιο από τα δύο.
		// free(map); // Αν όχι το free(NULL) απλά δεν κάνει τίποτα.
		return NULL;
	}
	// LCOV_EXCL_STOP

	vec->size = size;
	vec->capacity = capacity;
	vec->map = map_create(compare_keys, free, (DestroyFunc)node_destroy);
	vec->destroy_value = destroy_value;

	return vec;
}

int vector_size(Vector vec)
{
	return vec->size;
}

Pointer vector_get_at(Vector vec, int pos)
{
	assert(pos >= 0 && pos < vec->size); // LCOV_EXCL_LINE (αγνοούμε το branch από τα coverage reports, είναι δύσκολο να τεστάρουμε το false γιατί θα κρασάρει το test)
	VectorNode node = map_node_value(vec->map, map_find_node(vec->map, &pos));
	return node->value;
}

void vector_set_at(Vector vec, int pos, Pointer value)
{
	map_remove(vec->map, &pos); //Αφαιρούμε το VectorNode του map στη θέση pos
	Pointer ppos = create_int(pos);

	//φτιάχνουμε το νέο node με το καινούριο value στην ίδια θέση
	VectorNode node = malloc(sizeof(VectorNode));
	node->key = ppos;
	node->value = value;
	node->destroy_value = vec->destroy_value;

	//το βάζουμε στο map
	map_insert(vec->map, ppos, node); //Προσθέτουμε στην θέση pos του map στη θέση pos τη νέα τιμή value
}

void vector_insert_last(Vector vec, Pointer value)
{
	Pointer pos = create_int(vec->size);

	VectorNode node = malloc(sizeof(VectorNode));
	node->key = pos;
	node->value = value;
	node->destroy_value = vec->destroy_value;

	map_insert(vec->map, pos, node);
	vec->size++;
}

void vector_remove_last(Vector vec)
{
	// map_set_destroy_key(vec->map, vec->destroy_value);
	// map_set_destroy_value(vec->map, (DestroyFunc)node_destroy);
	map_remove(vec->map, &vec->size);
	vec->size--;
}

//Ψάχνω το map μέχρι να βρω σε ποία θέση είναι το value
//αν δεν υπάρχει στο map επιστρέφω NULL
Pointer vector_find(Vector vec, Pointer value, CompareFunc compare)
{
	for (int i = 0; i < vec->size; i++)
	{
		VectorNode node = map_find(vec->map, &i);
		if (compare(node->value, value) == 0) //βρέθηκε
		{
			return node->value;
		}
	}
	return NULL; //δεν βρέθηκε
}

DestroyFunc vector_set_destroy_value(Vector vec, DestroyFunc destroy_value)
{
	DestroyFunc old = vec->destroy_value;
	vec->destroy_value = destroy_value;
	return old;
}

void vector_destroy(Vector vec)
{
	map_destroy(vec->map); //καταστροφή map

	free(vec); //free vector
}

// Συναρτήσεις για διάσχιση μέσω node /////////////////////////////////////////////////////

VectorNode vector_first(Vector vec)
{
	if (vec->size == 0)
	{
		return VECTOR_BOF;
	}
	int first = 0;
	VectorNode node = map_find(vec->map, &first);
	if (node != NULL)
	{
		return node;
	}
	else
	{
		return NULL;
	}
}

VectorNode vector_last(Vector vec)
{
	if (vec->size == 0)
	{
		return VECTOR_EOF;
	}
	int last = vec->size - 1;
	VectorNode node = map_find(vec->map, &last);
	if (node != NULL)
	{
		return node;
	}
	else
	{
		return NULL;
	}
}

VectorNode vector_next(Vector vec, VectorNode node)
{
	//briskw se poia thesh einai to node me vector_find
	//kai epistrefw ton epomeno
	if (node != NULL)
	{
		int npos = *(int *)node->key + 1;
		VectorNode next = map_find(vec->map, &npos);
		return next;
	}
	else
	{
		return NULL;
	}
}

VectorNode vector_previous(Vector vec, VectorNode node)
{
	if (node != NULL)
	{
		int ppos = *(int *)node->key - 1;
		VectorNode previous = map_find(vec->map, &ppos);
		return previous;
	}
	else
	{
		return NULL;
	}
}

Pointer vector_node_value(Vector vec, VectorNode node)
{
	if (node != NULL)
	{
		return node->value;
	}
	else
	{
		return NULL;
	}
}

VectorNode vector_find_node(Vector vec, Pointer value, CompareFunc compare)
{
	for (int i = 0; i < vec->size; i++)
	{
		VectorNode node = map_find(vec->map, &i);
		if (compare(node->value, value) == 0) //βρέθηκε
		{
			return node;
		}
	}
	return NULL; //δεν βρέθηκε
}