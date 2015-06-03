/*
 * Copyright (c) 2015 CODAMONO, Ontario, Canada
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ldoc__document__
#define __ldoc__document__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include <Python.h>

#include "trie.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LDOC_QSTK_FULL(qstk) (qstk->wptr + 1) % qstk->max == qstk->rptr
#define LDOC_QSTK_NDE_ADDR(qstk,n) sizeof(ldoc_nde_t*) * (n)
    
/**
 * Node types: denoting some form of content hierarchy or atomic concepts.
 */
typedef enum
{
    LDOC_NDE_RT,  // "root", cannot be instantiated manually
    LDOC_NDE_UA,  // "unassigned"
    LDOC_NDE_H1,  // "header 1", top-most header, largest header
    LDOC_NDE_H2,  // "header 2"
    LDOC_NDE_H3,  // "header 3"
    LDOC_NDE_H4,  // "header 4"
    LDOC_NDE_H5,  // "header 5"
    LDOC_NDE_H6,  // "header 6", lowest header, smallest header
    LDOC_NDE_PAR, // "paragraph"
    LDOC_NDE_UL,  // "unordered list"
    LDOC_NDE_OL,  // "ordered list"
    /**
     * An "anchor" that is named by a single entity.
     */
    LDOC_NDE_ANC,
    LDOC_NDE_NN,  // "named node"
    LDOC_NDE_OO   // ontology object, resolves ontology references (OR)
} ldoc_struct_t;
    
/**
 * Entity types: denoting sequentially ordered, but essentially "flat", contents.
 */
typedef enum
{
    LDOC_ENT_TXT,
    LDOC_ENT_EM1,
    LDOC_ENT_EM2,
    LDOC_ENT_URI,
    LDOC_ENT_REF, // "reference", points to an anchor ("ANC")
    LDOC_ENT_NUM, // "number"
    LDOC_ENT_BL,  // "boolean", truth value
    LDOC_ENT_BR,  // boolean reference (for example, "citation": true)
    LDOC_ENT_NR,  // numbered reference (for example, "citation": 12)
    LDOC_ENT_OR   // ontology reference, base determined by an ontology object (OO)
} ldoc_content_t;

/**
 *
 */
typedef struct ldoc_raw_t
{
    uint8_t* pld;
    uint64_t len;
} ldoc_raw_t;

typedef union
{
    bool bl;
    char* str;
    ldoc_raw_t raw;
} ldoc_anno_pld_t;

/**
 *
 */
typedef struct ldoc_doc_anno_t
{
    ldoc_anno_pld_t dtm;
    ldoc_anno_pld_t anno;
} ldoc_doc_anno_t;
    
/**
 *
 */
typedef struct ldoc_py_t
{
    PyObject* dtm;
    PyObject* anno;
} ldoc_py_t;

/**
 *
 */
typedef union
{
    bool bl;
    char* str;
    ldoc_raw_t raw;
    ldoc_doc_anno_t pair;
    ldoc_py_t py;
} ldoc_pld_t;
    
typedef enum
{
    LDOC_SER_CSTR,
    LDOC_SER_PY_INT,
    LDOC_SER_PY_BL,
    LDOC_SER_PY_FLT,
    LDOC_SER_PY_STR,
    LDOC_SER_PY_LST,
    LDOC_SER_PY_DCT
} ldoc_serpld_t;
    
/**
 *
 */
typedef struct ldoc_ser_t
{
    ldoc_serpld_t tpe;
    ldoc_pld_t pld;
} ldoc_ser_t;
    
/**
 * @brief TODO
 *
 * Note: A node can hold information about multiple entities, but a document
 *       has to follow a strict hierarchical structure. Markup overlaps need
 *       to be encoded via separate entities.
 */
typedef struct ldoc_nde_t
{
    /**
     * Parent node.
     */
    struct ldoc_nde_t* prnt;
    ldoc_struct_t tpe;
    ldoc_doc_anno_t mkup;
    /**
     * Longest path to a leaf.
     */
    uint32_t lpth;
    uint32_t ent_cnt;
    uint32_t dsc_cnt;
    TAILQ_HEAD(ldoc_nde_list, ldoc_nde_t) dscs;
    TAILQ_HEAD(ldoc_ent_list, ldoc_ent_t) ents;
    TAILQ_ENTRY(ldoc_nde_t) ldoc_nde_entries;
} ldoc_nde_t;

/**
 * @brief Node quick stack.
 *
 * rptr : points to a node, if unequal to wptr; empty stack otherwise (rptr == wptr)
 * wptr : always points to an empty node
 */
