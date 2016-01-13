/*
 * Copyright (c) 2015-2016 CODAMONO, Ontario, Canada
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at: http://mozilla.org/MPL/2.0/
 */

/**
 * @mainpage An Open Source iOS, Mac OS X, and Linux C-Library
 *
 * LibDocument provides functions and data structures for working with documents
 * and prefix trees (tries). It is currently being used in CODAMONO's commercial
 * products [BioInterchange](https://www.codamono.com/biointerchange/) (Mac OS X
 * and Linux) and [Brightword](https://www.codamono.com/brightword/) (iOS).
 *
 * **License:** [Mozilla Public License, v. 2.0](http://mozilla.org/MPL/2.0/)
 *
 * ### Documents
 * 
 * **Header:** document.h
 *
 * **Unit-tests/examples:** document.cpp
 *
 * Functions and data structures for creating annotated documents that are treelike
 * (cross references permitted). Serialization functions for HTML, JSON, and Python
 * objects.
 *
 * ### Tries (Prefix Trees)
 *
 * **Header:** trie.h
 *
 * **Unit-tests/examples:** trie.cpp
 *
 * Functions and data structures for CRUD operations on tries (prefix trees).
 *
 * ### JSON/JSON-LD Reader
 *
 * **Header:** json.h
 *
 * **Unit-tests/examples:** json.cpp
 *
 * JSON and JSON-LD readers that create annotated document representations.
 */

/**
 * @file document.h
 *
 * @brief Document dot h.
 *
 *
 */

#ifndef __ldoc__document__
#define __ldoc__document__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#ifndef LDOC_NOPYTHON
#include <Python.h>
#endif // #ifndef LDOC_NOPYTHON

#include "trie.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LDOC_QSTK_FULL(qstk) (qstk->wptr + 1) % qstk->max == qstk->rptr
#define LDOC_QSTK_NDE_ADDR(qstk,n) sizeof(ldoc_nde_t*) * (n)
    
/**
 * @brief Node types denoting the document content hierarchy and atomic concepts.
 *
 * Nodes are providing the main structure of a document. Their type indicates how
 * child nodes and child entities are to be interpreted.
 */
typedef enum
{
    /// Document "root", cannot be instantiated manually.
    LDOC_NDE_RT,
    /// An "unassigned" node type; for use when no other node type fits.
    LDOC_NDE_UA,
    /// A "header 1", top-most header, largest header.
    LDOC_NDE_H1,
    /// A "header 2", second largest header.
    LDOC_NDE_H2,
    /// A "header 3", third largest header.
    LDOC_NDE_H3,
    /// A "header 4", fourth largest header.
    LDOC_NDE_H4,
    /// A "header 5", fifth largest header .
    LDOC_NDE_H5,
    /// A "header 6", sixth largest header, smallest header.
    LDOC_NDE_H6,
    /// A node that denotes a "paragraph".
    LDOC_NDE_PAR,
    /// An "unordered list".
    LDOC_NDE_UL,
    /// An "ordered list".
    LDOC_NDE_OL,
    /// An "anchor" within a document that is named by a single entity; linked to by a reference entity (`LDOC_ENT_REF`).
    LDOC_NDE_ANC,
    /// A "named node".
    LDOC_NDE_NN,
    /// An "ontology object"; resolves ontology references (`LDOC_ENT_OR`).
    LDOC_NDE_OO
} ldoc_struct_t;
    
/**
 * @brief Entity types denoting sequentially ordered, but essentially "flat", node contents.
 *
 * Entities are children of nodes and represent node contents. The type of their parent node
 * determines how they are to be interpreted, but entity types themselves permit for more fine
 * grained control.
 */
typedef enum
{
    /// Most general entity type; plain text.
    LDOC_ENT_TXT,
    /// Emphasized text (type 1 of 2).
    LDOC_ENT_EM1,
    /// Emphasized text (type 2 of 2).
    LDOC_ENT_EM2,
    /// An arbitrary URI.
    LDOC_ENT_URI,
    /// A "reference", points to an anchor node (`LDOC_NDE_ANC`).
    LDOC_ENT_REF,
    /// A "number"; integer, real, or floating point.
    LDOC_ENT_NUM,
    /// A "boolean"; truth value.
    LDOC_ENT_BL,
    /// A "boolean reference" (for example, "citation": true).
    LDOC_ENT_BR,
    /// A "numbered reference" (for example, "citation": 12).
    LDOC_ENT_NR,
    /// An "ontology reference"; base determined by an ontology object node (`LDOC_NDE_OO`).
    LDOC_ENT_OR
} ldoc_content_t;

