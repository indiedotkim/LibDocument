/*
 * Copyright (c) 2015 CODAMONO, Ontario, Canada
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ldoc__trie__
#define __ldoc__trie__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LDOC_TRIE_NDS_ARR_INIT 64
#define LDOC_TRIE_NDS_ARR_INC 16

/**
 *
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
    ASCII,
    UTF16,
    UTF32
} ldoc_trie_ptr_t;

/**
 *
 */
typedef enum
{
    NDE_ROOT,
    NDE_EMPTY,
    NDE_ANNO
} ldoc_trie_alloc_t;
    
/**
 *
 */
typedef struct ldoc_trie_anno_t
{
    /**
     * Category.
     */
    uint16_t cat;
    /**
     * Payload.
     */
    void* pld;
} ldoc_trie_anno_t;

extern ldoc_trie_anno_t LDOC_TRIE_ANNO_NULL;
    
typedef union
{
    void* c0;
    char* c8;
    uint16_t* c16;
    uint32_t* c32;
} ldoc_trie_char_t;

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
     *
     */
    ldoc_trie_alloc_t alloc;
    /**
     * Annotation.
     */
    ldoc_trie_anno_t anno;
} ldoc_trie_nde_t;

typedef struct ldoc_trie_nde_arr_t
{
    size_t wptr;
    size_t max;
    ldoc_trie_nde_t** nds;
} ldoc_trie_nde_arr_t;
    
/**
 * @brief TODO
 *
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
    
ldoc_trie_t* ldoc_trie_new();
void ldoc_trie_free(ldoc_trie_t* trie);

ldoc_trie_nde_arr_t* ldoc_trie_nde_arr_new();
void ldoc_trie_nde_arr_free(ldoc_trie_nde_arr_t* arr);

void ldoc_trie_nde_arr_appnd(ldoc_trie_nde_arr_t* arr, ldoc_trie_nde_t* nde);
    
void ldoc_trie_add(ldoc_trie_t* trie, const char* str, ldoc_trie_ptr_t tpe, ldoc_trie_anno_t anno);

ldoc_trie_nde_t* ldoc_trie_lookup(ldoc_trie_t* trie, const char* string, bool prefixes);
    
void ldoc_trie_dmp(ldoc_trie_t* trie);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* defined(__ldoc__trie__) */

