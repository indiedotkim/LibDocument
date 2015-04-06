/*
 *
 */

#include "trie.h"

ldoc_trie_anno_t LDOC_TRIE_ANNO_NULL = { 0, NULL };

/**
 *
 *
 * Returns 0-25 for characters "a"-"z", 26 for a space (" "), 27-37 for
 * numerical characters "0"-"9".
 */
static inline uint16_t ldoc_trie_offset_en(char chr)
{
    if (chr >= 'a' && chr <= 'z')
        return ((uint16_t)chr) - 'a';
    else if (chr >= '0' && chr <= '9')
        return ((uint16_t)chr) - 21;
    else if (chr == ' ')
        return 26;
    
    // TODO Error.
    return 0;
}

static inline char ldoc_trie_offset_en_inv(uint16_t off)
{
    if (off < 26)
        return 'a' + off;
    else if (off == 26)
        return ' ';
    else if (off > 26 && off < 38)
        return '0' + off;
    
    // TODO Error.
    return '^';
}

static char ldoc_trie_char(ldoc_trie_nde_t* nde, uint16_t off)
{
    switch (nde->tpe)
    {
        case EN_ALPH:
        case EN_ALPHNUM:
            return ldoc_trie_offset_en_inv(off);
            break;
        case ASCII:
            return nde->chr.c8[off];
        case UTF16:
            return (char)nde->chr.c16[off];
        case UTF32:
            return (char)nde->chr.c32[off];
        default:
            // TODO Error.
            return '^';
    }
}

static void ldoc_trie_nde_ndnt(uint16_t lvl)
{
    for (uint16_t i = 0; i < lvl * 2; i++)
        printf(" ");
}

static void ldoc_trie_nde_dmp(ldoc_trie_nde_t* nde, char chr, uint16_t lvl)
{
    // Bail out for fixed size array nodes (e.g., EN_ALPH).
    if (!nde)
        return;
    
    ldoc_trie_nde_ndnt(lvl);
    printf("%c, type %u, size %u, category %u, payload %08llx\n", chr, nde->tpe, nde->size, nde->anno.cat, (uint64_t)nde->anno.pld);
    
    for (uint16_t i = 0; i < nde->size; i++)
    {
        char dchr = ldoc_trie_char(nde, i);
        ldoc_trie_nde_dmp(nde->dscs[i], dchr, lvl + 1);
    }
}

void ldoc_trie_dmp(ldoc_trie_t* trie)
{
    ldoc_trie_nde_dmp(trie->root, '*', 0);
}

/**
 * Note: In order to work with `ldoc_trie_nde_free`, all types need to assign the
 *       descendents pointer-array memory allocated with `malloc`.
 */
static inline ldoc_trie_nde_t* ldoc_trie_nde_new(ldoc_trie_ptr_t tpe, ldoc_trie_alloc_t alloc, ldoc_trie_anno_t anno)
{
    ldoc_trie_char_t chr;
    uint16_t size;
    ldoc_trie_nde_t** dscs;
    size_t memsize;
    
    switch (tpe)
    {
        case EN_ALPH:
            chr.c0 = NULL;
            size = 27;
            memsize = size * sizeof(ldoc_trie_nde_t*);
            dscs = (ldoc_trie_nde_t**)malloc(memsize);
            
            if (!dscs)
            {
                // TODO Error.
            }
            
            memset(dscs, 0, memsize);
            break;
        case EN_ALPHNUM:
            chr.c0 = NULL;
            size = 37;
            memsize = size * sizeof(ldoc_trie_nde_t*);
            dscs = (ldoc_trie_nde_t**)malloc(memsize);
            
            if (!dscs)
            {
                // TODO Error.
            }
            
            memset(dscs, 0, memsize);
            break;
        case ASCII:
            // TODO
            size = 0;
            chr.c8 = NULL;
            dscs = NULL;
            break;
        case UTF16:
            // TODO
            size = 0;
            chr.c16 = NULL;
            dscs = NULL;
            break;
        case UTF32:
            size = 0;
            chr.c32 = NULL;
            dscs = NULL;
            break;
        default:
            // TODO Error.
            break;
    }
    
    ldoc_trie_nde_t* nde = (ldoc_trie_nde_t*)malloc(sizeof(ldoc_trie_nde_t));
    
    if (!nde)
    {
        // TODO Error.
    }
    
    nde->tpe = tpe;
    nde->chr = chr;
    nde->size = size;
    nde->alloc = alloc;
    nde->anno = anno;
    nde->dscs = dscs;
    
    return nde;
}