/**
 * @brief Raw data structure (payload pointer and data length).
 *
 * Raw data representation with a payload pointer and data length in bytes.
 */
typedef struct ldoc_raw_t
{
    /**
     * Pointer to the payload (the data).
     */
    uint8_t* pld;
    /**
     * Data length in bytes.
     */
    uint64_t len;
} ldoc_raw_t;

/**
 * @brief Payload data types for annotations.
 *
 * Data types of payloads that are associated with document annotations.
 */
typedef union
{
    /**
     * Boolean.
     */
    bool bl;
    /**
     * Null-terminated string.
     */
    char* str;
    /**
     * Raw data structure.
     */
    ldoc_raw_t raw;
} ldoc_anno_pld_t;

/**
 * @brief Document annotations (payloads of datum and annotation assignments).
 *
 * Contains document annotations that are a pair: a datum payload and a (datum) annotation payload.
 */
typedef struct ldoc_doc_anno_t
{
    /**
     * Datum.
     */
    ldoc_anno_pld_t dtm;
    /**
     * Annotation of the datum.
     */
    ldoc_anno_pld_t anno;
} ldoc_doc_anno_t;
    
#ifndef LDOC_NOPYTHON
    
/**
 * @brief Python representation of `ldoc_doc_anno_t`.
 *
 * <strong>Note:</strong> Removed when `LDOC_NOPYTHON` is defined.
 *
 * Python objects that contain document annotation pairs; see also `ldoc_doc_anno_t`.
 */
typedef struct ldoc_py_t
{
    /**
     * Datum.
     */
    PyObject* dtm;
    /**
     * Annotation of the datum.
     */
    PyObject* anno;
} ldoc_py_t;

#endif // #ifndef LDOC_NOPYTHON
    
/**
 * @brief Payload data types for datums.
 *
 * Data types of payloads that are associated with datums.
 */
typedef union
{
    /**
     * Boolean.
     */
    bool bl;
    /**
     * Null-terminated string.
     */
    char* str;
    /**
     * Raw data structure.
     */
    ldoc_raw_t raw;
    /**
     * Annotation pair (datum + annotation payload)
     */
    ldoc_doc_anno_t pair;
#ifndef LDOC_NOPYTHON
    /**
     * Python object. <strong>Note:</strong> Removed when `LDOC_NOPYTHON` is defined.
     */
    ldoc_py_t py;
#endif // #ifndef LDOC_NOPYTHON
} ldoc_pld_t;
    
/**
 * @brief Serialization payload types.
 *
 * Payload types for serializing documents.
 */
typedef enum
{
    /**
     * Null-terminated string.
     */
    LDOC_SER_CSTR,
    /**
     * Python integer.
     */
    LDOC_SER_PY_INT,
    /**
     * Python boolean.
     */
    LDOC_SER_PY_BL,
    /**
     * Python float.
     */
    LDOC_SER_PY_FLT,
    /**
     * Python string.
     */
    LDOC_SER_PY_STR,
    /**
     * Python list.
     */
    LDOC_SER_PY_LST,
    /**
     * Python dictionary.
     */
    LDOC_SER_PY_DCT
} ldoc_serpld_t;
    
/**
 * @brief Serialization data structure.
 *
 * A data serialization structure that is an aggregate of data payload and data type.
 */
typedef struct ldoc_ser_t
{
    /**
     * Serialization data type.
     */
    ldoc_serpld_t tpe;
    /**
     * Serialization payload.
     */
    ldoc_pld_t pld;
} ldoc_ser_t;
    
/**
 * @brief A document node.
 *
 * A document node that itself can be marked up with an annotation and which
 * has exactly one parent, zero or more node descendants, and zero or more
 * entities associated with it (latter contain data pertaining to the node).
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
    /**
     * Node type.
     */
    ldoc_struct_t tpe;
    /**
     * Optional node annotation (markup).
     */
    ldoc_doc_anno_t mkup;
    /**
     * Longest path to a leaf.
     */
    uint32_t lpth;
    /**
     * Number of entities.
     */
    uint32_t ent_cnt;
    /**
     * Number of descending nodes.
     */
    uint32_t dsc_cnt;
    /**
     * List of descendent nodes.
     */
    TAILQ_HEAD(ldoc_nde_list, ldoc_nde_t) dscs;
    /**
     * List of entities.
     */
    TAILQ_HEAD(ldoc_ent_list, ldoc_ent_t) ents;
    /**
     * Internal list-pointer.
     */
    TAILQ_ENTRY(ldoc_nde_t) ldoc_nde_entries;
} ldoc_nde_t;

