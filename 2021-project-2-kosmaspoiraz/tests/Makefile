# Κάνοντας compile το <foo>_test.c με μια υλοποίηση <foo>.c του
# συγκεκριμένου τύπου, παράγουμε ένα tets για την υλοποίηση αυτή.

# Υλοποιήσεις μέσω dynamic array: ADTVector
#
UsingDynamicArray_ADTVector_test_OBJS = ADTVector_test.o $(MODULES)/UsingDynamicArray/ADTVector.o

# Υλοποιήσεις μέσω ADTMap: ADTVector
#
# Για να λειτουργήσουν χρειάζονται και μια υλοποίηση του ADTMap. Χρησιμοποιούμε την UsingADTSet/ADTMap.o,
# η οποία με τη σειρά της χρειάζεται μια υλοποίηση του ADTSet (χρησιμοποιούμε την UsingAVL/ADTSet.o).
#
UsingADTMap_ADTVector_test_OBJS = ADTVector_test.o $(MODULES)/UsingADTMap/ADTVector.o $(MODULES)/UsingADTSet/ADTMap.o $(MODULES)/UsingAVL/ADTSet.o

UsingRealTimeDynamicArray_ADTVector_test_OBJS = ADTVector_test.o $(MODULES)/UsingRealTimeDynamicArray/ADTVector.o 

# Υλοποιήσεις μέσω ADTMap: ADTDeque
#
# Για να λειτουργήσουν χρειάζονται και μια υλοποίηση του ADTMap. Χρησιμοποιούμε την UsingADTSet/ADTMap.o,
# η οποία με τη σειρά της χρειάζεται μια υλοποίηση του ADTSet (χρησιμοποιούμε την UsingAVL/ADTSet.o).
#
UsingADTMap_ADTDeque_test_OBJS = ADTDeque_test.o $(MODULES)/UsingADTMap/ADTDeque.o $(MODULES)/UsingADTSet/ADTMap.o $(MODULES)/UsingAVL/ADTSet.o
UsingDynamicArray_ADTDeque_test_OBJS = ADTDeque_test.o $(MODULES)/UsingDynamicArray/ADTDeque.o

# Υλοποιήσεις μέσω BinarySearchTree: ADTSet
#
UsingBinarySearchTree_ADTSet_test_OBJS = ADTSet_test.o $(MODULES)/UsingBinarySearchTree/ADTSet.o

# Ο βασικός κορμός του Makefile
include ../common.mk