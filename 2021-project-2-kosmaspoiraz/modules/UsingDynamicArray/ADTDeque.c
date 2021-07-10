///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT deque μέσω Dynamic Array.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>

#include "ADTDeque.h"

// Το αρχικό μέγεθος που δεσμεύουμε
#define DEQUE_MIN_CAPACITY 10

// Ένα dequeNode είναι pointer σε αυτό το struct. (το struct περιέχει μόνο ένα στοιχείο, οπότε θα μπροούσαμε και να το αποφύγουμε, αλλά κάνει τον κώδικα απλούστερο)
struct deque_node
{
    Pointer value; // Η τιμή του κόμβου.
};

// Ενα deque είναι pointer σε αυτό το struct
struct deque
{
    DequeNode array; // Τα δεδομένα, πίνακας από struct deque_node
    int size;        // Πόσα στοιχεία έχουμε προσθέσει
    int capacity;    // Πόσο χώρο έχουμε δεσμεύσει (το μέγεθος του array). Πάντα capacity >= size, αλλά μπορεί να έχουμε
    int new_first;
    DestroyFunc destroy_value; // Συνάρτηση που καταστρέφει ένα στοιχείο του deque.
};

Deque deque_create(int size, DestroyFunc destroy_value)
{
    // Δημιουργία του struct
    Deque deq = malloc(sizeof(*deq));

    deq->size = size;
    deq->destroy_value = destroy_value;

    // Δέσμευση μνήμης για τον πίνακα. Αρχικά το deque περιέχει size
    // μη-αρχικοποιημένα στοιχεία, αλλά εμείς δεσμεύουμε xώρο για τουλάχιστον
    // deque_MIN_CAPACITY για να αποφύγουμε τα πολλαπλά resizes.
    //
    deq->capacity = size < DEQUE_MIN_CAPACITY ? DEQUE_MIN_CAPACITY : size;
    deq->array = calloc(deq->capacity, sizeof(*deq->array)); // αρχικοποίηση σε 0 (NULL)
    deq->new_first = deq->capacity - 1;

    return deq;
}

int deque_size(Deque deq)
{
    return deq->size;
}

Pointer deque_get_at(Deque deq, int pos)
{
    assert(pos >= 0 && pos < deq->size); // LCOV_EXCL_LINE (αγνοούμε το branch από τα coverage reports, είναι δύσκολο να τεστάρουμε το false γιατί θα κρασάρει το test)

    if (deq->new_first == deq->capacity - 1)
        return deq->array[pos].value;
    else
        return deq->array[deq->new_first + 1 + pos].value;
}

void deque_set_at(Deque deq, int pos, Pointer value)
{
    assert(pos >= 0 && pos < deq->size); // LCOV_EXCL_LINE

    // Αν υπάρχει συνάρτηση destroy_value, την καλούμε για το στοιχείο που αντικαθίσταται
    int index = 0;
    if (deq->new_first == deq->capacity - 1)
        index = pos;
    else
        index = deq->new_first + 1 + pos;

    if (value != deq->array[index].value && deq->destroy_value != NULL)
        deq->destroy_value(deq->array[index].value);

    deq->array[index].value = value;
}

void deque_insert_first(Deque deq, Pointer value)
{
    // Μεγαλώνουμε τον πίνακα (αν χρειαστεί), ώστε να χωράει τουλάχιστον size στοιχεία
    // Διπλασιάζουμε κάθε φορά το capacity (σημαντικό για την πολυπλοκότητα!)
    if (deq->capacity == deq->size)
    {
        // Προσοχή: δεν πρέπει να κάνουμε free τον παλιό pointer, το κάνει η realloc
        deq->capacity *= 2;
        DequeNode new_array = calloc(deq->capacity, sizeof(*deq->array)); // αρχικοποίηση σε 0 (NULL)

        // Αντιγράφω τα στοιχεία που έχουν εισαχθεί στην αρχή του deque
        int j = 0;
        for (int i = deq->new_first + 1; i > deq->size; i++)
        {
            new_array[j] = deq->array[i];
            j++;
        }

        // Αντιγράφω τα στοιχεία του υπόλοιπου deque
        for (int i = 0; i < deq->size; i++)
        {
            new_array[j] = deq->array[i];
            j++;
        }

        deq->array = new_array;

        free(new_array);
        deq->new_first = deq->capacity - 1;
    }

    // Μεγαλώνουμε τον πίνακα και προσθέτουμε το στοιχείο
    deq->array[deq->new_first].value = value;
    deq->new_first--;
    deq->size++;
}

void deque_insert_last(Deque deq, Pointer value)
{
    // Μεγαλώνουμε τον πίνακα (αν χρειαστεί), ώστε να χωράει τουλάχιστον size στοιχεία
    // Διπλασιάζουμε κάθε φορά το capacity (σημαντικό για την πολυπλοκότητα!)
    if (deq->capacity == deq->size)
    {
        // Προσοχή: δεν πρέπει να κάνουμε free τον παλιό pointer, το κάνει η realloc
        deq->capacity *= 2;
        deq->array = realloc(deq->array, deq->capacity * sizeof(*deq->array));
        deq->new_first = deq->capacity - 1;
    }

    // Μεγαλώνουμε τον πίνακα και προσθέτουμε το στοιχείο
    deq->array[deq->size].value = value;
    deq->size++;
}