/**
 * @brief Node quick-stack for efficiently handing node allocations/deallocations.
 *
 * `rptr` points to a node, if unequal to wptr, or the empty stack otherwise (`rptr` equals `wptr`).
 * `wptr` always points to an empty node.
 */
typedef struct ldoc_nde_qstk_t
{
    /**
     * Read pointer.
     */
    size_t rptr;
    /**
     * Write pointer.
     */
    size_t wptr;
    /**
     * Maximum size of the stack.
     */
    size_t max;
    /**
     * Increment size in which the stack is being increased.
     */
    size_t inc;
    /**
     * Array of node pointers (the actual stack).
     */
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
    /**
     * Parent node.
     */
    ldoc_nde_t* prnt;
    /**
     * Entity content type.
     */
    ldoc_content_t tpe;
    /**
     * Entity payload (datum).
     */
    ldoc_pld_t pld;
    /**
     * Internal list-pointer.
     */
    TAILQ_ENTRY(ldoc_ent_t) ldoc_ent_entries;
} ldoc_ent_t;

/**
 * @brief Document structure.
 */
typedef struct ldoc_doc_t
{
    /**
     * Root node of the document.
     */
    ldoc_nde_t* rt;
} ldoc_doc_t;

/**
 * @brief A cursor position within a node.
 *
 * This is a null-based cursor that points between character
 * positions of a node (offset relative to node contents).
 */
typedef struct ldoc_pos_t
{
    /**
     * Node to which the cursor applies.
     */
    ldoc_nde_t* nde;
    /**
     * Character offset at the beginning of the node (document context).
     */
    uint64_t nde_off;
    /**
     * Cursor offset within the node.
     */
    uint64_t off;
} ldoc_pos_t;

/**
 * @brief Information carrying types in a document: nodes and entities.
 */
typedef union
{
    ldoc_nde_t* nde;
    ldoc_ent_t* ent;
} ldoc_info_t;

/**
 * @brief Entity or node returned by a search for a specific annotation.
 */
typedef struct ldoc_res_t
{
    bool nde;
    ldoc_info_t info;
} ldoc_res_t;
    
/**
 * @brief Coordinate with in a document.
 *
 * The coordinate level `lvl` denotes the distance to the root node, whilst the
 * plane `pln` denotes the horizontal position within a level.
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
 * @brief A node visitor object calls the given visitor functions based on node type.
 *
 * Note: This structure and the enumerations `ldoc_struct_t` and `ldoc_content_t`
 *       go hand-in-hand. Each content type can have its own visitor, but it is
 *       also possible to re-use a single function for all content types.
 */
typedef struct ldoc_vis_nde_t
{
    /**
     * Called when visiting the document root (`LDOC_NDE_RT`).
     */
    ldoc_ser_t* (*vis_rt)(ldoc_nde_t* nde, ldoc_coord_t* coord);
    /**
     * Called when visiting an "unassigned" node (`LDOC_NDE_UA`).
     */
    ldoc_ser_t* (*vis_ua)(ldoc_nde_t* nde, ldoc_coord_t* coord);
    /**
     * Called when visiting a "paragraph" node (`LDOC_NDE_PAR`).
     */
    ldoc_ser_t* (*vis_par)(ldoc_nde_t* nde, ldoc_coord_t* coord);
    /**
     * Called when visiting an "unordered list" node (`LDOC_NDE_UL`).
     */
    ldoc_ser_t* (*vis_ul)(ldoc_nde_t* nde, ldoc_coord_t* coord);
    /**
     * Called when visiting an "ordered list" node (`LDOC_NDE_OL`).
     */
    ldoc_ser_t* (*vis_ol)(ldoc_nde_t* nde, ldoc_coord_t* coord);
    /**
     * Called when visiting an "anchor" node (`LDOC_NDE_ANC`).
     */
    ldoc_ser_t* (*vis_anc)(ldoc_nde_t* nde, ldoc_coord_t* coord);
    /**
     * Called when visiting a "named node" (`LDOC_NDE_NN`).
     */
    ldoc_ser_t* (*vis_nn)(ldoc_nde_t* nde, ldoc_coord_t* coord);
    /**
     * Called when visiting an "ontology object" node (`LDOC_NDE_OO`).
     */
    ldoc_ser_t* (*vis_oo)(ldoc_nde_t* nde, ldoc_coord_t* coord);
    /**
     * Called when visiting a "header 1" node (`LDOC_NDE_H1`).
     */
    ldoc_ser_t* (*vis_h1)(ldoc_nde_t* nde, ldoc_coord_t* coord);
    /**
     * Called when visiting a "header 2" node (`LDOC_NDE_H2`).
     */
    ldoc_ser_t* (*vis_h2)(ldoc_nde_t* nde, ldoc_coord_t* coord);
    /**
     * Called when visiting a "header 3" node (`LDOC_NDE_H3`).
     */
    ldoc_ser_t* (*vis_h3)(ldoc_nde_t* nde, ldoc_coord_t* coord);
    /**
     * Called when visiting a "header 4" node (`LDOC_NDE_H4`).
     */
    ldoc_ser_t* (*vis_h4)(ldoc_nde_t* nde, ldoc_coord_t* coord);
    /**
     * Called when visiting a "header 5" node (`LDOC_NDE_H5`).
     */
    ldoc_ser_t* (*vis_h5)(ldoc_nde_t* nde, ldoc_coord_t* coord);
    /**
     * Called when visiting a "header 6" node (`LDOC_NDE_H6`).
     */
    ldoc_ser_t* (*vis_h6)(ldoc_nde_t* nde, ldoc_coord_t* coord);
} ldoc_vis_nde_t;