typedef struct ldoc_nde_qstk_t
{
    size_t rptr;
    size_t wptr;
    size_t max;
    size_t inc;
    ldoc_nde_t** nds;
} ldoc_nde_qstk_t;

/**
 * @brief Document entity: a concrete piece of data that is part of a document.
 *
 * A document entity is an actual representation of tangible data in a document.
 * The entity is typed (`tpe`); applicable types are defined by `ldoc_content_t`.
 * Data is accessed as payload (`pld`), whose data type depends on the entity
 * type.
 *
 * Example: a primary heading is encoded as `tpe` set to `ldoc_content_t.H1` and
 *          `ldoc_set_t.str` payload set to "Heading Example".
 */
typedef struct ldoc_ent_t
{
    ldoc_content_t tpe;
    ldoc_pld_t pld;
    TAILQ_ENTRY(ldoc_ent_t) ldoc_ent_entries;
} ldoc_ent_t;

/**
 * @brief Document: TODO
 */
typedef struct ldoc_doc_t
{
    /**
     * Root node of the document.
     */
    ldoc_nde_t* rt;
} ldoc_doc_t;

/**
 * @brief Cursor position: a null-based cursor that points between character
 *                         positions of a node (offset relative to node contents).
 */
typedef struct ldoc_pos_t
{
    ldoc_nde_t* nde;
    uint64_t nde_off;
    uint64_t off;
} ldoc_pos_t;
   
/**
 *
 */
typedef struct ldoc_coord_t
{
    /**
     * Level.
     */
    uint32_t lvl;
    /**
     * Plane.
     */
    uint32_t pln;
} ldoc_coord_t;

/**
 * @brief Visitor object: calls the given visitor functions based on entity type.
 *
 * TODO
 *
 * Note: This structure and the enumerations `ldoc_struct_t` and `ldoc_content_t`
 *       go hand-in-hand. Each content type can have its own visitor, but it is
 *       also possible to re-use a single function for all content types.
 */
typedef struct ldoc_vis_nde_t
{
    ldoc_ser_t* (*vis_rt)(ldoc_nde_t* nde, ldoc_coord_t* coord);
    ldoc_ser_t* (*vis_ua)(ldoc_nde_t* nde, ldoc_coord_t* coord);
    ldoc_ser_t* (*vis_par)(ldoc_nde_t* nde, ldoc_coord_t* coord);
    ldoc_ser_t* (*vis_ul)(ldoc_nde_t* nde, ldoc_coord_t* coord);
    ldoc_ser_t* (*vis_ol)(ldoc_nde_t* nde, ldoc_coord_t* coord);
    ldoc_ser_t* (*vis_anc)(ldoc_nde_t* nde, ldoc_coord_t* coord);
    ldoc_ser_t* (*vis_nn)(ldoc_nde_t* nde, ldoc_coord_t* coord);
    ldoc_ser_t* (*vis_oo)(ldoc_nde_t* nde, ldoc_coord_t* coord);
    ldoc_ser_t* (*vis_h1)(ldoc_nde_t* nde, ldoc_coord_t* coord);
    ldoc_ser_t* (*vis_h2)(ldoc_nde_t* nde, ldoc_coord_t* coord);
    ldoc_ser_t* (*vis_h3)(ldoc_nde_t* nde, ldoc_coord_t* coord);
    ldoc_ser_t* (*vis_h4)(ldoc_nde_t* nde, ldoc_coord_t* coord);
    ldoc_ser_t* (*vis_h5)(ldoc_nde_t* nde, ldoc_coord_t* coord);
    ldoc_ser_t* (*vis_h6)(ldoc_nde_t* nde, ldoc_coord_t* coord);
} ldoc_vis_nde_t;

    
typedef struct ldoc_vis_nde_ord_t
{
    ldoc_ser_t* (*vis_setup)(void);
    ldoc_ser_t* (*vis_teardown)(void);
    ldoc_vis_nde_t pre;
    ldoc_vis_nde_t infx;
    ldoc_vis_nde_t post;
} ldoc_vis_nde_ord_t;
    
/**
 * @brief Visitor object: calls the given visitor functions based on entity type.
 *
 * TODO
 *
 * Note: This structure and the enumerations `ldoc_struct_t` and `ldoc_content_t`
 *       go hand-in-hand. Each content type can have its own visitor, but it is
 *       also possible to re-use a single function for all content types.
 */
