///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Deque μέσω ADT Map.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>

#include <stdio.h>

#include "ADTDeque.h"
#include "ADTMap.h"

// Το αρχικό μέγεθος που δεσμεύουμε
#define DEQUE_MIN_CAPACITY 100

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

// Ένα DequeNode είναι pointer σε αυτό το struct.
struct deque_node
{
    Pointer key;
    Pointer value;
    DestroyFunc destroy_value;
};

static void node_destroy(Pointer value)
{
    DequeNode node = value;
    if (node == NULL)
        return;

    if (node->destroy_value != NULL)
    {
        if (node->value != NULL)
            node->destroy_value(node->value);
    }

    free(node);
}
// Ενα Deque είναι pointer σε αυτό το struct
struct deque
{
    Map map;      //Tα δεδομένα, map απο DequeNodes
    int size;     //Πόσα στοιχεία έχουμε προσθέσει
    int capacity; //Πόσο χώρο έχουμε δεσμεύσει(το μέγεθος του map). Πάντα capacity >=size
    int counter;
    DestroyFunc destroy_value; //Συνάρτηση που καταστρέφει ένα στοιχείο του Deque
};

Deque deque_create(int size, DestroyFunc destroy_value)
{
    // Αρχικά το Deque περιέχει size μη-αρχικοποιημένα στοιχεία, αλλά εμείς δεσμεύουμε xώρο για
    // τουλάχιστον Deque_MIN_CAPACITY για να αποφύγουμε τα πολλαπλά resizes
    int capacity = size < DEQUE_MIN_CAPACITY ? DEQUE_MIN_CAPACITY : size;

    // Δέσμευση μνήμης, για το struct και το map
    Deque deq = malloc(sizeof(*deq));

    // Είναι γενικά καλή πρακτική (ειδικά σε modules γενικής χρήσης), να ελέγχουμε αν η μνήμη δεσμεύτηκε με επιτυχία
    // LCOV_EXCL_START (αγνοούμε από το coverage report, είναι δύσκολο να τεστάρουμε αποτυχίες της malloc)
    if (deq == NULL)
    {
        free(deq); // free αν καταφέραμε να δεσμεύσουμε κάποιο από τα δύο.
        // free(map); // Αν όχι το free(NULL) απλά δεν κάνει τίποτα.
        return NULL;
    }
    // LCOV_EXCL_STOP

    deq->size = size;
    deq->capacity = capacity;
    deq->map = map_create(compare_keys, free, (DestroyFunc)node_destroy);
    deq->destroy_value = destroy_value;

    deq->counter = 0;

    return deq;
}

int deque_size(Deque deq)
{
    return deq->size;
}

Pointer deque_get_at(Deque deq, int pos)
{
    assert(pos >= 0 && pos < deq->size); // LCOV_EXCL_LINE (αγνοούμε το branch από τα coverage reports, είναι δύσκολο να τεστάρουμε το false γιατί θα κρασάρει το test)
    int npos = 0;
    npos = pos - deq->counter;
    DequeNode node = map_find(deq->map, &npos);
    return node->value;
}

void deque_set_at(Deque deq, int pos, Pointer value)
{
    pos = pos - deq->counter;
    map_remove(deq->map, &pos); //Αφαιρούμε το DequeNode του map στη θέση pos
    Pointer ppos = create_int(pos);

    //φτιάχνουμε το νέο node με το καινούριο value στην ίδια θέση
    DequeNode node = malloc(sizeof(DequeNode));
    node->key = ppos;
    node->value = value;
    node->destroy_value = deq->destroy_value;

    //το βάζουμε στο map
    map_insert(deq->map, ppos, node); //Προσθέτουμε στην θέση pos του map στη θέση pos τη νέα τιμή value
}

void deque_insert_first(Deque deq, Pointer value)
{
    int new_first = 0;
    if (deq->size != 0)
    {
        new_first = -deq->counter - 1;
        deq->counter++;
    }

    Pointer pnew_first = create_int(new_first);

    //φτιάχνω το νέο node για εισαγωγή
    DequeNode node = malloc(sizeof(DequeNode));
    node->destroy_value = deq->destroy_value;
    node->key = pnew_first;
    node->value = value;

    //το βάζω στο map
    map_insert(deq->map, pnew_first, node);
    deq->size++; //αυξάνω το size
}

void deque_remove_first(Deque deq)
{
    map_remove(deq->map, map_node_key(deq->map, map_first(deq->map)));
    deq->size--;
}

void deque_insert_last(Deque deq, Pointer value)
{

    int real_last = deq->size - deq->counter;
    // int new_last = real_last + 1;
    Pointer pnew_last = create_int(real_last);

    DequeNode node = malloc(sizeof(DequeNode));
    node->key = pnew_last;
    node->value = value;
    node->destroy_value = deq->destroy_value;

    map_insert(deq->map, pnew_last, node);
    deq->size++;
}

void deque_remove_last(Deque deq)
{
    int real_last = deq->size - deq->counter;
    map_remove(deq->map, &real_last);
    deq->size--;
}

//Ψάχνω το map μέχρι να βρω σε ποία θέση είναι το value
//αν δεν υπάρχει στο map επιστρέφω NULL
Pointer deque_find(Deque deq, Pointer value, CompareFunc compare)
{
    for (DequeNode node = deque_first(deq); node != DEQUE_EOF; node = deque_next(deq, node))
    {
        if (compare(node->value, value) == 0) //βρέθηκε
        {
            return node->value;
        }
    }
    return NULL; //δεν βρέθηκε
}

DestroyFunc deque_set_destroy_value(Deque deq, DestroyFunc destroy_value)
{
    DestroyFunc old = deq->destroy_value;
    deq->destroy_value = destroy_value;
    return old;
}

void deque_destroy(Deque deq)
{
    map_destroy(deq->map); //καταστροφή mapσ

    free(deq); //free Deque
}

// Συναρτήσεις για διάσχιση μέσω node /////////////////////////////////////////////////////

DequeNode deque_first(Deque deq)
{
    if (deq->size == 0)
    {
        return DEQUE_BOF;
    }
    DequeNode node = map_node_value(deq->map, map_first(deq->map));
    if (node != NULL)
    {
        return node;
    }
    else
    {
        return NULL;
    }
}

DequeNode deque_last(Deque deq)
{
    if (deq->size == 0)
    {
        return DEQUE_EOF;
    }
    int last = deq->size - 1;
    DequeNode node = map_find(deq->map, &last);
    if (node != NULL)
    {
        return node;
    }
    else
    {
        return NULL;
    }
}

DequeNode deque_next(Deque deq, DequeNode node)
{
    //briskw se poia thesh einai to node me Deque_find
    //kai epistrefw ton epomeno
    if (node != NULL)
    {
        int npos = *(int *)node->key + 1;
        DequeNode next = map_find(deq->map, &npos);
        return next;
    }
    else
    {
        return NULL;
    }
}

DequeNode deque_previous(Deque deq, DequeNode node)
{
    if (node != NULL)
    {
        int ppos = *(int *)node->key - 1;
        DequeNode previous = map_find(deq->map, &ppos);
        return previous;
    }
    else
    {
        return NULL;
    }
}

Pointer deque_node_value(Deque deq, DequeNode node)
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

DequeNode deque_find_node(Deque deq, Pointer value, CompareFunc compare)
{
    for (DequeNode node = deque_first(deq); node != DEQUE_EOF; node = deque_next(deq, node))
    {
        if (compare(node->value, value) == 0) //βρέθηκε
        {
            return node;
        }
    }
    return NULL; //δεν βρέθηκε
}