/**
 * @brief Ordered visitor structure for setting up serialization, tearing it down, and pre-, in-, and postfix visits.
 */
typedef struct ldoc_vis_nde_ord_t
{
    /**
     * Function for setting up document serialization.
     */
    ldoc_ser_t* (*vis_setup)(void);
    /**
     * Function for tearing down document serialization.
     */
    ldoc_ser_t* (*vis_teardown)(void);
    /**
     * Functions for prefix node visits.
     */
    ldoc_vis_nde_t pre;
    /**
     * Functions for infix node visits.
     */
    ldoc_vis_nde_t infx;
    /**
     * Functions for postfix node visits.
     */
    ldoc_vis_nde_t post;
} ldoc_vis_nde_ord_t;
    
/**
 * @brief An entity visitor object calls the given visitor functions based on entity type.
 *
 * Note: This structure and the enumerations `ldoc_struct_t` and `ldoc_content_t`
 *       go hand-in-hand. Each content type can have its own visitor, but it is
 *       also possible to re-use a single function for all content types.
 */
typedef struct ldoc_vis_ent_t
{
    /**
     * Called when visiting a "boolean" entity (`LDOC_ENT_BL`).
     */
    ldoc_ser_t* (*vis_bl)(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord);
    /**
     * Called when visiting a "boolean reference" entity (`LDOC_ENT_BR`).
     */
    ldoc_ser_t* (*vis_br)(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord);
    /**
     * Called when visiting an "emphasized text" (type 1) entity (`LDOC_ENT_EM1`).
     */
    ldoc_ser_t* (*vis_em1)(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord);
    /**
     * Called when visiting an "emphasized text" (type 2) entity (`LDOC_ENT_EM2`).
     */
    ldoc_ser_t* (*vis_em2)(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord);
    /**
     * Called when visiting an "URI" entity (`LDOC_ENT_URI`).
     */
    ldoc_ser_t* (*vis_uri)(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord);
    /**
     * Called when visiting a "reference" entity (`LDOC_ENT_REF`).
     */
    ldoc_ser_t* (*vis_ref)(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord);
    /**
     * Called when visiting a "number" entity (`LDOC_ENT_NUM`).
     */
    ldoc_ser_t* (*vis_num)(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord);
    /**
     * Called when visiting a "numbered reference" entity (`LDOC_ENT_NR`).
     */
    ldoc_ser_t* (*vis_nr)(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord);
    /**
     * Called when visiting an "ontology reference" entity (`LDOC_ENT_OR`).
     */
    ldoc_ser_t* (*vis_or)(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord);
    /**
     * Called when visiting a "plain text" entity (`LDOC_ENT_TXT`).
     */
    ldoc_ser_t* (*vis_txt)(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord);
} ldoc_vis_ent_t;

/**
 * @brief Null pointer for serialization data structures.
 */
extern ldoc_ser_t* LDOC_SER_NULL;
/**
 * @brief Null pointer for cursors.
 */
extern ldoc_pos_t* LDOC_POS_NULL;
/**
 * @brief Null pointer for node data structures.
 */
extern ldoc_nde_t* LDOC_NDE_NULL;
/**
 * @brief Null pointer for entity data structures.
 */
extern ldoc_ent_t* LDOC_ENT_NULL;
/**
 * @brief Null pointer for result data structures.
 */
