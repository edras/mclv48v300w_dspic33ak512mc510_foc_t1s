/**
 * @file hashtable.h
 * @brief A simple implementation of a hash table in C.
 *
 * This header file defines the structures and functions for a generic hash table.
 * The hash table supports insertion, search, deletion, and iteration over elements.
 * The user must provide functions for hashing, comparing, and freeing elements.
 */

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @struct HASHTABLE_Node
 * @brief A node in the hash table.
 *
 * This structure represents a node in the hash table, which contains a pointer to the data
 * and a pointer to the next node in the bucket.
 */
typedef struct HASHTABLE_Node {
    void* data;          /**< Pointer to the data stored in the node */
    struct HASHTABLE_Node* next;   /**< Pointer to the next node in the bucket */
} HASHTABLE_Node;

/**
 * @struct HASHTABLE
 * @brief The hash table structure.
 *
 * This structure represents the hash table, which contains an array of buckets,
 * the size of the table, the number of elements, and function pointers for hashing,
 * comparing, and freeing elements.
 */
typedef struct {
    HASHTABLE_Node** buckets;       /* Array of pointers to the buckets */
    size_t size;                    /* Size of the hash table (number of buckets) */
    size_t count;                   /* Number of elements in the hash table */
    unsigned int (*hashFunction)(const void* data);    /* Function pointer for hashing elements */
    bool (*compareFunction)(const void* data1, const void* data2);  /* Function pointer for comparing elements */
    void (*freeFunction)(void* data); /* Function pointer for freeing elements */
} HASHTABLE;

/**
 * @brief Creates a new hash table.
 *
 * This function allocates and initializes a new hash table with the specified size and function pointers.
 *
 * @param size The initial size of the hash table (number of buckets).
 * @param hashFunction Function pointer for hashing elements.
 * @param compareFunction Function pointer for comparing elements.
 * @param freeFunction Function pointer for freeing elements.
 * @return A pointer to the newly created hash table, or NULL if memory allocation fails.
 */
HASHTABLE* HASHTABLE_create(size_t size, unsigned int (*hashFunction)(const void* data), 
        bool (*compareFunction)(const void* data1, const void* data2), void (*freeFunction)(void* data));

/**
 * @brief Inserts an element into the hash table.
 *
 * This function inserts the specified data into the hash table. If the load factor exceeds 75%,
 * the table is rehashed to a larger size.
 *
 * @param table A pointer to the hash table.
 * @param data A pointer to the data to be inserted.
 * @return true if the insert was successful, false otherwise.
 */
bool HASHTABLE_insert(HASHTABLE* table, void* data);

/**
 * @brief Searches for an element in the hash table.
 *
 * This function searches for the specified data in the hash table.
 *
 * @param table A pointer to the hash table.
 * @param data A pointer to the data to be searched for.
 * @return A pointer to the found data, or NULL if the data is not found.
 */
void* HASHTABLE_search(HASHTABLE* table, const void* data);

/**
 * @brief Searches for the next element in the hash table.
 *
 * This function searches the next element hash table regarding the
 * given one. If the given element is NULL, the next element is the 
 * first one.
 *
 * @param table A pointer to the hash table.
 * @param data A pointer to the data to be searched for.
 * @return A pointer to the found data, or NULL if the data is not found.
 */
void* HASHTABLE_next(HASHTABLE* table, const void* data);

/**
 * @brief Deletes an element from the hash table.
 *
 * This function deletes the specified data from the hash table.
 *
 * @param table A pointer to the hash table.
 * @param data A pointer to the data to be deleted.
 */
void HASHTABLE_delete(HASHTABLE* table, const void* data);

/**
 * @brief Frees the hash table.
 *
 * This function frees all memory associated with the hash table, including all elements and the table itself.
 *
 * @param table A pointer to the hash table to be freed.
 */
void HASHTABLE_free(HASHTABLE* table);

/**
 * @brief Function pointer type for iterating over elements.
 *
 * This type defines a function pointer for a function that takes a pointer to data and returns void.
 */
typedef void (*HASHTABLE_IterateFunction)(void* data);

/**
 * @brief Iterates over all elements in the hash table.
 *
 * This function applies the specified function to each element in the hash table.
 *
 * @param table A pointer to the hash table.
 * @param func A function pointer to the function to be applied to each element.
 */
void HASHTABLE_iterate(HASHTABLE* table, HASHTABLE_IterateFunction func);

/**
 * @brief Gets an element by index.
 *
 * This function returns the element at the specified index in the hash table.
 * The index is based on the order of insertion.
 *
 * @param table A pointer to the hash table.
 * @param idx The index of the element to be retrieved.
 * @return A pointer to the data at the specified index, or NULL if the index is out of range.
 */
void* HASHTABLE_getItemByIndex(HASHTABLE* table, size_t idx);

/**
 * @brief Gets the total number of elements in the hash table.
 *
 * This function returns the total number of elements currently stored in the hash table.
 *
 * @param table A pointer to the hash table.
 * @return The total number of elements in the hash table.
 */
size_t HASHTABLE_getCount(HASHTABLE* table);

#endif // HASHTABLE_H