static inline void ldoc_trie_dsc_add(ldoc_trie_nde_t* nde, ldoc_trie_nde_t* dsc, char chr)
{
    char* c8;
    uint16_t* c16;
    uint32_t* c32;
    ldoc_trie_nde_t** dscs;
    uint16_t nsize = nde->size + 1;
    
    switch (nde->tpe)
    {
        case EN_ALPH:
        case EN_ALPHNUM:
            // TODO Error.
            dscs = NULL;
            break;
        case ASCII:
            if (nde->chr.c8)
            {
                c8 = realloc(nde->chr.c8, nsize * sizeof(char));
                dscs = realloc(nde->dscs, nsize * sizeof(ldoc_trie_nde_t**));
            }
            else
            {
                c8 = malloc(nsize * sizeof(char));
                dscs = malloc(nsize * sizeof(ldoc_trie_nde_t**));
            }
            // TODO Error handling.
            nde->chr.c8 = c8;
            nde->size = nsize;
            nde->chr.c8[nsize - 1] = chr;
            break;
        case UTF16:
            if (nde->chr.c16)
            {
                c16 = realloc(nde->chr.c16, nsize * sizeof(uint16_t));
                dscs = realloc(nde->dscs, nsize * sizeof(ldoc_trie_nde_t**));
            }
            else
            {
                c16 = malloc(nsize * sizeof(uint16_t));
                dscs = malloc(nsize * sizeof(ldoc_trie_nde_t**));
            }
            // TODO Error handling.
            nde->chr.c16 = c16;
            nde->size = nsize;
            nde->chr.c16[nsize - 1] = chr;
            break;
        case UTF32:
            if (nde->chr.c32)
            {
                c32 = realloc(nde->chr.c32, nsize * sizeof(uint32_t));
                dscs = realloc(nde->dscs, nsize * sizeof(ldoc_trie_nde_t**));
            }
            else
            {
                c32 = malloc(nsize * sizeof(uint32_t));
                dscs = malloc(nsize * sizeof(ldoc_trie_nde_t**));
            }
            // TODO Error handling.
            nde->chr.c32 = c32;
            nde->size = nsize;
            nde->chr.c32[nsize - 1] = chr;
            break;
        default:
            // TODO Error.
            break;
    }
    
    nde->dscs = dscs;
    nde->dscs[nsize - 1] = dsc;
}

static inline void ldoc_trie_nde_set(ldoc_trie_nde_t* nde, ldoc_trie_nde_t* dsc, char chr)
{
    switch (nde->tpe)
    {
        case EN_ALPH:
        case EN_ALPHNUM:
            nde->dscs[ldoc_trie_offset_en(chr)] = dsc;
            break;
        case ASCII:
        case UTF16:
        case UTF32:
            ldoc_trie_dsc_add(nde, dsc, chr);
            break;
        default:
            // TODO Error.
            break;
    }
}

ldoc_trie_t* ldoc_trie_new()
{
    ldoc_trie_nde_t* root = ldoc_trie_nde_new(UTF32, NDE_ROOT, LDOC_TRIE_ANNO_NULL);
    
    if (!root)
    {
        // TODO Error.
    }
    
    ldoc_trie_t* trie = (ldoc_trie_t*)malloc(sizeof(ldoc_trie_t));
    
    if (!trie)
    {
        // TODO Error.
    }
    
    trie->min = 0;
    trie->max = 0;
    trie->root = root;
    
    return trie;
}

ldoc_trie_nde_t* ldoc_trie_dsc_iter(ldoc_trie_nde_t* nde, char chr)
{
    for (uint16_t i = 0; i < nde->size; i++)
    {
        char ci;
        
        switch (nde->tpe) {
            case EN_ALPH:
            case EN_ALPHNUM:
                // TODO Error.
                return NULL;
            case ASCII:
                ci = nde->chr.c8[i];
                break;
            case UTF16:
                ci = nde->chr.c16[i];
                break;
            case UTF32:
                ci = nde->chr.c32[i];
                break;
            default:
                // TODO Error.
                return NULL;
        }
        
        if (chr == ci)
            return nde->dscs[i];
    }
    
    return NULL;
}

ldoc_trie_nde_t* ldoc_trie_dsc(ldoc_trie_nde_t* nde, char chr)
{
    if (!nde->size)
        return NULL;
    
    switch (nde->tpe)
    {
        case EN_ALPH:
        case EN_ALPHNUM:
            return nde->dscs[ldoc_trie_offset_en(chr)];
        case ASCII:
        case UTF16:
        case UTF32:
            return ldoc_trie_dsc_iter(nde, chr);
        default:
            // TODO Error.
            break;
    }
    
    return NULL;
}