extern ldoc_res_t* LDOC_RES_NULL;
/**
 * @brief Null pointer for annotation data structures.
 */
extern ldoc_doc_anno_t LDOC_ANNO_NULL;

#ifndef LDOC_NOPYTHON
    
/**
 * @brief Returns a string representation of a Python object (Python call "str(obj)").
 *
 * <strong>Note:</strong> Removed when `LDOC_NOPYTHON` is defined.
 *
 * @param obj Python object.
 * @return Null terminated string representation of `obj`.
 */
char* ldoc_py2str(PyObject* obj);
   
/**
 * @brief Turns a Python dictionary into a LibDocument document.
 *
 * <strong>Note:</strong> Removed when `LDOC_NOPYTHON` is defined.
 *
 * @param dict Python dictionary.
 * @return Document representation of `dict`.
 */
ldoc_doc_t* ldoc_pydict2doc(PyObject* dict);
    
#endif // #ifndef LDOC_NOPYTHON
    
/**
 * @brief Allocates a new serialization object.
 *
 * @param tpe Serialization type (string, integer, float, etc.).
 * @return A new serialization object with type `tpe`, but empty (unassigned) payload.
 */
ldoc_ser_t* ldoc_ser_new(ldoc_serpld_t tpe);

/**
 * @brief Frees the memory of a serialization object.
 *
 * @param ser Serialization object whose memory is being released.
 */
void ldoc_ser_free(ldoc_ser_t* ser);
    
/**
 * @brief Allocates a new node visitor object.
 *
 * @return A new visitor object (functions unassigned).
 */
ldoc_vis_nde_ord_t* ldoc_vis_nde_ord_new();

/**
 * @brief Frees the memory of a node visitor object.
 *
 * @param vis_nde Visitor object whose memory is being released.
 */
void ldoc_vis_nde_ord_free(ldoc_vis_nde_ord_t* vis_nde);

/**
 * @brief Allocates a new entity visitor object.
 *
 * @return A new entity visitor object (functions unassigned).
 */
ldoc_vis_ent_t* ldoc_vis_ent_new();

/**
 * @brief Frees the memory of an entity visitor object.
 *
 * @param vis_ent Entity visitor structor whose memory will be released.
 */
void ldoc_vis_ent_free(ldoc_vis_ent_t* vis_ent);
    
/**
 * @brief Uniformly populates a node visitor object with a single visitor function.
 *
 * @param vis Visitor object whose function pointers are being set to `vis_uni`.
 * @param vis_uni Visitor function that is being assigned to all function pointers in `vis`.
 */
void ldoc_vis_nde_uni(ldoc_vis_nde_t* vis, ldoc_ser_t* (*vis_uni)(ldoc_nde_t* nde, ldoc_coord_t* coord));

/**
 * @brief Uniformly populates an entity visitor object with a single visitor function.
 *
 * @param vis Visitor object whose function pointers are being set to `vis_uni`.
 * @param vis_uni Visitor function that is being assigned to all function pointers in `vis`.
 */
void ldoc_vis_ent_uni(ldoc_vis_ent_t* vis, ldoc_ser_t* (*vis_uni)(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord));

/**
 * @brief Creates a new position object.
 *
 * @param nde Node that is the root for the position object.
 * @param nde_off Offset to the beginning of `nde`.
 * @param off Offset within `nde`.
 * @return A new position object.
 */
ldoc_pos_t* ldoc_pos_new(ldoc_nde_t* nde, uint64_t nde_off, uint64_t off);

/**
 * Releases the memory of a position object.
 *
 * @param pos Position object whose memory is being released.
 */
void ldoc_pos_free(ldoc_pos_t* pos);
    
/**
 * @brief Create a new document.
 *
 * @return An empty document with a root node (`LDOC_NDE_RT`).
 */
ldoc_doc_t* ldoc_doc_new();

/**
 * @brief Releases all allocated memory of a document (including nodes and entities).
 *
 * @param doc Document whose memory is being released.
 */
void ldoc_doc_free(ldoc_doc_t* doc);

/**
 * @brief Creates a new entity object.
 *
 * @param tpe Entity type.
 * @return A new entity object with the payload set to NULL.
 */
ldoc_ent_t* ldoc_ent_new(ldoc_content_t tpe);

/**
 * @brief Frees the memory of an entity object.
 *
 * <strong>Note:</strong> Does not release memory of the payload.
 *
 * @param ent Entity object whose memory is being released.
 */
