#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

/* abstract functions to be implemented by the final application */
/**
 * Calculates the hash number of "data" object.
 */
unsigned int (*hashFunction)(const void* data);
/**
 * Compare two objects and return true if equal, false otherwise.
 */
bool (*compareFunction)(const void* data1, const void* data2);
/**
 * Free memory used by "data" object.
 */
void (*freeFunction)(void* data);

/* private functions prototypes */
void HASHTABLE_rehash(HASHTABLE* table);

/* public functions implementations */

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
        bool (*compareFunction)(const void* data1, const void* data2), void (*freeFunction)(void* data)) 
{
    HASHTABLE* table = (HASHTABLE*)malloc(sizeof(HASHTABLE));
    if (!table) {
        return NULL;
    }

    table->buckets = (HASHTABLE_Node**)calloc(size, sizeof(HASHTABLE_Node*));
    if (!table->buckets) {
        free(table);
        return NULL;
    }

    table->size = size;
    table->count = 0;
    table->hashFunction = hashFunction;
    table->compareFunction = compareFunction;
    table->freeFunction = freeFunction;

    return table;
}

/**
 * @brief Inserts an element into the hash table.
 *
 * This function inserts the specified data into the hash table. If the load factor exceeds 75%,
 * the table is rehashed to a larger size.
 *
 * @param table A pointer to the hash table.
 * @param data A pointer to the data to be inserted.
 */
bool HASHTABLE_insert(HASHTABLE* table, void* data) 
{
    unsigned int hash = table->hashFunction(data);
    size_t index = hash % table->size;

    HASHTABLE_Node* newNode = (HASHTABLE_Node*)malloc(sizeof(HASHTABLE_Node));
    if (!newNode) 
    {
        return false;
    }
    newNode->data = data;
    newNode->next = table->buckets[index];
    table->buckets[index] = newNode;
    table->count++;

    // Rehash if load factor exceeds 75%
    if ((table->count * 100) / table->size > 75) 
    {
        HASHTABLE_rehash(table);
    }
    return true;
}

/**
 * @brief Searches for an element in the hash table.
 *
 * This function searches for the specified data in the hash table.
 *
 * @param table A pointer to the hash table.
 * @param data A pointer to the data to be searched for.
 * @return A pointer to the found data, or NULL if the data is not found.
 */
void* HASHTABLE_search(HASHTABLE* table, const void* data) 
{
    unsigned int hash = table->hashFunction(data);
    size_t index = hash % table->size;

    HASHTABLE_Node* node = table->buckets[index];
    while (node) 
    {
        if (table->compareFunction(node->data, data)) 
        {
            return node->data;
        }
        node = node->next;
    }
    return NULL;
}

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
void* HASHTABLE_next(HASHTABLE* table, const void* data) 
{
    size_t index = 0;
    bool found = true;
    
    if(table == NULL) return NULL;
    
    if(data)
    {
        unsigned int hash = table->hashFunction(data);
        index = hash % table->size;
        found = false;
    }

    for (size_t i = index; i < table->size; i++) 
    {
        HASHTABLE_Node* node = table->buckets[i];
        while (node) 
        {
            if(found)
            {
                return node->data; 
            }
            if (table->compareFunction(node->data, data)) 
            {
                found = true;
            }
            node = node->next;
        }
    }
    return NULL;
}

/**
 * @brief Deletes an element from the hash table.
 *
 * This function deletes the specified data from the hash table.
 *
 * @param table A pointer to the hash table.
 * @param data A pointer to the data to be deleted.
 */
void HASHTABLE_delete(HASHTABLE* table, const void* data) 
{
    unsigned int hash = table->hashFunction(data);
    size_t index = hash % table->size;

    HASHTABLE_Node* node = table->buckets[index];
    HASHTABLE_Node* prev = NULL;
    while (node) 
    {
        if (table->compareFunction(node->data, data)) 
        {
            if (prev) 
            {
                prev->next = node->next;
            } 
            else 
            {
                table->buckets[index] = node->next;
            }
            table->freeFunction(node->data);
            free(node);
            table->count--;
            return;
        }
        prev = node;
        node = node->next;
    }
}

/**
 * @brief Frees the hash table.
 *
 * This function frees all memory associated with the hash table, including all elements and the table itself.
 *
 * @param table A pointer to the hash table to be freed.
 */
void HASHTABLE_free(HASHTABLE* table) 
{
    for (size_t i = 0; i < table->size; i++) 
    {
        HASHTABLE_Node* node = table->buckets[i];
        while (node) 
        {
            HASHTABLE_Node* next = node->next;
            table->freeFunction(node->data);
            free(node);
            node = next;
        }
    }
    free(table->buckets);
    free(table);
}

/**
 * @brief Iterates over all elements in the hash table.
 *
 * This function applies the specified function to each element in the hash table.
 *
 * @param table A pointer to the hash table.
 * @param func A function pointer to the function to be applied to each element.
 */
void HASHTABLE_iterate(HASHTABLE* table, HASHTABLE_IterateFunction func) 
{
    for (size_t i = 0; i < table->size; i++) 
    {
        HASHTABLE_Node* node = table->buckets[i];
        while (node) 
        {
            func(node->data);
            node = node->next;
        }
    }
}

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
void* HASHTABLE_getItemByIndex(HASHTABLE* table, size_t idx) 
{
    if (idx >= table->count) 
    {
        return NULL;
    }

    size_t currentIndex = 0;
    for (size_t i = 0; i < table->size; i++) 
    {
        HASHTABLE_Node* node = table->buckets[i];
        while (node) 
        {
            if (currentIndex == idx) 
            {
                return node->data;
            }
            currentIndex++;
            node = node->next;
        }
    }
    return NULL;
}

/**
 * @brief Gets the total number of elements in the hash table.
 *
 * This function returns the total number of elements currently stored in the hash table.
 *
 * @param table A pointer to the hash table.
 * @return The total number of elements in the hash table.
 */
size_t HASHTABLE_getCount(HASHTABLE* table) 
{
    return table->count;
}

/* private functions implementations */

void HASHTABLE_rehash(HASHTABLE* table)
{
    size_t newSize = table->size * 2;
    HASHTABLE_Node** newBuckets = (HASHTABLE_Node**)calloc(newSize, sizeof(HASHTABLE_Node*));
    if (!newBuckets) 
    {
        return;
    }

    for (size_t i = 0; i < table->size; i++) 
    {
        HASHTABLE_Node* node = table->buckets[i];
        while (node) 
        {
            HASHTABLE_Node* next = node->next;
            unsigned int hash = table->hashFunction(node->data);
            size_t newIndex = hash % newSize;
            node->next = newBuckets[newIndex];
            newBuckets[newIndex] = node;
            node = next;
        }
    }

    free(table->buckets);
    table->buckets = newBuckets;
    table->size = newSize;
}