typedef struct ldoc_vis_ent_t
{
    ldoc_ser_t* (*vis_bl)(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord);
    ldoc_ser_t* (*vis_br)(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord);
    ldoc_ser_t* (*vis_em1)(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord);
    ldoc_ser_t* (*vis_em2)(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord);
    ldoc_ser_t* (*vis_uri)(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord);
    ldoc_ser_t* (*vis_ref)(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord);
    ldoc_ser_t* (*vis_num)(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord);
    ldoc_ser_t* (*vis_nr)(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord);
    ldoc_ser_t* (*vis_or)(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord);
    ldoc_ser_t* (*vis_txt)(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord);
} ldoc_vis_ent_t;

extern ldoc_ser_t* LDOC_SER_NULL;
extern ldoc_pos_t* LDOC_POS_NULL;
extern ldoc_nde_t* LDOC_NDE_NULL;
extern ldoc_ent_t* LDOC_ENT_NULL;
extern ldoc_doc_anno_t LDOC_ANNO_NULL;

/*
 * @brief Returns a string representation of a Python object (Python call "str(obj)").
 */
char* ldoc_py2str(PyObject* obj);
   
ldoc_doc_t* ldoc_pydict2doc(PyObject* dict);
    
/**
 * @brief Allocates a new serialization structure.
 */
ldoc_ser_t* ldoc_ser_new(ldoc_serpld_t tpe);

/**
 * @brief Allocates a new node visitor data structure.
 */
ldoc_vis_nde_ord_t* ldoc_vis_nde_ord_new();

/**
 * @brief Allocates a new entity visitor data structure.
 */
ldoc_vis_ent_t* ldoc_vis_ent_new();

/**
 * @brief Uniformly populates a node visitor object with a single visitor function.
 *
 * Sets all visitor functions of a node visitor object to a single function.
 */
void ldoc_vis_nde_uni(ldoc_vis_nde_t* vis, ldoc_ser_t* (*vis_uni)(ldoc_nde_t* nde, ldoc_coord_t* coord));

/**
 * @brief Uniformly populates an entity visitor object with a single visitor function.
 *
 * Sets all visitor functions of an entity visitor object to a single function.
 */
void ldoc_vis_ent_uni(ldoc_vis_ent_t* vis, ldoc_ser_t* (*vis_uni)(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord));

ldoc_pos_t* ldoc_pos_new(ldoc_nde_t* nde, uint64_t nde_off, uint64_t off);

void ldoc_pos_free(ldoc_pos_t* pos);
    
/*
 * @brief Create a new document.
 */
ldoc_doc_t* ldoc_doc_new();

/*
 * @brief Releases all allocated memory of a document (including nodes and entities).
 */
void ldoc_doc_free(ldoc_doc_t* doc);

/*
 * @brief Creates a new entity with the payload set to NULL.
 */
ldoc_ent_t* ldoc_ent_new(ldoc_content_t tpe);

/*
 * @brief Deletes an entity and its payload.
 */
void ldoc_ent_free(ldoc_ent_t* ent);

/*
 * @brief Creates a new node with empty entity list and no descendants.
 */
ldoc_nde_t* ldoc_nde_new(ldoc_struct_t tpe);

/*
 * @brief Deletes a node, its associated entities, and its descendant nodes.
 */
void ldoc_nde_free(ldoc_nde_t* nde);

/**
 * @brief Add an entity to the end of a node's entity list.
 */
void ldoc_nde_ent_push(ldoc_nde_t* nde, ldoc_ent_t* ent);
    
/**
 * @brief Remove an entity at the end of a node's entity list.
 */
void ldoc_nde_ent_pop();

/**
 * @brief Add an entity to the beginning of a node's entity list.
 */
void ldoc_nde_ent_shift(ldoc_nde_t* nde, ldoc_ent_t* ent);

/**
 * @brief Remove an entity from the beginning of a node's entity list.
 */
void ldoc_nde_ent_unshift();

/**
 * @brief Insert an entity before another entity in a node's entity list.
 */
void ldoc_nde_ent_ins();

/**
 * @brief Remove a specific entity from a node's entity list.
 */
void ldoc_nde_ent_rm();

/**
 * @brief Add an entity to the end of a node's descendant list.
 */
void ldoc_nde_dsc_push(ldoc_nde_t* nde, ldoc_nde_t* dsc);

/**
 * @brief Remove an entity at the end of a node's descendant list.
 */
void ldoc_nde_dsc_pop();

/**
 * @brief Add an entity to the beginning of a node's descendant list.
 */
void ldoc_nde_dsc_shift(ldoc_nde_t* nde, ldoc_nde_t* dsc);

/**
 * @brief Remove an entity from the beginning of a node's descendant list.
 */
void ldoc_nde_dsc_unshift();

/**
 * @brief Insert an entity before another entity in a node's descendant list.
 */