void ldoc_ent_free(ldoc_ent_t* ent);

/**
 * @brief Creates a new node object.
 *
 * @param tpe Node type.
 * @return A new node object with empty entity list and no descendants.
 */
ldoc_nde_t* ldoc_nde_new(ldoc_struct_t tpe);

/**
 * @brief Deletes a node, its associated entities, and its descendant nodes.
 *
 * <strong>Note:</strong> Does not release memory of the payload.
 *
 * @param nde Node whose memory is being released.
 */
void ldoc_nde_free(ldoc_nde_t* nde);

/**
 * @brief Add an entity to the end of a node's entity list.
 *
 * @param nde Node to which the entity `ent` is appended.
 * @param ent Entity that is appended to the entity list of node `nde`.
 */
void ldoc_nde_ent_push(ldoc_nde_t* nde, ldoc_ent_t* ent);
    
/**
 * @brief Remove an entity at the end of a node's entity list.
 *
 * <div class="alert alert-danger" role="alert">Not implemented yet!</div>
 *
 * @param nde Node from which the entity is removed.
 * @return Entity object that was removed from the node's (`nde`) entity list.
 */
ldoc_ent_t* ldoc_nde_ent_pop(ldoc_nde_t* nde);

/**
 * @brief Add an entity to the beginning of a node's entity list.
 */
void ldoc_nde_ent_shift(ldoc_nde_t* nde, ldoc_ent_t* ent);

/**
 * @brief Remove an entity from the beginning of a node's entity list.
 *
 * <div class="alert alert-danger" role="alert">Not implemented yet!</div>
 *
 * @param nde Node from which the entity is removed.
 * @return Entity object that was removed from the node's (`nde`) entity list.
 */
ldoc_ent_t* ldoc_nde_ent_unshift(ldoc_nde_t* nde);

/**
 * @brief Insert an entity before another entity in a node's entity list.
 *
 * <div class="alert alert-danger" role="alert">Not implemented yet!</div>
 *
 * @param ent Entity object that is being added before `ent_ref` in the entity list of the node to which `ent_ref` belongs to.
 @ @param ent_ref Entity object.
 */
void ldoc_nde_ent_ins(ldoc_ent_t* ent, ldoc_ent_t* ent_ref);

/**
 * @brief Remove an entity from a document tree.
 *
 * @param ent Entity that is being removed from a document tree.
 */
void ldoc_ent_rm(ldoc_ent_t* ent);

/**
 * @brief Add a node to the end of a node's descendant list.
 *
 * @param nde Node object to which node `dsc` is added as a descendant.
 * @param dsc Node object that is being added as a descendant to node `nde`.
 */
void ldoc_nde_dsc_push(ldoc_nde_t* nde, ldoc_nde_t* dsc);

/**
 * @brief Remove a node at the end of a node's descendant list.
 *
 * <div class="alert alert-danger" role="alert">Not implemented yet!</div>
 *
 * @param nde Node object from which the last descendant node is removed.
 * @return Node that was removed from the descendant list.
 */
ldoc_nde_t* ldoc_nde_dsc_pop(ldoc_nde_t* nde);

/**
 * @brief Add an entity to the beginning of a node's descendant list.
 */
void ldoc_nde_dsc_shift(ldoc_nde_t* nde, ldoc_nde_t* dsc);

/**
 * @brief Remove a node from the beginning of a node's descendant list.
 *
 * <div class="alert alert-danger" role="alert">Not implemented yet!</div>
 *
 * @param nde Node object from which the first descendant node is removed.
 * @return Node that was removed from the descendant list.
 */
ldoc_nde_t* ldoc_nde_dsc_unshift(ldoc_nde_t* nde);

/**
 * @brief Insert a node before another entity in a node's descendant list.
 *
 * <div class="alert alert-danger" role="alert">Not implemented yet!</div>
 *
 * @param dsc Node object that is being added before `nde_ref` in the descendant list of the node to which `nde_ref` belongs to.
 * @param nde_ref Node object.
 */
void ldoc_nde_dsc_ins(ldoc_nde_t* dsc, ldoc_nde_t* nde_ref);

/**
 * @brief Remove a node and its descendants from a document tree.
 *
 * @param nde Node that is going to be removed from its document tree.
 */
void ldoc_nde_rm(ldoc_nde_t* nde);

/**
 * @brief Returns the level at which a node is located at.
 *
 * The root node is located at level 0, its immediate descendants are at level 1, and so on.
 *
 * @param nde Node object for which the level is being determined.
 * @return Level of the node `nde`.
 */