void deque_remove_first(Deque deq)
{
    // Αν υπάρχει συνάρτηση destroy_value, την καλούμε για το στοιχείο που αφαιρείται
    if (deq->new_first == deq->capacity - 1)
    {
        if (deq->destroy_value != NULL)
            deq->destroy_value(deq->array[0].value);
    }
    else
    {
        if (deq->destroy_value != NULL)
            deq->destroy_value(deq->array[deq->new_first].value);
        deq->new_first++;
    }

    // Αφαιρούμε στοιχείο οπότε ο πίνακας μικραίνει
    deq->size--;

    // Μικραίνουμε τον πίνακα αν χρειαστεί, ώστε να μην υπάρχει υπερβολική σπατάλη χώρου.
    // Για την πολυπλοκότητα είναι σημαντικό να μειώνουμε το μέγεθος στο μισό, και μόνο
    // αν το capacity είναι τετραπλάσιο του size (δηλαδή το 75% του πίνακα είναι άδειος).
    //
    if (deq->capacity > deq->size * 4 && deq->capacity > 2 * DEQUE_MIN_CAPACITY)
    {
        deq->capacity /= 2;
        deq->array = realloc(deq->array, deq->capacity * sizeof(*deq->array));
    }
}

void deque_remove_last(Deque deq)
{
    assert(deq->size != 0); // LCOV_EXCL_LINE

    // Αν υπάρχει συνάρτηση destroy_value, την καλούμε για το στοιχείο που αφαιρείται
    if (deq->destroy_value != NULL)
        deq->destroy_value(deq->array[deq->size - 1].value);

    // Αφαιρούμε στοιχείο οπότε ο πίνακας μικραίνει
    deq->size--;

    // Μικραίνουμε τον πίνακα αν χρειαστεί, ώστε να μην υπάρχει υπερβολική σπατάλη χώρου.
    // Για την πολυπλοκότητα είναι σημαντικό να μειώνουμε το μέγεθος στο μισό, και μόνο
    // αν το capacity είναι τετραπλάσιο του size (δηλαδή το 75% του πίνακα είναι άδειος).
    //
    if (deq->capacity > deq->size * 4 && deq->capacity > 2 * DEQUE_MIN_CAPACITY)
    {
        deq->capacity /= 2;
        deq->array = realloc(deq->array, deq->capacity * sizeof(*deq->array));
        deq->new_first = deq->capacity - 1;
    }
}

Pointer deque_find(Deque deq, Pointer value, CompareFunc compare)
{
    // Διάσχιση του deque
    for (int i = 0; i < deq->size; i++)
        if (compare(deq->array[i].value, value) == 0)
            return deq->array[i].value; // βρέθηκε

    return NULL; // δεν υπάρχει
}

DestroyFunc deque_set_destroy_value(Deque deq, DestroyFunc destroy_value)
{
    DestroyFunc old = deq->destroy_value;
    deq->destroy_value = destroy_value;
    return old;
}

void deque_destroy(Deque deq)
{
    // Αν υπάρχει συνάρτηση destroy_value, την καλούμε για όλα τα στοιχεία
    if (deq->destroy_value != NULL)
        for (int i = 0; i < deq->size; i++)
            deq->destroy_value(deq->array[i].value);

    // Πρέπει να κάνουμε free τόσο τον πίνακα όσο και το struct!
    free(deq->array);
    free(deq); // τελευταίο το deq!
}

// Συναρτήσεις για διάσχιση μέσω node /////////////////////////////////////////////////////

DequeNode deque_first(Deque deq)
{
    if (deq->size == 0)
        return DEQUE_BOF;
    else if (deq->new_first == deq->capacity - 1)
        return &deq->array[0];
    else
        return &deq->array[deq->new_first];
}

DequeNode deque_last(Deque deq)
{
    if (deq->size == 0)
        return DEQUE_EOF;
    else
        return &deq->array[deq->size - 1];
}

DequeNode deque_next(Deque deq, DequeNode node)
{
    if (node == &deq->array[deq->size - 1])
        return DEQUE_EOF;
    else
        return node + 1;
}

DequeNode deque_previous(Deque deq, DequeNode node)
{
    if (node == &deq->array[0])
        return DEQUE_EOF;
    else
        return node - 1;
}

Pointer deque_node_value(Deque deq, DequeNode node)
{
    return node->value;
}

DequeNode deque_find_node(Deque deq, Pointer value, CompareFunc compare)
{
    // Διάσχιση του deque
    for (int i = 0; i < deq->size; i++)
        if (compare(deq->array[i].value, value) == 0)
            return &deq->array[i]; // βρέθηκε

    return DEQUE_EOF; // δεν υπάρχει
}