void ldoc_nde_dsc_ins();

/**
 * @brief Remove a specific entity from a node's descendant list.
 */
void ldoc_nde_dsc_rm();

/**
 * @brief Returns the level at which a node is located at.
 */
uint16_t ldoc_nde_lvl(ldoc_nde_t* nde);
    
/**
 *
 */
ldoc_ser_t* ldoc_format(ldoc_doc_t* doc, ldoc_vis_nde_ord_t* vis_nde, ldoc_vis_ent_t* vis_ent);

/**
 * @brief Given a null-based cursor position that is counted from the beginning of
 *        a document, returns the document node and offset within the node that entails
 *        the cursor position.
 *
 * Will return the deepest node (most specialized; furthest down the tree) for the
 * given cursor position. If the cursor position falls on a boundary -- the position
 * between two adjacent nodes -- then the first node will be returned (the node for
 * which the cursor position denotes the end of the node, rather than the beginning of
 * the following node).
 *
 * The cursor offset is null-based and addresses the position between two characters.
 * Cursor position 0 is the start of the text; the position before the first character
 * (if any) that appears in a document. If there are n characters in the document, then
 * position n refers to the end of the text.
 *
 * Note: Will currently perform very slow on extremely long texts. Needs optimization.
 */
ldoc_pos_t* ldoc_find_pos(ldoc_doc_t* doc, uint64_t off);

/**
 * @brief Given a null-based cursor position that is counted from the beginning of
 *        a document as well as a search string, returns the most detailed node that
 *        captures the first match of the search string that occurs on or after the
 *        cursor position.
 *
 * The cursor offset is null-based and addresses the position between two characters.
 * Cursor position 0 is the start of the text; the position before the first character
 * (if any) that appears in a document. If there are n characters in the document, then
 * position n refers to the end of the text.
 *
 * Note: Will currently perform very slow on extremely long texts. Needs optimization.
 */
ldoc_pos_t* ldoc_find_kw(ldoc_doc_t* doc, uint64_t off, char* str);

//
// HTML
//

/**
 * @brief TODO
 */
ldoc_ser_t* ldoc_vis_setup_html(void);

/**
 * @brief TODO
 */
ldoc_ser_t* ldoc_vis_teardown_html(void);
    
/**
 * @brief TODO
 */
ldoc_ser_t* ldoc_vis_nde_pre_html(ldoc_nde_t* nde, ldoc_coord_t* coord);

/**
 * @brief TODO
 */
ldoc_ser_t* ldoc_vis_nde_infx_html(ldoc_nde_t* nde, ldoc_coord_t* coord);

/**
 * @brief TODO
 */
ldoc_ser_t* ldoc_vis_nde_post_html(ldoc_nde_t* nde, ldoc_coord_t* coord);

/**
 * @brief TODO
 */
ldoc_ser_t* ldoc_vis_ent_html(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord);

//
// JSON
//
    
/**
 * @brief TODO
 */
ldoc_ser_t* ldoc_vis_setup_json(void);

/**
 * @brief TODO
 */
ldoc_ser_t* ldoc_vis_teardown_json(void);

/**
 * @brief TODO
 */
ldoc_ser_t* ldoc_vis_nde_pre_json(ldoc_nde_t* nde, ldoc_coord_t* coord);

/**
 * @brief TODO
 */
ldoc_ser_t* ldoc_vis_nde_infx_json(ldoc_nde_t* nde, ldoc_coord_t* coord);

/**
 * @brief TODO
 */
ldoc_ser_t* ldoc_vis_nde_post_json(ldoc_nde_t* nde, ldoc_coord_t* coord);

/**
 * @brief TODO
 */
ldoc_ser_t* ldoc_vis_ent_json(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord);
    
//
// Python
//

/**
 * @brief TODO
 */
ldoc_ser_t* ldoc_vis_setup_py(void);

/**
 * @brief TODO
 */
ldoc_ser_t* ldoc_vis_teardown_py(void);

/**
 * @brief TODO
 */
ldoc_ser_t* ldoc_vis_nde_pre_py(ldoc_nde_t* nde, ldoc_coord_t* coord);

/**
 * @brief TODO
 */
ldoc_ser_t* ldoc_vis_nde_infx_py(ldoc_nde_t* nde, ldoc_coord_t* coord);

/**
 * @brief TODO
 */
ldoc_ser_t* ldoc_vis_nde_post_py(ldoc_nde_t* nde, ldoc_coord_t* coord);

/**
 * @brief TODO
 */
ldoc_ser_t* ldoc_vis_ent_py(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* defined(__ldoc__document__) */