uint16_t ldoc_nde_lvl(ldoc_nde_t* nde);
    
/**
 * @brief Formats (serializes) a document using a set of node and entity visitors.
 *
 * @param doc Document that is being serialized.
 * @param vis_nde Node visitors.
 * @param vis_ent Entity visitors.
 * @return Serialization object for document `doc` based on the visitors `vis_nde` and `vis_ent`.
 */
ldoc_ser_t* ldoc_format(ldoc_doc_t* doc, ldoc_vis_nde_ord_t* vis_nde, ldoc_vis_ent_t* vis_ent);

/**
 * @brief Format a document as an object in JSON.
 *
 * @param doc Document that is being serialized as a JSON object.
 * @return Serialization object containing the serialized version of `doc` as JSON object.
 */
ldoc_ser_t* ldoc_format_json(ldoc_doc_t* doc);

/**
 * @brief Find an entity object based on its annotation.
 *
 * <strong>Example:</strong> Searching for an entity annotation "name" in a document `doc`.
 * <pre>
 * ldoc_res_t* res = ldoc_find_anno_ent(doc->rt, "name");
 * // Do something with `res`.
 * ldoc_res_free(res);
 * </pre>
 *
 * @param nde Node object at which the search starts.
 * @param leaf Annotation to search for.
 * @return Result object with the entity that matches the search, or `LDOC_RES_NULL` otherwise.
 */
ldoc_res_t* ldoc_find_anno_ent(ldoc_nde_t* nde, char* leaf);

/**
 * @brief Find a node or entity based on a given search path.
 *
 * <strong>Example:</strong> Searching for an entity annotation "name" in a document `doc`.
 * <pre>
 * const char* pth[] = { "address", "name" };
 * ldoc_res_t* res = ldoc_find_anno_nde(doc->rt, pth, 2);
 * // Do something with `res`.
 * ldoc_res_free(res);
 * </pre>
 *
 * @param nde Node object at which the search starts.
 * @param pth Search path (array of strings).
 * @param plen Length of the search path `pth`.
 * @return Result object with a node or entity that matches the search, or `LDOC_RES_NULL` otherwise.
 */
ldoc_res_t* ldoc_find_anno_nde(ldoc_nde_t* nde, char** pth, size_t plen);
    
/**
 * @brief Find a node or entity in a document based on a given search path.
 *
 * <strong>Example:</strong> Searching for an entity annotation "name" in a document `doc`.
 * <pre>
 * const char* pth[] = { "address", "name" };
 * ldoc_res_t* res = ldoc_find_anno(doc, pth, 2);
 * // Do something with `res`.
 * ldoc_res_free(res);
 * </pre>
 *
 * @param doc Document object that is being searched.
 * @param pth Search path (array of strings).
 * @param plen Length of the search path `pth`.
 * @return Result object with a node or entity that matches the search, or `LDOC_RES_NULL` otherwise.
 */
ldoc_res_t* ldoc_find_anno(ldoc_doc_t* doc, char** pth, size_t plen);

/**
 * @brief Frees a result object.
 *
 * @param res Result object whose memory is being released.
 */
void ldoc_res_free(ldoc_res_t* res);
    
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
 * @brief Setup for HTML serialization.
 *
 * @return A serialization object initialized for HTML serialization.
 */
ldoc_ser_t* ldoc_vis_setup_html(void);

/**
 * @brief Teardown for HTML serialization.
 *
 * @return A serialization object with closing statements for HTML serialization.
 */
ldoc_ser_t* ldoc_vis_teardown_html(void);
    
/**
 * @brief Node visitor for HTML serialization (prefix traversal).
 *
 * @param nde Node object that is being serialized as HTML.
 * @param coord Coordinate object that determines the node's (`nde`) position on a document level.
 * @return A serialization object for HTML serialization.
 */
ldoc_ser_t* ldoc_vis_nde_pre_html(ldoc_nde_t* nde, ldoc_coord_t* coord);

/**
 * @brief Node visitor for HTML serialization (infix traversal).
 *
 * @param nde Node object that is being serialized as HTML.
 * @param coord Coordinate object that determines the node's (`nde`) position on a document level.
 * @return A serialization object for HTML serialization.
 */
ldoc_ser_t* ldoc_vis_nde_infx_html(ldoc_nde_t* nde, ldoc_coord_t* coord);

/**
 * @brief Node visitor for HTML serialization (postfix traversal).
 *
 * @param nde Node object that is being serialized as HTML.
 * @param coord Coordinate object that determines the node's (`nde`) position on a document level.
 * @return A serialization object for HTML serialization.
 */
