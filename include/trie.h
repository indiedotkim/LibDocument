/*
 * Copyright (c) 2015-2016 CODAMONO, Ontario, Canada
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at: http://mozilla.org/MPL/2.0/
 */

/**
 * @file trie.h
 *
 * @brief Functions and data structures for CRUD operations on tries (prefix trees).
 */

#ifndef __ldoc__trie__
#define __ldoc__trie__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initial size of a trie-node array.
 *
 * Used to allocate memory for search results.
 */
#define LDOC_TRIE_NDS_ARR_INIT 64
/**
 * @brief Allocation increment for trie-node arrays.
 *
 * Used to increase the memory in search results.
 */
#define LDOC_TRIE_NDS_ARR_INC 16

/**
 * @brief Reserved character that cannot appear in a trie.
 */
#define LDOC_TRIE_RES_CHR '^'
    
/**
 * @brief Type of pointer used to access descendant trie nodes.
 *
 * A trie can be composed of nodes with multiple kinds of pointer types.
 * Mixing of trie nodes with different pointer types can be useful when
 * optimizing for lookup or insertion performance and/or memory usage.
 */
typedef enum
{
    /**
     * Characters of the English alphabet ("a" to "z", lower case; " ", space).
     */
    EN_ALPH,
    /**
     * Characters of the English alphabet ("a" to "z", lower case; " ", space),
     * followed by numerical characters ("0" to "9").
     */
    EN_ALPHNUM,
    /**
     * ASCII characters, except `LDOC_TRIE_RES_CHR`.
     */
    ASCII,
    /**
     * UTF16, except `LDOC_TRIE_RES_CHR`.
     *
     * <span class="alert alert-danger" role="alert">Not implemented yet!</span>
     */
    UTF16,
    /**
     * UTF32, except `LDOC_TRIE_RES_CHR`.
     *
     * <span class="alert alert-danger" role="alert">Not implemented yet!</span>
     */
    UTF32
} ldoc_trie_ptr_t;

/**
 * @brief Trie-node type.
 *
 * A trie has a single unique root (`NDE_ROOT`) and its descendants are either
 * empty nodes (`NDE_EMPTY`) or annotated nodes (`NDE_ANNO`). Empty nodes can
 * occur on a path from the root to an annotated node; they basically denote that
 * a traversed prefix is not a prefix that was added to a trie. Annotated
 * nodes denote prefixes that have been explicitly added to a trie.
 */
typedef enum
{
    /**
     * Root node.
     */
    NDE_ROOT,
    /**
     * An empty node which is on a path to an annotated node.
     */
    NDE_EMPTY,
    /**
     * An annotated node.
     */
    NDE_ANNO
} ldoc_trie_alloc_t;
    
/**
 * @brief Trie node annotation.
 */
typedef struct ldoc_trie_anno_t
{
    /**
     * Category. Any 16-bit number for assigning simple categories. For
     * more complicated category schemes, it is necesary to move the information
     * into the payload.
     */
    uint16_t cat;
    /**
     * Payload. A pointer to an arbitrary data structure.
     */
    void* pld;
} ldoc_trie_anno_t;

/**
 * @brief Null pointer for trie nodes.
 */
extern ldoc_trie_anno_t LDOC_TRIE_NDE_NULL;

/**
 * @brief Null pointer for trie annotations.
 */
extern ldoc_trie_anno_t LDOC_TRIE_ANNO_NULL;
    
/**
 * @brief Character pointer types.
 */
typedef union
{
    /**
     * Arbitrary character pointer (void pointer).
     */
    void* c0;
    /**
     * C character pointer (most likely byte oriented; check your compiler).
     */
    char* c8;
    /**
     * 16-bit character pointer for use with UTF16.
     */
    uint16_t* c16;
    /**
     * 32-bit character pointer for use with UTF32.
     */
    uint32_t* c32;
} ldoc_trie_char_t;

/**
 * @brief Trie node.
 */
typedef struct ldoc_trie_nde_t
{
    /**
     * Type of the trie node, which determines the data structure used
     * for the character array (`chr`) and linking its descendants (`dscs`).
     */
    ldoc_trie_ptr_t tpe;
    /**
     * Number of elements in the character array (`chr`), if applicable.
     */
    uint16_t size;
    /**
     * Character array that determines the character of descendant (via `dscs`)
     * nodes.
     *
     * For types `EN_ALPH` and `EN_ALPHNUM` the pointer `c0` is set to NULL and
     * the length of the `dscs` pointer array is determined by the implicitly
     * represented characters as described by `ldoc_trie_ptr_t`.
     */
    ldoc_trie_char_t chr;
    struct ldoc_trie_nde_t** dscs;
    /**
     * Node type (root, empty, or annotation).
     */
    ldoc_trie_alloc_t alloc;
    /**
     * Annotation.
     */
    ldoc_trie_anno_t anno;
} ldoc_trie_nde_t;