ldoc_trie_nde_t* ldoc_trie_dscn(ldoc_trie_nde_t* nde, uint16_t n)
{
    if (!nde->size)
        return NULL;
    
    switch (nde->tpe)
    {
        case EN_ALPH:
            return nde->dscs[n];
        case EN_ALPHNUM:
            return nde->dscs[n];
        case ASCII:
            return nde->dscs[n];
        case UTF16:
            return nde->dscs[n];
        case UTF32:
            return nde->dscs[n];
        default:
            // TODO Error.
            break;
    }
    
    return NULL;
}

void ldoc_trie_nde_free(ldoc_trie_nde_t* nde)
{
    // Deal with fixed size array nodes (s.a. EN_ALPH):
    if (!nde)
        return;
    
    // If the node has descendents, then free those too:
    if (nde->size)
    {
        for (uint16_t n = 0; n < nde->size; n++)
        {
            ldoc_trie_nde_t* dsc = ldoc_trie_dscn(nde, n);
            ldoc_trie_nde_free(dsc);
        }
    }
    
    free(nde->dscs);
    
    free(nde);
}

void ldoc_trie_free(ldoc_trie_t* trie)
{
    ldoc_trie_nde_free(trie->root);
    
    free(trie);
}

ldoc_trie_nde_arr_t* ldoc_trie_nde_arr_new()
{
    ldoc_trie_nde_arr_t* arr = (ldoc_trie_nde_arr_t*)malloc(sizeof(ldoc_trie_nde_arr_t));
    
    arr->wptr = 0;
    arr->max = LDOC_TRIE_NDS_ARR_INIT;
    
    arr->nds = (ldoc_trie_nde_t**)malloc(sizeof(ldoc_trie_nde_t*) * arr->max);
    
    return arr;
}

void ldoc_trie_nde_arr_free(ldoc_trie_nde_arr_t* arr)
{
    free(arr);
}

void ldoc_trie_nde_arr_appnd(ldoc_trie_nde_arr_t* arr, ldoc_trie_nde_t* nde)
{
    // Increment size of array, if necessary:
    if (arr->wptr == arr->max)
    {
        arr->max += LDOC_TRIE_NDS_ARR_INC;
        
        ldoc_trie_nde_t** nds_inc = (ldoc_trie_nde_t**)malloc(sizeof(ldoc_trie_nde_t*) * arr->max);
        
        if (!nds_inc)
        {
            // TODO Error.
        }
        
        arr->nds = nds_inc;
    }
    
    arr->nds[arr->wptr++] = nde;
}

void ldoc_trie_add_trv(ldoc_trie_nde_t* nde, const char* str, ldoc_trie_ptr_t tpe, ldoc_trie_anno_t anno)
{
    char chr = *str;
    
    // If the string has been consumed, place the user supplied contents (i.e., we are done):
    if (!chr)
    {
        // Note: Should be obvious that this overwrites existing entries.
        nde->anno = anno;
        
        // Now the node carries some annotations:
        nde->alloc = NDE_ANNO;
        
        return;
    }
    
    ldoc_trie_nde_t* dsc = ldoc_trie_dsc(nde, chr);
    
    // If there is no descendent node available, create one now:
    if (!dsc)
    {
        // Set category and payload to default values here, because this might not be
        // the last node we are creating on the "way down".
        dsc = ldoc_trie_nde_new(tpe, NDE_EMPTY, LDOC_TRIE_ANNO_NULL);
        
        ldoc_trie_nde_set(nde, dsc, chr);
    }
    
    ldoc_trie_add_trv(dsc, str + 1, tpe, anno);
}

ldoc_trie_nde_t* ldoc_trie_lookup_trv(ldoc_trie_nde_t* nde, const char* string, bool prefixes)
{
    char chr = *string;
    
    // If the string has been consumed, then the entry has been found in the trie.
    if (!chr)
    {
        if (prefixes)
            return nde;
        else if (nde->alloc == NDE_ANNO)
            return nde;
        else
            return NULL;
    }
    
    ldoc_trie_nde_t* dsc = ldoc_trie_dsc(nde, chr);
    
    if (!dsc)
        return NULL;
    
    return ldoc_trie_lookup_trv(dsc, string + 1, prefixes);
}

void ldoc_trie_add(ldoc_trie_t* trie, const char* str, ldoc_trie_ptr_t tpe, ldoc_trie_anno_t anno)
{
    ldoc_trie_add_trv(trie->root, str, tpe, anno);
}

ldoc_trie_nde_t* ldoc_trie_lookup(ldoc_trie_t* trie, const char* string, bool prefixes)
{
    return ldoc_trie_lookup_trv(trie->root, string, prefixes);
}