ldoc_ser_t* ldoc_vis_nde_post_html(ldoc_nde_t* nde, ldoc_coord_t* coord);

/**
 * @brief Entity visitor for HTML serialization.
 *
 * @param nde Node object context in which the entity `ent` is being serialized.
 * @param ent Entity that is being serialized as HTML.
 * @param coord Coordinate object that determines the entity's (`ent`) position on a document level.
 * @return A serialization object for HTML serialization.
 */
ldoc_ser_t* ldoc_vis_ent_html(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord);

//
// JSON
//
    
/**
 * @brief Setup for JSON serialization.
 *
 * @return A serialization object initialized for JSON serialization.
 */
ldoc_ser_t* ldoc_vis_setup_json(void);

/**
 * @brief Teardown for JSON serialization.
 *
 * @return A serialization object with closing statements for JSON serialization.
 */
ldoc_ser_t* ldoc_vis_teardown_json(void);

/**
 * @brief Node visitor for JSON serialization (prefix traversal).
 *
 * @param nde Node object that is being serialized as JSON.
 * @param coord Coordinate object that determines the node's (`nde`) position on a document level.
 * @return A serialization object for JSON serialization.
 */
ldoc_ser_t* ldoc_vis_nde_pre_json(ldoc_nde_t* nde, ldoc_coord_t* coord);

/**
 * @brief Node visitor for JSON serialization (infix traversal).
 *
 * @param nde Node object that is being serialized as JSON.
 * @param coord Coordinate object that determines the node's (`nde`) position on a document level.
 * @return A serialization object for JSON serialization.
 */
ldoc_ser_t* ldoc_vis_nde_infx_json(ldoc_nde_t* nde, ldoc_coord_t* coord);

/**
 * @brief Node visitor for JSON serialization (postfix traversal).
 *
 * @param nde Node object that is being serialized as JSON.
 * @param coord Coordinate object that determines the node's (`nde`) position on a document level.
 * @return A serialization object for JSON serialization.
 */
ldoc_ser_t* ldoc_vis_nde_post_json(ldoc_nde_t* nde, ldoc_coord_t* coord);

/**
 * @brief Entity visitor for JSON serialization.
 *
 * @param nde Node object context in which the entity `ent` is being serialized.
 * @param ent Entity that is being serialized as JSON.
 * @param coord Coordinate object that determines the entity's (`ent`) position on a document level.
 * @return A serialization object for JSON serialization.
 */
ldoc_ser_t* ldoc_vis_ent_json(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord);
    
//
// Python
//

/**
 * @brief Setup for Python object serialization.
 *
 * @return A serialization object initialized for Python object serialization.
 */
ldoc_ser_t* ldoc_vis_setup_py(void);

/**
 * @brief Setup for Python object serialization.
 *
 * @return A serialization object initialized for Python object serialization.
 */
ldoc_ser_t* ldoc_vis_teardown_py(void);

/**
 * @brief Node visitor for Python object serialization (prefix traversal).
 *
 * @param nde Node object that is being serialized as Python object.
 * @param coord Coordinate object that determines the node's (`nde`) position on a document level.
 * @return A serialization object for Python object serialization.
 */
ldoc_ser_t* ldoc_vis_nde_pre_py(ldoc_nde_t* nde, ldoc_coord_t* coord);

/**
 * @brief Node visitor for Python object serialization (infix traversal).
 *
 * @param nde Node object that is being serialized as Python object.
 * @param coord Coordinate object that determines the node's (`nde`) position on a document level.
 * @return A serialization object for Python object serialization.
 */
ldoc_ser_t* ldoc_vis_nde_infx_py(ldoc_nde_t* nde, ldoc_coord_t* coord);

/**
 * @brief Node visitor for Python object serialization (postfix traversal).
 *
 * @param nde Node object that is being serialized as Python object.
 * @param coord Coordinate object that determines the node's (`nde`) position on a document level.
 * @return A serialization object for Python object serialization.
 */
ldoc_ser_t* ldoc_vis_nde_post_py(ldoc_nde_t* nde, ldoc_coord_t* coord);

/**
 * @brief Entity visitor for Python object serialization.
 *
 * @param nde Node object context in which the entity `ent` is being serialized.
 * @param ent Entity that is being serialized as Python object.
 * @param coord Coordinate object that determines the entity's (`ent`) position on a document level.
 * @return A serialization object for Python object serialization.
 */
ldoc_ser_t* ldoc_vis_ent_py(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* defined(__ldoc__document__) */