/**
 * @brief Trie-node array.
 *
 * A trie-node array is filled up to the index denoted by `wptr` and its allocated
 * memory is sufficient to hold up to `max` entries. Indexes from `wptr` to `max`
 * are unassigned and should not be accessed.
 */
typedef struct ldoc_trie_nde_arr_t
{
    /**
     * Write pointer. Index at which the next node will be placed.
     */
    size_t wptr;
    /**
     * Maximum number of nodes that the array can hold without reallocating memory.
     */
    size_t max;
    /**
     * Actual array of trie nodes.
     */
    ldoc_trie_nde_t** nds;
} ldoc_trie_nde_arr_t;
    
/**
 * @brief A trie (prefix tree).
 */
typedef struct ldoc_trie_t
{
    /**
     * Minimum level (depth) at which a word becomes available, i.e. the length of the
     * shortest word in the trie.
     */
    uint16_t min;
    /**
     * Maximum level (depth) of the trie, i.e. the length of the longest word in the trie.
     */
    uint16_t max;
    /**
     * Root node.
     */
    ldoc_trie_nde_t* root;
} ldoc_trie_t;

#pragma mark - Trie Allocation/Deallocation

/**
 * @brief Creates a new trie object (including trie root node).
 *
 * @return A new trie object.
 */
ldoc_trie_t* ldoc_trie_new();
    
/**
 * @brief Frees the memory of a trie object.
 *
 * Releases the memory of the trie object, including all nodes, but it does
 * not release the memory of annotations.
 *
 * @param trie Trie whose memory is being released.
 */
void ldoc_trie_free(ldoc_trie_t* trie);

#pragma mark - Trie-Node Array CRUD
    
/**
 * @brief Creates a new trie-node array.
 *
 * The initial size of the array will be `LDOC_TRIE_NDS_ARR_INIT`.
 *
 * @return New trie-node array.
 */
ldoc_trie_nde_arr_t* ldoc_trie_nde_arr_new();
    
/**
 * @brief Frees a trie-node array.
 *
 * <strong>Note:</strong> This will node free the memory allocated for the individual nodes in the array.
 *
 * @param arr Trie-node array whose memory is being released.
 */
void ldoc_trie_nde_arr_free(ldoc_trie_nde_arr_t* arr);

/**
 * @brief Appends a node at the end of a trie-node array.
 *
 * If there is insufficient space available in the array, then the array's size will be
 * increased by `LDOC_TRIE_NDS_ARR_INC`.
 *
 * @param arr Array to which the node `nde` is appended.
 * @param nde Node that is being appended to array `arr`.
 */
void ldoc_trie_nde_arr_appnd(ldoc_trie_nde_arr_t* arr, ldoc_trie_nde_t* nde);
    
#pragma mark - Trie Updates/Modifications
    
/**
 * @brief Adds a string to a trie.
 *
 * @param trie Trie object to which the string `str` is being added.
 * @param str String that is being added to the trie object `trie`.
 * @param tpe If nodes need to be allocated to store `str` in `trie`, then they will be of type `tpe`.
 * @param anno Annotation that is being stored along with the string `str`.
 */
void ldoc_trie_add(ldoc_trie_t* trie, const char* str, ldoc_trie_ptr_t tpe, ldoc_trie_anno_t anno);

/**
 * @brief Remove a string (prefix) from a trie.
 *
 * @param trie Trie from which the string `str` is to be removed.
 * @param str String that is being removed from `trie`.
 * @return Trie node that was removed, or `LDOC_TRIE_NDE_NULL` if `str` was not in `trie`.
 */
ldoc_trie_nde_t* ldoc_trie_remove(ldoc_trie_t* trie, const char* str);

#pragma mark - Trie Search
    
/**
 * @brief Lookup a string in a trie.
 *
 * @param trie Trie object to search for the presence of `string`.
 * @param string String to search for in trie `trie`.
 * @param prefixes When true, then return the node in `trie` at which the search for `string` ended. This indicates whether `string` is a prefix of another string in `trie`, but `string` itself was not added to `trie`.
 * @return Trie node that matches `string` (see also `prefixes` description), or `LDOC_TRIE_NDE_NULL` otherwise.
 */
ldoc_trie_nde_t* ldoc_trie_lookup(ldoc_trie_t* trie, const char* string, bool prefixes);
    
#pragma mark - Summarization
    
/**
 * @brief Create a list of all strings in a trie.
 *
 * @param trie Trie whose strings should be listed.
 * @param sep Separator to use between strings in the list.
 * @return List of all strings in `trie` separated by `sep`.
 */
char* ldoc_trie_collect(ldoc_trie_t* trie, const char* sep);

#pragma mark - Debugging

/**
 * @brief Produce debugging output of a trie on `STDOUT`.
 *
 * @param trie Trie that should be output on `STDOUT`.
 */
void ldoc_trie_dmp(ldoc_trie_t* trie);
    
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* defined(__ldoc__trie__) */

