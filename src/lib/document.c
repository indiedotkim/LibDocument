/*
 * Copyright (c) 2015 CODAMONO, Ontario, Canada
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "document.h"

ldoc_ser_t* LDOC_SER_NULL = NULL;
ldoc_pos_t* LDOC_POS_NULL = NULL;
ldoc_nde_t* LDOC_NDE_NULL = NULL;
ldoc_ent_t* LDOC_ENT_NULL = NULL;
ldoc_doc_anno_t LDOC_ANNO_NULL = { { NULL }, { NULL } };

static const char* ldoc_cnst_html_doc_opn = "<html>\n  <body>\n";
static const char* ldoc_cnst_html_em1_opn = "<em>";
static const char* ldoc_cnst_html_em2_opn = "<strong>";
static const char* ldoc_cnst_html_h1_opn = "<h1>";
static const char* ldoc_cnst_html_h2_opn = "<h2>";
static const char* ldoc_cnst_html_h3_opn = "<h3>";
static const char* ldoc_cnst_html_h4_opn = "<h4>";
static const char* ldoc_cnst_html_h5_opn = "<h5>";
static const char* ldoc_cnst_html_h6_opn = "<h6>";
static const char* ldoc_cnst_html_par_opn = "<p>";

static const char* ldoc_cnst_html_doc_cls = "  </body>\n</html>\n";
static const char* ldoc_cnst_html_em1_cls = "</em>";
static const char* ldoc_cnst_html_em2_cls = "</strong>";
static const char* ldoc_cnst_html_h1_cls = "</h1>\n";
static const char* ldoc_cnst_html_h2_cls = "</h2>\n";
static const char* ldoc_cnst_html_h3_cls = "</h3>\n";
static const char* ldoc_cnst_html_h4_cls = "</h4>\n";
static const char* ldoc_cnst_html_h5_cls = "</h5>\n";
static const char* ldoc_cnst_html_h6_cls = "</h6>\n";
static const char* ldoc_cnst_html_par_cls = "</p>\n";

static const char* ldoc_cnst_json_opn = "{";
static const char* ldoc_cnst_json_lopn = "[";

static const char* ldoc_cnst_json_nde = "NDE";

static const char* ldoc_cnst_json_bl = "BOOL";
static const char* ldoc_cnst_json_em1 = "EM1";
static const char* ldoc_cnst_json_em2 = "EM2";
static const char* ldoc_cnst_json_h1 = "H1";
static const char* ldoc_cnst_json_h2 = "H2";
static const char* ldoc_cnst_json_h3 = "H3";
static const char* ldoc_cnst_json_h4 = "H4";
static const char* ldoc_cnst_json_h5 = "H5";
static const char* ldoc_cnst_json_h6 = "H6";
static const char* ldoc_cnst_json_num = "NUM";
static const char* ldoc_cnst_json_par = "PAR";
static const char* ldoc_cnst_json_txt = "TXT";

static const char* ldoc_cnst_json_false = "false";
static const char* ldoc_cnst_json_true = "true";
static const char* ldoc_cnst_json_null = "null";

static const char* ldoc_cnst_json_cls = "}";
static const char* ldoc_cnst_json_lcls = "]";

static ldoc_nde_t* ldoc_pydict2doc_dict(PyObject* lbl, PyObject* dict);
static ldoc_nde_t* ldoc_pydict2doc_lst(PyObject* lbl, PyObject* dict);

char* ldoc_py2str(PyObject* obj)
{
    PyObject* str = PyObject_Str(obj);
    
    char* s = strdup(PyUnicode_AsUTF8(str));
    
    Py_DECREF(str);
    
    return s;
}

inline static ldoc_ent_t* ldoc_pydict2doc_str(PyObject* str)
{
    ldoc_ent_t* ent = ldoc_ent_new(LDOC_ENT_TXT);
    
    ent->pld.str = strdup(PyUnicode_AsUTF8(str));
    
    return ent;
}

inline static ldoc_ent_t* ldoc_pydict2doc_num(PyObject* str)
{
    ldoc_ent_t* ent = ldoc_ent_new(LDOC_ENT_NUM);
    
    ent->pld.str = strdup(PyUnicode_AsUTF8(PyObject_Str(str)));
    
    return ent;
}

inline static ldoc_ent_t* ldoc_pydict2doc_bl(PyObject* str)
{
    ldoc_ent_t* ent = ldoc_ent_new(LDOC_ENT_BL);
    
    if (str == Py_True)
        ent->pld.bl = true;
    else
        ent->pld.bl = false;
    
    return ent;
}

inline static ldoc_ent_t* ldoc_pydict2doc_anno(PyObject* lbl, ldoc_content_t tpe, PyObject* obj)
{
    ldoc_ent_t* ent = ldoc_ent_new(tpe);
    
    if (lbl)
    {
        char* clbl = strdup(PyUnicode_AsUTF8(lbl));
        ent->pld.pair.anno.str = clbl;
    }
    
    PyObject* tmp;
    switch (tpe)
    {
        case LDOC_ENT_BR:
            if (obj == Py_True)
                ent->pld.pair.dtm.bl = true;
            else
                ent->pld.pair.dtm.bl = false;
            break;
        case LDOC_ENT_NR:
            tmp = PyObject_Str(obj);
            ent->pld.pair.dtm.str = strdup(PyUnicode_AsUTF8(tmp));
            Py_DECREF(tmp);
            break;
        case LDOC_ENT_OR:
            ent->pld.pair.dtm.str = strdup(PyUnicode_AsUTF8(obj));
            break;
        default:
            // TODO Internal error.
            ent = NULL;
            break;
    }
    
    return ent;
}

inline static ldoc_nde_t* ldoc_pydict2doc_lst(PyObject* lbl, PyObject* lst)
{
    ldoc_nde_t* nde = ldoc_nde_new(LDOC_NDE_OL);
    
    if (lbl)
    {
        char* clbl = strdup(PyUnicode_AsUTF8(lbl));
        nde->mkup.anno.str = clbl;
    }
    
    Py_ssize_t pos = 0;
    Py_ssize_t len = PyList_Size(lst);

    PyObject* val;
    ldoc_ent_t* ent = NULL;
    ldoc_nde_t* dsc = NULL;
    while (pos < len)
    {
        val = PyList_GetItem(lst, pos++);
        
        if (PyDict_CheckExact(val))
        {
            dsc = ldoc_pydict2doc_dict(NULL, val);
            
            ldoc_nde_dsc_push(nde, dsc);
        }
        else if (PyList_CheckExact(val))
        {
            dsc = ldoc_pydict2doc_lst(NULL, val);
            
            ldoc_nde_dsc_push(nde, dsc);
        }
        else if (PyUnicode_CheckExact(val))
        {
            ent = ldoc_pydict2doc_str(val);
            
            ldoc_nde_ent_push(nde, ent);
        }
        else if (PyLong_CheckExact(val) || PyFloat_CheckExact(val))
        {
            ent = ldoc_pydict2doc_num(val);
            
            ldoc_nde_ent_push(nde, ent);
        }
        else if (PyBool_Check(val))
        {
            ent = ldoc_pydict2doc_bl(val);
            
            ldoc_nde_ent_push(nde, ent);
        }
        else
        {
            // TODO Passed custom type -- which is not supported.
        }
    }
    
    return nde;
}

inline static ldoc_nde_t* ldoc_pydict2doc_dict(PyObject* lbl, PyObject* dict)
{
    ldoc_nde_t* nde = ldoc_nde_new(LDOC_NDE_UA);
    
    if (lbl)
    {
        char* clbl = strdup(PyUnicode_AsUTF8(lbl));
        nde->mkup.anno.str = clbl;
    }
    
    PyObject* ky;
    PyObject* val;
    Py_ssize_t pos = 0;
    
    ldoc_ent_t* ent = NULL;
    ldoc_nde_t* dsc = NULL;
    while (PyDict_Next(dict, &pos, &ky, &val))
    {
        if (PyDict_CheckExact(val))
        {
            dsc = ldoc_pydict2doc_dict(ky, val);
            
            ldoc_nde_dsc_push(nde, dsc);
        }
        else if (PyList_CheckExact(val))
        {
            dsc = ldoc_pydict2doc_lst(ky, val);
            
            ldoc_nde_dsc_push(nde, dsc);
        }
        else if (PyUnicode_CheckExact(val))
        {
            ent = ldoc_pydict2doc_anno(ky, LDOC_ENT_OR, val);
            
            ldoc_nde_ent_push(nde, ent);
        }
        else if (PyLong_CheckExact(val) || PyFloat_CheckExact(val))
        {
            ent = ldoc_pydict2doc_anno(ky, LDOC_ENT_NR, val);
            
            ldoc_nde_ent_push(nde, ent);
        }
        else if (PyBool_Check(val))
        {
            ent = ldoc_pydict2doc_anno(ky, LDOC_ENT_BR, val);
            
            ldoc_nde_ent_push(nde, ent);
        }
        else
        {
            // TODO Passed custom type -- which is not supported.
        }
    }
    
    return nde;
}

ldoc_doc_t* ldoc_pydict2doc(PyObject* dict)
{
    ldoc_doc_t* doc = ldoc_doc_new();
    
    free(doc->rt);
    doc->rt = ldoc_pydict2doc_dict(NULL, dict);
    doc->rt->tpe = LDOC_NDE_RT;
    
    return doc;
}

static inline bool ldoc_isfloat(char* str)
{
    bool flt = false;
    
    while (*str)
    {
        if (*(str++) == '.')
            return true;
    }
    
    return flt;
}

inline ldoc_ser_t* ldoc_ser_new(ldoc_serpld_t tpe)
{
    ldoc_ser_t* ser = (ldoc_ser_t*)malloc(sizeof(ldoc_ser_t));
    
    // TODO Error handling.
    
    ser->tpe = tpe;
    
    return ser;
}

static inline ldoc_nde_t* ldoc_nde_new_(ldoc_struct_t tpe)
{
    ldoc_nde_t* nde = (ldoc_nde_t*)malloc(sizeof(ldoc_nde_t));
    
    if (!nde)
    {
        // TODO
        return LDOC_NDE_NULL;
    }
    
    nde->tpe = tpe;
    nde->prnt = LDOC_NDE_NULL;
    nde->mkup = LDOC_ANNO_NULL;
    nde->ent_cnt = 0;
    nde->dsc_cnt = 0;
    TAILQ_INIT(&(nde->ents));
    TAILQ_INIT(&(nde->dscs));
    
    return nde;
}

static inline void ldoc_ser_concat_str(ldoc_ser_t* ser1, ldoc_ser_t* ser2)
{
    size_t len2 = strlen(ser2->pld.str);
    
    // Nothing to concatenate:
    if (!len2)
        return;
    
    size_t len1 = strlen(ser1->pld.str);
    size_t len = len1 + len2 + 1;
    
    ser1->pld.str = realloc(ser1->pld.str, len);
    
    strncat(ser1->pld.str, ser2->pld.str, len2);
}

static inline void ldoc_ser_concat_py_str(ldoc_ser_t* ser1, ldoc_ser_t* ser2)
{
    PyObject* ccat = PyUnicode_Concat(ser1->pld.py.dtm, ser1->pld.py.dtm);
    
    ser1->pld.py.dtm = ccat;
}

static inline void ldoc_ser_concat_py_dct(ldoc_ser_t* ser1, ldoc_ser_t* ser2)
{
    switch (ser2->tpe)
    {
        case LDOC_SER_PY_BL:
        case LDOC_SER_PY_FLT:
        case LDOC_SER_PY_STR:
        case LDOC_SER_PY_DCT:
        case LDOC_SER_PY_LST:
            PyDict_SetItem(ser1->pld.py.dtm, ser2->pld.py.anno, ser2->pld.py.dtm);
            break;
        default:
            // TODO Internal error.
            break;
    }
}

static inline void ldoc_ser_concat_py_lst(ldoc_ser_t* ser1, ldoc_ser_t* ser2)
{
    switch (ser2->tpe)
    {
        case LDOC_SER_PY_BL:
        case LDOC_SER_PY_FLT:
        case LDOC_SER_PY_STR:
        case LDOC_SER_PY_DCT:
        case LDOC_SER_PY_LST:
            PyList_Append(ser1->pld.py.dtm, ser2->pld.py.dtm);
            break;
        default:
            // TODO Internal error.
            break;
    }
}

void ldoc_ser_concat(ldoc_ser_t* ser1, ldoc_ser_t* ser2)
{
    // Nothing to concatenate:
    if (ser2 == LDOC_SER_NULL)
        return;
    
    // TODO Only strings supported right now.
    
    switch (ser1->tpe)
    {
        case LDOC_SER_PY_BL:
        case LDOC_SER_PY_FLT:
        case LDOC_SER_PY_INT:
            // Things that cannot be concatenated.
            
            // TODO Internal error.
            
            break;
        case LDOC_SER_PY_STR:
            if (ser2->tpe == LDOC_SER_PY_STR)
                ldoc_ser_concat_py_str(ser1, ser2);
            else
            {
                // TODO Internal error. Second object is not a string.
            }
        case LDOC_SER_PY_DCT:
            ldoc_ser_concat_py_dct(ser1, ser2);
            break;
        case LDOC_SER_PY_LST:
            ldoc_ser_concat_py_lst(ser1, ser2);
            break;
        case LDOC_SER_CSTR:
            return ldoc_ser_concat_str(ser1, ser2);
        default:
            break;
    }
}

ldoc_pos_t* ldoc_pos_new(ldoc_nde_t* nde, uint64_t nde_off, uint64_t off)
{
    ldoc_pos_t* pos = (ldoc_pos_t*)malloc(sizeof(ldoc_pos_t));
    
    if (!pos)
    {
        // TODO
    }
    
    pos->nde = nde;
    pos->nde_off = nde_off;
    pos->off = off;
    
    return pos;
}

void ldoc_pos_free(ldoc_pos_t* pos)
{
    free(pos);
}

ldoc_doc_t* ldoc_doc_new()
{
    ldoc_doc_t* doc = (ldoc_doc_t*)malloc(sizeof(ldoc_doc_t));
    
    if (!doc)
    {
        // TODO Error.
    }
    
    ldoc_nde_t* rt = ldoc_nde_new_(LDOC_NDE_RT);
    
    if (!rt)
    {
        free(doc);
        // TODO Error.
    }
    
    doc->rt = rt;
    
    return doc;
}

void ldoc_doc_free(ldoc_doc_t* doc)
{
    ldoc_nde_free(doc->rt);
    
    free(doc);
}

ldoc_vis_nde_ord_t* ldoc_vis_nde_ord_new()
{
    ldoc_vis_nde_ord_t* vis = (ldoc_vis_nde_ord_t*)malloc(sizeof(ldoc_vis_nde_ord_t));
    
    if (!vis)
    {
        // TODO.
    }
    
    // Assign NULL pointers to all callbacks:
    memset(vis, 0, sizeof(ldoc_vis_nde_ord_t));
    
    return vis;
}

ldoc_vis_ent_t* ldoc_vis_ent_new()
{
    ldoc_vis_ent_t* vis = (ldoc_vis_ent_t*)malloc(sizeof(ldoc_vis_ent_t));
    
    if (!vis)
    {
        // TODO.
    }
    
    // Assign NULL pointers to all callbacks:
    memset(vis, 0, sizeof(ldoc_vis_ent_t));
    
    return vis;
}

void ldoc_vis_nde_uni(ldoc_vis_nde_t* vis, ldoc_ser_t* (*vis_uni)(ldoc_nde_t* nde, ldoc_coord_t* coord))
{
    vis->vis_anc = vis_uni;
    vis->vis_h1 = vis_uni;
    vis->vis_h2 = vis_uni;
    vis->vis_h3 = vis_uni;
    vis->vis_h4 = vis_uni;
    vis->vis_h5 = vis_uni;
    vis->vis_h6 = vis_uni;
    vis->vis_nn = vis_uni;
    vis->vis_ol = vis_uni;
    vis->vis_oo = vis_uni;
    vis->vis_par = vis_uni;
    vis->vis_rt = vis_uni;
    vis->vis_ua = vis_uni;
    vis->vis_ul = vis_uni;
}


void ldoc_vis_ent_uni(ldoc_vis_ent_t* vis, ldoc_ser_t* (*vis_uni)(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord))
{
    vis->vis_bl = vis_uni;
    vis->vis_br = vis_uni;
    vis->vis_em1 = vis_uni;
    vis->vis_em2 = vis_uni;
    vis->vis_num = vis_uni;
    vis->vis_nr = vis_uni;
    vis->vis_or = vis_uni;
    vis->vis_ref = vis_uni;
    vis->vis_txt = vis_uni;
    vis->vis_uri = vis_uni;
}

static inline ldoc_ser_t* ldoc_vis_ent(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord, ldoc_vis_ent_t* vis)
{
    switch (ent->tpe)
    {
        case LDOC_ENT_BL:
            return vis->vis_bl(nde, ent, coord);
        case LDOC_ENT_BR:
            return vis->vis_br(nde, ent, coord);
        case LDOC_ENT_EM1:
            return vis->vis_em1(nde, ent, coord);
        case LDOC_ENT_EM2:
            return vis->vis_em2(nde, ent, coord);
        case LDOC_ENT_NUM:
            return vis->vis_num(nde, ent, coord);
        case LDOC_ENT_NR:
            return vis->vis_nr(nde, ent, coord);
        case LDOC_ENT_OR:
            return vis->vis_or(nde, ent, coord);
        case LDOC_ENT_REF:
            return vis->vis_ref(nde, ent, coord);
        case LDOC_ENT_TXT:
            return vis->vis_txt(nde, ent, coord);
        case LDOC_ENT_URI:
            return vis->vis_uri(nde, ent, coord);
        default:
            // TODO Error.
            break;
    }
    
    return LDOC_SER_NULL;
}

char* ldoc_qry_ent_fst(ldoc_nde_t* nde)
{
    if (!nde->ent_cnt)
    {
        // TODO Error.
    }
    
    ldoc_ent_t* ent = TAILQ_FIRST(&(nde->ents));
    size_t str_len = strlen(ent->pld.str);
    char* str = (char*)malloc(str_len + 1);
    strncpy(str, ent->pld.str, str_len);
    str[str_len] = 0;
    
    if (!str)
    {
        // TODO Error.
    }
    
    return str;
}

char* ldoc_qry_ent_unq(ldoc_nde_t* nde)
{
    if (nde->ent_cnt != 1)
    {
        // TODO Error.
    }

    return ldoc_qry_ent_fst(nde);
}

char* ldoc_cnv_ent_html(ldoc_ent_t* ent)
{
    char* html;
    size_t html_len;
    
    switch (ent->tpe) {
        case LDOC_ENT_BL:
            // TODO
            break;
        case LDOC_ENT_BR:
            // TODO
            break;
        case LDOC_ENT_EM1:
            html_len = strlen(ldoc_cnst_html_em1_opn) + strlen(ent->pld.str) + strlen(ldoc_cnst_html_em1_cls) + 1;
            html = (char*)malloc(html_len + 1);
            snprintf(html, html_len, "%s%s%s", ldoc_cnst_html_em1_opn, ent->pld.str, ldoc_cnst_html_em1_cls);
            return html;
        case LDOC_ENT_EM2:
            html_len = strlen(ldoc_cnst_html_em2_opn) + strlen(ent->pld.str) + strlen(ldoc_cnst_html_em2_cls) + 1;
            html = (char*)malloc(html_len + 1);
            snprintf(html, html_len, "%s%s%s", ldoc_cnst_html_em2_opn, ent->pld.str, ldoc_cnst_html_em2_cls);
            return html;
        case LDOC_ENT_NUM:
            // TODO
            break;
        case LDOC_ENT_NR:
            // TODO
            break;
        case LDOC_ENT_OR:
            // TODO
            break;
        case LDOC_ENT_REF:
            // TODO
            break;
        case LDOC_ENT_TXT:
            html_len = strlen(ent->pld.str);
            html = (char*)malloc(html_len + 1);
            strncpy(html, ent->pld.str, html_len);
            html[html_len] = 0;
            return html;
        case LDOC_ENT_URI:
            // TODO
            break;
        default:
            // TODO Error.
            break;
    }
    
    // Note; Hardly optimal, but currently no idea how to
    //       implement it better.
    html = (char*)calloc(1, 1);
    
    return html;
}

char* ldoc_cnv_nde_html_opn(ldoc_nde_t* nde)
{
    char* pld;
    char* html;
    size_t html_len;
    
    switch (nde->tpe) {
        case LDOC_NDE_ANC:
            pld = ldoc_qry_ent_unq(nde);
            html_len = 15 + strlen(pld) + 1;
            html = (char*)malloc(html_len + 1);
            snprintf(html, html_len, "<a name=\"%s\"></a>", pld);
            free(pld);
            return html;
        case LDOC_NDE_H1:
            html_len = strlen(ldoc_cnst_html_h1_opn);
            html = (char*)malloc(html_len + 1);
            strcpy(html, ldoc_cnst_html_h1_opn);
            return html;
        case LDOC_NDE_H2:
            html_len = strlen(ldoc_cnst_html_h2_opn);
            html = (char*)malloc(html_len + 1);
            strcpy(html, ldoc_cnst_html_h2_opn);
            return html;
        case LDOC_NDE_H3:
            html_len = strlen(ldoc_cnst_html_h3_opn);
            html = (char*)malloc(html_len + 1);
            strcpy(html, ldoc_cnst_html_h3_opn);
            return html;
        case LDOC_NDE_H4:
            html_len = strlen(ldoc_cnst_html_h4_opn);
            html = (char*)malloc(html_len + 1);
            strcpy(html, ldoc_cnst_html_h4_opn);
            return html;
        case LDOC_NDE_H5:
            html_len = strlen(ldoc_cnst_html_h5_opn);
            html = (char*)malloc(html_len + 1);
            strcpy(html, ldoc_cnst_html_h5_opn);
            return html;
        case LDOC_NDE_H6:
            html_len = strlen(ldoc_cnst_html_h6_opn);
            html = (char*)malloc(html_len + 1);
            strcpy(html, ldoc_cnst_html_h6_opn);
            return html;
        case LDOC_NDE_NN:
            break;
        case LDOC_NDE_OL:
            break;
        case LDOC_NDE_OO:
            break;
        case LDOC_NDE_PAR:
            html_len = strlen(ldoc_cnst_html_par_opn);
            html = (char*)malloc(html_len + 1);
            strcpy(html, ldoc_cnst_html_par_opn);
            return html;
        case LDOC_NDE_RT:
            break;
        case LDOC_NDE_UA:
            break;
        case LDOC_NDE_UL:
            break;
        default:
            // TODO Error.
            break;
    }
    
    // Note; Hardly optimal, but currently no idea how to
    //       implement it better.
    html = (char*)calloc(1, 1);
    
    return html;
}

char* ldoc_cnv_nde_html_cls(ldoc_nde_t* nde)
{
    char* html;
    size_t html_len;
    
    switch (nde->tpe) {
        case LDOC_NDE_ANC:
            break;
        case LDOC_NDE_H1:
            html_len = strlen(ldoc_cnst_html_h1_cls);
            html = (char*)malloc(html_len + 1);
            strcpy(html, ldoc_cnst_html_h1_cls);
            return html;
        case LDOC_NDE_H2:
            html_len = strlen(ldoc_cnst_html_h2_cls);
            html = (char*)malloc(html_len + 1);
            strcpy(html, ldoc_cnst_html_h2_cls);
            return html;
        case LDOC_NDE_H3:
            html_len = strlen(ldoc_cnst_html_h3_cls);
            html = (char*)malloc(html_len + 1);
            strcpy(html, ldoc_cnst_html_h3_cls);
            return html;
        case LDOC_NDE_H4:
            html_len = strlen(ldoc_cnst_html_h4_cls);
            html = (char*)malloc(html_len + 1);
            strcpy(html, ldoc_cnst_html_h4_cls);
            return html;
        case LDOC_NDE_H5:
            html_len = strlen(ldoc_cnst_html_h5_cls);
            html = (char*)malloc(html_len + 1);
            strcpy(html, ldoc_cnst_html_h5_cls);
            return html;
        case LDOC_NDE_H6:
            html_len = strlen(ldoc_cnst_html_h6_cls);
            html = (char*)malloc(html_len + 1);
            strcpy(html, ldoc_cnst_html_h6_cls);
            return html;
        case LDOC_NDE_NN:
            break;
        case LDOC_NDE_OL:
            break;
        case LDOC_NDE_OO:
            break;
        case LDOC_NDE_PAR:
            html_len = strlen(ldoc_cnst_html_par_cls);
            html = (char*)malloc(html_len + 1);
            strcpy(html, ldoc_cnst_html_par_cls);
            return html;
        case LDOC_NDE_UA:
            break;
        case LDOC_NDE_UL:
            break;
        default:
            // TODO Error.
            break;
    }
    
    // Note; Hardly optimal, but currently no idea how to
    //       implement it better.
    html = (char*)calloc(1, 1);
    
    return html;
}

static inline void ldoc_strcat(ldoc_ser_t* ser, char* s1)
{
    size_t len1;
    
    if (!s1)
        len1 = 0;
    else
        len1 = strlen(s1);
    
    ser->pld.str = (char*)malloc(len1 + 1);
    
    // If you do not know why the next line needs to be there, then
    // you have not read the man page of strncpy.
    ser->pld.str[len1] = 0;
    
    if (len1)
        strncpy(ser->pld.str, s1, len1);
}

static inline void ldoc_strcat2(ldoc_ser_t* ser, char* s1, char* s2)
{
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);

    ser->pld.str = (char*)malloc(len1 + len2 + 1);
    
    // If you do not know why the next line needs to be there, then
    // you have not read the man page of strncpy.
    ser->pld.str[len1] = 0;
    strncpy(ser->pld.str, s1, len1);
    strncat(ser->pld.str, s2, len2);
}

static inline void ldoc_strcat3(ldoc_ser_t* ser, char* s1, char* s2, char* s3)
{
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    size_t len3 = strlen(s3);
    ser->pld.str = (char*)malloc(len1 + len2 + len3 + 1);

    // If you do not know why the next line needs to be there, then
    // you have not read the man page of strncpy.
    ser->pld.str[len1] = 0;
    strncpy(ser->pld.str, s1, len1);
    strncat(ser->pld.str, s2, len2);
    strncat(ser->pld.str, s3, len3);
}

ldoc_ser_t* ldoc_vis_ent_html(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord)
{
    // TODO Only strings supported right now.
    ldoc_ser_t* ser = ldoc_ser_new(LDOC_SER_CSTR);
    ser->pld.str = ldoc_cnv_ent_html(ent);
    
    return ser;
}

ldoc_ser_t* ldoc_vis_setup_html(void)
{
    ldoc_ser_t* ser = ldoc_ser_new(LDOC_SER_CSTR);
    
    ser->pld.str = (char*)malloc(strlen(ldoc_cnst_html_doc_opn) + 1);
    strcpy(ser->pld.str, ldoc_cnst_html_doc_opn);
    
    return ser;
}

ldoc_ser_t* ldoc_vis_teardown_html(void)
{
    ldoc_ser_t* ser = ldoc_ser_new(LDOC_SER_CSTR);
    
    ser->pld.str = (char*)malloc(strlen(ldoc_cnst_html_doc_cls) + 1);
    strcpy(ser->pld.str, ldoc_cnst_html_doc_cls);
    
    return ser;
}

ldoc_ser_t* ldoc_vis_nde_pre_html(ldoc_nde_t* nde, ldoc_coord_t* coord)
{
    size_t idnt_len = coord->lvl ? 2 * (coord->lvl) + 2 : 0;
    char* idnt = (char*)malloc(idnt_len + 1);
    
    memset(idnt, ' ', idnt_len);
    idnt[idnt_len] = 0;
    
    char* html = ldoc_cnv_nde_html_opn(nde);
    
    ldoc_ser_t* ser = ldoc_ser_new(LDOC_SER_CSTR);
    ldoc_strcat2(ser, idnt, html);
    
    return ser;
}

static ldoc_ser_t* ldoc_vis_nde_cls_html(ldoc_nde_t* nde, ldoc_coord_t* coord, bool indent)
{
    char* idnt = NULL;
    if (indent)
    {
        size_t idnt_len = coord->lvl ? 2 * (coord->lvl) + 2 : 0;
        char* idnt = (char*)malloc(idnt_len + 1);
        memset(idnt, ' ', idnt_len);
        idnt[idnt_len] = 0;
    }
    
    char* html = ldoc_cnv_nde_html_cls(nde);
    
    ldoc_ser_t* ser = ldoc_ser_new(LDOC_SER_CSTR);
    
    if (idnt)
        ldoc_strcat2(ser, idnt, html);
    else
        ldoc_strcat(ser, html);
    
    return ser;
}

ldoc_ser_t* ldoc_vis_nde_infx_html(ldoc_nde_t* nde, ldoc_coord_t* coord)
{
    switch (nde->tpe) {
        case LDOC_NDE_PAR:
            // Will be handled in post call.
            return LDOC_SER_NULL;
        default:
            // Fall through for all other cases.
            break;
    }
    
    return ldoc_vis_nde_cls_html(nde, coord, false);
}

ldoc_ser_t* ldoc_vis_nde_post_html(ldoc_nde_t* nde, ldoc_coord_t* coord)
{
    switch (nde->tpe) {
        case LDOC_NDE_H1:
        case LDOC_NDE_H2:
        case LDOC_NDE_H3:
        case LDOC_NDE_H4:
        case LDOC_NDE_H5:
        case LDOC_NDE_H6:
            // Already closed in infix call.
            return LDOC_SER_NULL;
        default:
            // Fall through for all other cases.
            break;
    }
    
    return ldoc_vis_nde_cls_html(nde, coord, true);
}

ldoc_ser_t* ldoc_vis_setup_json(void)
{
    ldoc_ser_t* ser = ldoc_ser_new(LDOC_SER_CSTR);
    
    ser->pld.str = (char*)malloc(strlen(ldoc_cnst_json_opn) + 1);
    strcpy(ser->pld.str, ldoc_cnst_json_opn);
    
    return ser;
}

ldoc_ser_t* ldoc_vis_teardown_json(void)
{
    ldoc_ser_t* ser = ldoc_ser_new(LDOC_SER_CSTR);
    
    ser->pld.str = (char*)malloc(strlen(ldoc_cnst_json_cls) + 1);
    strcpy(ser->pld.str, ldoc_cnst_json_cls);
    
    return ser;
}

ldoc_ser_t* ldoc_vis_nde_pre_json(ldoc_nde_t* nde, ldoc_coord_t* coord)
{
    ldoc_ser_t* ser;
    
    if (!coord->lvl)
    {
        ser = ldoc_ser_new(LDOC_SER_CSTR);
        
        // TODO Error handling.
        
        ser->pld.str = (char*)calloc(1, 1);
        
        // TODO Error handling.
        
        return ser;
    }
    
    char sep[2];
    
    if (coord->pln > 0 || nde->prnt->ent_cnt)
    {
        sep[0] = ',';
        sep[1] = 0;
    }
    else
    {
        sep[0] = 0;
    }
    
    size_t lbl_len;
    char* lbl;
    
    if (nde->prnt->tpe == LDOC_NDE_OL)
        lbl = "";
    else if (nde->mkup.anno.str != NULL)
    {
        lbl_len = strlen(nde->mkup.anno.str);
        lbl = nde->mkup.anno.str;
    }
    else
    {
        size_t lbl_len = strlen(ldoc_cnst_json_nde) + 17;
        lbl = (char*)malloc(lbl_len + 1);
        snprintf(lbl, lbl_len, "%s-%llx", ldoc_cnst_json_nde, (unsigned long long)nde);
    }
    
    const char* opn;
    if (nde->tpe == LDOC_NDE_OL)
        opn = ldoc_cnst_json_lopn;
    else
        opn = ldoc_cnst_json_opn;
    
    ser = ldoc_ser_new(LDOC_SER_CSTR);
    size_t ser_len = strlen(lbl) + 5 + 1;
    
    //if (nde->prnt->tpe == LDOC_NDE_OL)
    //    ser_len++;
    
    ser->pld.str = (char*)malloc(ser_len + 1);
    
    // TODO Error handling.
    
    // If we are in an ordered list, then do not include a node's label:
    if (nde->prnt->tpe == LDOC_NDE_OL)
        snprintf(ser->pld.str, ser_len, "%s%s", sep, opn);
    else
        snprintf(ser->pld.str, ser_len, "%s\"%s\":%s", sep, lbl, opn);
    
    return ser;
}

ldoc_ser_t* ldoc_vis_nde_infx_json(ldoc_nde_t* nde, ldoc_coord_t* coord)
{
    return LDOC_SER_NULL;
}

ldoc_ser_t* ldoc_vis_nde_post_json(ldoc_nde_t* nde, ldoc_coord_t* coord)
{
    char* json;
    
    if (!coord->lvl)
    {
        json = (char*)calloc(1, 1);
    }
    else if (nde->tpe == LDOC_NDE_OL)
    {
        json = strdup(ldoc_cnst_json_lcls);
    }
    else
    {
        json = strdup(ldoc_cnst_json_cls);
    }
    
    ldoc_ser_t* ser = ldoc_ser_new(LDOC_SER_CSTR);
    
    if (nde->prnt && nde->prnt->tpe == LDOC_NDE_OL)
    {
        size_t clen = strlen(json) + 1;
        ser->pld.str = (char*)malloc(clen + 1);
        
        // TODO Error handling.
        
        snprintf(ser->pld.str, clen, "%s", json);
        
        free(json);
    }
    else
        ser->pld.str = json;
    
    return ser;
}

char* ldoc_vis_ent_json_val(ldoc_ent_t* ent, ldoc_coord_t* coord, size_t* len)
{
    size_t val_len;
    char* val;
    
    if ((ent->tpe == LDOC_ENT_BR && !ent->pld.pair.dtm.str) ||
        (ent->tpe == LDOC_ENT_NR && !ent->pld.pair.dtm.str) ||
        (ent->tpe == LDOC_ENT_OR && !ent->pld.pair.dtm.str) ||
        (ent->tpe != LDOC_ENT_OR && !ent->pld.str))
    {
        val_len = strlen(ldoc_cnst_json_null);
        val = strdup(ldoc_cnst_json_null);
    }
    else
        // TODO Check why val_len is one larger than required (???) some times.
        switch (ent->tpe)
        {
            case LDOC_ENT_BL:
                if (ent->pld.bl)
                    val = strdup(ldoc_cnst_json_true);
                else
                    val = strdup(ldoc_cnst_json_false);
                val_len = strlen(val);
                break;
            case LDOC_ENT_BR:
                if (ent->pld.pair.dtm.bl)
                    val = strdup(ldoc_cnst_json_true);
                else
                    val = strdup(ldoc_cnst_json_false);
                val_len = strlen(val);
                break;
            case LDOC_ENT_NUM:
                val_len = strlen(ent->pld.str);
                val = strdup(ent->pld.str);
                break;
            case LDOC_ENT_NR:
                val_len = strlen(ent->pld.pair.dtm.str) + 1;
                val = strdup(ent->pld.pair.dtm.str);
                break;
            case LDOC_ENT_OR:
                val_len = strlen(ent->pld.pair.dtm.str) + 3;
                val = (char*)malloc(val_len + 1);
                snprintf(val, val_len, "\"%s\"", ent->pld.pair.dtm.str);
                break;
            default:
                val_len = strlen(ent->pld.str) + 3;
                val = (char*)malloc(val_len + 1);
                snprintf(val, val_len, "\"%s\"", ent->pld.str);
                break;
        }
    
    if (!val)
    {
        // TODO Error handling.
        *len = 0;
    }
    
    *len = val_len;
    
    return val;
}

ldoc_ser_t* ldoc_vis_ent_json(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord)
{
    size_t json_len;
    char* json = ldoc_vis_ent_json_val(ent, coord, &json_len);
    
    ldoc_ser_t* ser = ldoc_ser_new(LDOC_SER_CSTR);
    
    if (!ser)
    {
        // TODO Error handling.
    }
    
    size_t lbl_len = 0;
    char* lbl = NULL;
    if (nde->tpe == LDOC_NDE_OL)
        switch (ent->tpe)
        {
            case LDOC_ENT_BR:
            case LDOC_ENT_NR:
            case LDOC_ENT_OR:
                lbl_len = strlen(ent->pld.pair.anno.str) + json_len + 6;
                lbl = (char*)malloc(lbl_len + 1);
                // TODO Error handling.
                snprintf(lbl, lbl_len, "{\"%s\":%s}", ent->pld.pair.anno.str, json);
                free(json);
                json_len = 0;
                json = NULL;
                break;
            default:
                break;
        }
    else
        switch (ent->tpe)
        {
            case LDOC_ENT_BL:
                lbl_len = strlen(ldoc_cnst_json_bl) + 5 + 1;
                lbl = (char*)malloc(lbl_len);
                // TODO Error handling.
                snprintf(lbl, lbl_len, "\"%s-%llx\":", ldoc_cnst_json_bl, (unsigned long long)ent);
                break;
            case LDOC_ENT_EM1:
                lbl_len = strlen(ldoc_cnst_json_em1) + 20 + 1;
                lbl = (char*)malloc(lbl_len);
                // TODO Error handling.
                snprintf(lbl, lbl_len, "\"%s-%llx\":", ldoc_cnst_json_em1, (unsigned long long)ent);
                break;
            case LDOC_ENT_EM2:
                lbl_len = strlen(ldoc_cnst_json_em2) + 20 + 1;
                lbl = (char*)malloc(lbl_len);
                // TODO Error handling.
                snprintf(lbl, lbl_len, "\"%s-%llx\":", ldoc_cnst_json_em2, (unsigned long long)ent);
                break;
            case LDOC_ENT_NUM:
                lbl_len = strlen(ldoc_cnst_json_num) + 20 + 1;
                lbl = (char*)malloc(lbl_len);
                // TODO Error handling.
                snprintf(lbl, lbl_len, "\"%s-%llx\":", ldoc_cnst_json_num, (unsigned long long)ent);
                break;
            case LDOC_ENT_BR:
            case LDOC_ENT_NR:
            case LDOC_ENT_OR:
                lbl_len = strlen(ent->pld.pair.anno.str) + 4;
                lbl = (char*)malloc(lbl_len + 1);
                // TODO Error handling.
                snprintf(lbl, lbl_len, "\"%s\":", ent->pld.pair.anno.str);
                break;
            case LDOC_ENT_REF:
                // TODO
                break;
            case LDOC_ENT_TXT:
                lbl_len = strlen(ldoc_cnst_json_txt) + 21;
                lbl = (char*)malloc(lbl_len + 1);
                // TODO Error handling.
                snprintf(lbl, lbl_len, "\"%s-%llx\":", ldoc_cnst_json_txt, (unsigned long long)ent);
                break;
            case LDOC_ENT_URI:
                // TODO
                break;
            default:
                // TODO Error handling.
                lbl = NULL;
                break;
        }
    
    size_t lbl_len_act;
    
    if (lbl)
        lbl_len_act = strlen(lbl);
    else
        lbl_len_act = lbl_len;
    
    size_t clen = lbl_len_act + json_len + (coord->pln ? 1 : 0);
    
    ser->pld.str = (char*)malloc(clen + 1);
    
    if (coord->pln)
    {
        ser->pld.str[0] = ',';
        strncpy(&ser->pld.str[1], lbl, lbl_len_act);
        strncpy(&ser->pld.str[lbl_len_act + 1], json, json_len);
        ser->pld.str[clen] = 0;
    }
    else
    {
        strncpy(&ser->pld.str[0], lbl, lbl_len_act);
        strncpy(&ser->pld.str[lbl_len_act], json, json_len);
        ser->pld.str[clen] = 0;
    }
    
    return ser;
}

//
// Python
//

ldoc_ser_t* ldoc_vis_setup_py(void)
{
    return LDOC_SER_NULL;
}

ldoc_ser_t* ldoc_vis_teardown_py(void)
{
    return LDOC_SER_NULL;
}

ldoc_ser_t* ldoc_vis_nde_pre_py(ldoc_nde_t* nde, ldoc_coord_t* coord)
{
    ldoc_ser_t* ser;
    
    if (!coord->lvl)
    {
        ser = ldoc_ser_new(LDOC_SER_PY_DCT);
        
        ser->pld.py.anno = NULL;
        ser->pld.py.dtm = PyDict_New();
        
        return ser;
    }
    
    PyObject* lbl;
    
    if (nde->prnt->tpe == LDOC_NDE_OL)
    {
        lbl = NULL;
    }
    else if (nde->mkup.anno.str != NULL)
    {
        lbl = PyUnicode_FromString(nde->mkup.anno.str);
    }
    else
    {
        size_t lbl_len = strlen(ldoc_cnst_json_nde) + 17;
        char* clbl = (char*)malloc(lbl_len + 1);
        snprintf(clbl, lbl_len, "%s-%llx", ldoc_cnst_json_nde, (unsigned long long)nde);
        
        lbl = PyUnicode_FromString(clbl);
        
        free(clbl);
    }

    if (nde->tpe == LDOC_NDE_OL)
        ser = ldoc_ser_new(LDOC_SER_PY_LST);
    else
        ser = ldoc_ser_new(LDOC_SER_PY_DCT);
    
    // TODO Error handling.
    
    ser->pld.py.anno = lbl;
    
    if (nde->tpe == LDOC_NDE_OL)
        ser->pld.py.dtm = PyList_New(0);
    else
        ser->pld.py.dtm = PyDict_New();
    
    return ser;
}

ldoc_ser_t* ldoc_vis_nde_infx_py(ldoc_nde_t* nde, ldoc_coord_t* coord)
{
    return LDOC_SER_NULL;
}

ldoc_ser_t* ldoc_vis_nde_post_py(ldoc_nde_t* nde, ldoc_coord_t* coord)
{
    return LDOC_SER_NULL;
}

PyObject* ldoc_vis_ent_py_val(ldoc_ent_t* ent, ldoc_coord_t* coord, size_t* len)
{
    PyObject* val;
    
    if ((ent->tpe == LDOC_ENT_BR && !ent->pld.pair.dtm.str) ||
        (ent->tpe == LDOC_ENT_NR && !ent->pld.pair.dtm.str) ||
        (ent->tpe == LDOC_ENT_OR && !ent->pld.pair.dtm.str) ||
        (ent->tpe != LDOC_ENT_OR && !ent->pld.str))
    {
        val = Py_None;
    }
    else
        // TODO Check why val_len is one larger than required (???) some times.
        switch (ent->tpe)
        {
            case LDOC_ENT_BL:
                if (ent->pld.bl)
                    val = Py_True;
                else
                    val = Py_False;
                break;
            case LDOC_ENT_BR:
                if (ent->pld.pair.dtm.bl)
                    val = Py_True;
                else
                    val = Py_False;
                break;
            case LDOC_ENT_NUM:
                if (ldoc_isfloat(ent->pld.str))
                    val = PyFloat_FromDouble(strtod(ent->pld.str, NULL));
                else
                    val = PyLong_FromLong(strtol(ent->pld.str, NULL, 10));
                break;
            case LDOC_ENT_NR:
                val = PyLong_FromLong(strtol(ent->pld.pair.dtm.str, NULL, 10));
                break;
            case LDOC_ENT_OR:
                val = PyUnicode_FromString(ent->pld.pair.dtm.str);
                break;
            default:
                val = PyUnicode_FromString(ent->pld.str);
                break;
        }
    
    return val;
}

ldoc_ser_t* ldoc_vis_ent_py(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord)
{
    PyObject* json = ldoc_vis_ent_py_val(ent, coord, NULL);
    
    ldoc_ser_t* ser;
    
    if (ent->tpe == LDOC_ENT_NUM)
    {
        if (ldoc_isfloat(ent->pld.str))
            ser = ldoc_ser_new(LDOC_SER_PY_FLT);
        else
            ser = ldoc_ser_new(LDOC_SER_PY_INT);
    }
    else if (ent->tpe == LDOC_ENT_BL)
        ser = ldoc_ser_new(LDOC_SER_PY_BL);
    else
        ser = ldoc_ser_new(LDOC_SER_PY_STR);
    
    if (!ser)
    {
        // TODO Error handling.
    }
    
    size_t lbl_len;
    char* clbl;
    PyObject* lbl = NULL;
    if (nde->tpe == LDOC_NDE_OL)
        switch (ent->tpe)
        {
            case LDOC_ENT_BR:
            case LDOC_ENT_NR:
            case LDOC_ENT_OR:
                ser->pld.py.anno = PyUnicode_FromString(ent->pld.pair.anno.str);
                ser->pld.py.dtm = json;
                
                return ser;
            default:
                break;
        }
    else
        switch (ent->tpe)
        {
            case LDOC_ENT_BL:
                lbl_len = strlen(ldoc_cnst_json_bl) + 17 + 1;
                clbl = (char*)malloc(lbl_len);
                // TODO Error handling.
                snprintf(clbl, lbl_len, "%s-%llx", ldoc_cnst_json_bl, (unsigned long long)ent);
                
                lbl = PyUnicode_FromString(clbl);
                
                free(clbl);
                break;
            case LDOC_ENT_EM1:
                lbl_len = strlen(ldoc_cnst_json_em1) + 17 + 1;
                clbl = (char*)malloc(lbl_len);
                // TODO Error handling.
                snprintf(clbl, lbl_len, "%s-%llx", ldoc_cnst_json_em1, (unsigned long long)ent);
                
                lbl = PyUnicode_FromString(clbl);
                
                free(clbl);
                break;
            case LDOC_ENT_EM2:
                lbl_len = strlen(ldoc_cnst_json_em2) + 17 + 1;
                clbl = (char*)malloc(lbl_len);
                // TODO Error handling.
                snprintf(clbl, lbl_len, "%s-%llx", ldoc_cnst_json_em2, (unsigned long long)ent);
                
                lbl = PyUnicode_FromString(clbl);
                
                free(clbl);
                break;
            case LDOC_ENT_NUM:
                lbl_len = strlen(ldoc_cnst_json_num) + 17 + 1;
                clbl = (char*)malloc(lbl_len);
                // TODO Error handling.
                snprintf(clbl, lbl_len, "%s-%llx", ldoc_cnst_json_num, (unsigned long long)ent);
                
                lbl = PyUnicode_FromString(clbl);
                
                free(clbl);
                break;
            case LDOC_ENT_BR:
            case LDOC_ENT_NR:
            case LDOC_ENT_OR:
                lbl = PyUnicode_FromString(ent->pld.pair.anno.str);
                break;
            case LDOC_ENT_REF:
                // TODO
                break;
            case LDOC_ENT_TXT:
                lbl_len = strlen(ldoc_cnst_json_txt) + 18; // TODO Why is this 18 when everything else is +17?
                clbl = (char*)malloc(lbl_len + 1);
                // TODO Error handling.
                snprintf(clbl, lbl_len, "%s-%llx", ldoc_cnst_json_txt, (unsigned long long)ent);
                
                lbl = PyUnicode_FromString(clbl);
                
                free(clbl);
                break;
            case LDOC_ENT_URI:
                // TODO
                break;
            default:
                // TODO Error handling.
                lbl = NULL;
                break;
        }
    
    ser->pld.py.anno = lbl;
    ser->pld.py.dtm = json;
    
    return ser;
}

////

ldoc_ser_t* ldoc_vis_dmp(ldoc_nde_t* nde, ldoc_ent_t* ent, ldoc_coord_t* coord)
{
    return LDOC_SER_NULL;
}

static inline ldoc_ser_t* ldoc_vis_nde_tpe(ldoc_nde_t* nde, ldoc_coord_t* coord, ldoc_vis_nde_t* vis)
{
    switch (nde->tpe)
    {
        case LDOC_NDE_ANC:
            return vis->vis_anc(nde, coord);
        case LDOC_NDE_H1:
            return vis->vis_h1(nde, coord);
        case LDOC_NDE_H2:
            return vis->vis_h2(nde, coord);
        case LDOC_NDE_H3:
            return vis->vis_h3(nde, coord);
        case LDOC_NDE_H4:
            return vis->vis_h4(nde, coord);
        case LDOC_NDE_H5:
            return vis->vis_h5(nde, coord);
        case LDOC_NDE_H6:
            return vis->vis_h6(nde, coord);
        case LDOC_NDE_NN:
            return vis->vis_nn(nde, coord);
        case LDOC_NDE_OL:
            return vis->vis_ol(nde, coord);
        case LDOC_NDE_OO:
            return vis->vis_oo(nde, coord);
        case LDOC_NDE_PAR:
            return vis->vis_par(nde, coord);
        case LDOC_NDE_RT:
            return vis->vis_rt(nde, coord);
        case LDOC_NDE_UA:
            return vis->vis_ua(nde, coord);
        case LDOC_NDE_UL:
            return vis->vis_ul(nde, coord);
        default:
            // TODO Error.
            break;
    }
    
    return LDOC_SER_NULL;
}

static ldoc_ser_t* ldoc_vis_nde(ldoc_nde_t* nde, ldoc_coord_t* coord, ldoc_vis_nde_ord_t* vis_nde, ldoc_vis_ent_t* vis_ent)
{
    ldoc_ser_t* ser = ldoc_vis_nde_tpe(nde, coord, &(vis_nde->pre));

    // Always set plane to zero when entering a new node:
    // - Bug? Was beforehand:
    //     Remember which plane the node is on:
    //     uint32_t pln = coord->pln;
    uint32_t pln = 0;
    
    // Descendants (entities) will get the plane reported relative to this node:
    coord->pln = 0;
    
    ldoc_ent_t* ent;
    TAILQ_FOREACH(ent, &(nde->ents), ldoc_ent_entries)
    {
        ldoc_ser_t* ser_ent = ldoc_vis_ent(nde, ent, coord, vis_ent);
        
        ldoc_ser_concat(ser, ser_ent);
        
        coord->pln++;
    }

    ldoc_ser_t* ser_infx = ldoc_vis_nde_tpe(nde, coord, &(vis_nde->infx));
    ldoc_ser_concat(ser, ser_infx);
    
    // Reset plane -- for node visits:
    coord->pln = pln;
    
    // Increase level for following node visits:
    coord->lvl++;

    ldoc_nde_t* dsc;
    TAILQ_FOREACH(dsc, &(nde->dscs), ldoc_nde_entries)
    {
        ldoc_ser_t* ser_nde = ldoc_vis_nde(dsc, coord, vis_nde, vis_ent);
        
        ldoc_ser_concat(ser, ser_nde);
        
        coord->pln++;
    }
    
    // Decrease level after visit:
    // TODO Probably not working like this... check with binary tree (n >= 2).
    coord->lvl--;
    
    ldoc_ser_t* ser_post = ldoc_vis_nde_tpe(nde, coord, &(vis_nde->post));
    
    ldoc_ser_concat(ser, ser_post);
    
    return ser;
}

ldoc_ent_t* ldoc_ent_new(ldoc_content_t tpe)
{
    ldoc_ent_t* ent = (ldoc_ent_t*)malloc(sizeof(ldoc_ent_t));
    
    ent->tpe = tpe;
    ent->pld.str = NULL;
    
    return ent;
}

void ldoc_ent_free(ldoc_ent_t* ent)
{
    // TODO -- update: this should be it; no more to-do?
    
    free(ent);
}

ldoc_nde_t* ldoc_nde_new(ldoc_struct_t tpe)
{
    if (tpe == LDOC_NDE_RT)
    {
        // TODO Error.
        return LDOC_NDE_NULL;
    }
    
    return ldoc_nde_new_(tpe);
}

void ldoc_nde_free(ldoc_nde_t* nde)
{
    ldoc_ent_t* ent;
    
    TAILQ_FOREACH(ent, &(nde->ents), ldoc_ent_entries)
    {
        ldoc_ent_free(ent);
    }
    
    ldoc_nde_t* cld;
    
    TAILQ_FOREACH(cld, &(nde->dscs), ldoc_nde_entries)
    {
        ldoc_nde_free(cld);
    }
    
    free(nde);
}

void ldoc_nde_ent_push(ldoc_nde_t* nde, ldoc_ent_t* ent)
{
    TAILQ_INSERT_TAIL(&(nde->ents), ent, ldoc_ent_entries);
    nde->ent_cnt++;
}

void ldoc_nde_ent_pop()
{
    
}

void ldoc_nde_ent_shift(ldoc_nde_t* nde, ldoc_ent_t* ent)
{
    TAILQ_INSERT_HEAD(&(nde->ents), ent, ldoc_ent_entries);
    nde->ent_cnt++;
}

void ldoc_nde_ent_unshift()
{

}

void ldoc_nde_ent_ins()
{
    
}

void ldoc_nde_ent_rm()
{
    
}

void ldoc_nde_dsc_push(ldoc_nde_t* nde, ldoc_nde_t* dsc)
{
    dsc->prnt = nde;
    TAILQ_INSERT_TAIL(&(nde->dscs), dsc, ldoc_nde_entries);
    nde->dsc_cnt++;
}

void ldoc_nde_dsc_pop()
{
    
}

void ldoc_nde_dsc_shift(ldoc_nde_t* nde, ldoc_nde_t* dsc)
{
    dsc->prnt = nde;
    TAILQ_INSERT_HEAD(&(nde->dscs), dsc, ldoc_nde_entries);
    nde->dsc_cnt++;
}

void ldoc_nde_dsc_unshift()
{
    
}

void ldoc_nde_dsc_ins()
{
    
}

void ldoc_nde_dsc_rm()
{
    
}

uint16_t ldoc_nde_lvl(ldoc_nde_t* nde)
{
    uint16_t lvl = 0;
    
    while (nde->prnt)
    {
        nde = nde->prnt;
        lvl++;
    }
    
    return lvl;
}

ldoc_nde_qstk_t* ldoc_nde_qstk_new(uint64_t inc)
{
    ldoc_nde_qstk_t* qstk = (ldoc_nde_qstk_t*)malloc(sizeof(ldoc_nde_qstk_t));
    
    if (!qstk)
    {
        // TODO Error.
    }
    
    qstk->nds = (ldoc_nde_t**)malloc(sizeof(ldoc_nde_t*) * inc);
    
    if (!qstk->nds)
    {
        free(qstk);
        // TODO Error.
    }
    
    qstk->rptr = 0;
    qstk->wptr = 0;
    qstk->max = inc;
    qstk->inc = inc;
    
    return qstk;
}

void ldoc_nde_qstk_push(ldoc_nde_qstk_t* qstk, ldoc_nde_t* nde)
{
    // Write pointer is always on an empty spot: so fill it immediately!
    qstk->nds[qstk->wptr] = nde;
    
    if (LDOC_QSTK_FULL(qstk))
    {
        // Expand stack (increase array size):
        ldoc_nde_t** nnds = (ldoc_nde_t**)realloc(qstk->nds, sizeof(ldoc_nde_t*) * (qstk->max + qstk->inc));
        
        if (!nnds)
        {
            // TODO Error.
        }
        
        // Copy stack contents that are just before the write pointer into
        // the newly allocated space:
        if (!qstk->rptr)
        {
            // No action necessary. The newly allocated space can be written to "as is".
            qstk->wptr++;
        }
        else if (qstk->rptr < qstk->inc)
        {
            // Everything up to the read pointer (sic) can be moved into the additionally
            // created space. The new space increment might not be completely filled.
            // Note that the read pointer will always be greater than zero in this case.
            memcpy(LDOC_QSTK_NDE_ADDR(qstk, qstk->max), LDOC_QSTK_NDE_ADDR(qstk, 0), sizeof(ldoc_nde_t*) * qstk->rptr);
            qstk->wptr = qstk->max + qstk->rptr;
        }
        else
        {
            // Move up:
            memcpy(LDOC_QSTK_NDE_ADDR(qstk, qstk->max), LDOC_QSTK_NDE_ADDR(qstk, 0), sizeof(ldoc_nde_t*) * qstk->inc);
            // Move down:
            memcpy(LDOC_QSTK_NDE_ADDR(qstk, 0), LDOC_QSTK_NDE_ADDR(qstk, qstk->inc), sizeof(ldoc_nde_t*) * (qstk->rptr - qstk->inc));
            qstk->wptr -= qstk->inc;
        }
        
        qstk->max = qstk->max + qstk->inc;
    }
    else
    {
        qstk->wptr = (qstk->wptr + 1) % qstk->max;
    }
}

ldoc_nde_t* ldoc_nde_qstk_pop(ldoc_nde_qstk_t* qstk)
{
    if (qstk->rptr == qstk->wptr)
    {
        // Stack empty. Sorry.
        return LDOC_NDE_NULL;
    }

    ldoc_nde_t* nde = qstk->nds[qstk->rptr];
    
    qstk->rptr = (qstk->rptr + 1) % qstk->max;
    
    return nde;
}

ldoc_ser_t* ldoc_lkahead(ldoc_doc_t* doc, uint64_t off, uint16_t ln)
{
    return LDOC_SER_NULL;
}

ldoc_ser_t* ldoc_format(ldoc_doc_t* doc, ldoc_vis_nde_ord_t* vis_nde, ldoc_vis_ent_t* vis_ent)
{
    ldoc_coord_t coord = { 0, 0 };
    
    ldoc_ser_t* opn = vis_nde->vis_setup();
    
    ldoc_ser_t* ser = ldoc_vis_nde(doc->rt, &coord, vis_nde, vis_ent);
    
    ldoc_ser_t* cls = vis_nde->vis_teardown();
    
    // Opening can be omitted, in which case the root node serialization will
    // start the serialization:
    if (opn == LDOC_SER_NULL)
        opn = ser;
    else
        ldoc_ser_concat(opn, ser);
    
    ldoc_ser_concat(opn, cls);
    
    return opn;
}

static inline uint64_t ldoc_nde_ent_skip(ldoc_nde_t* nde, uint64_t off)
{
    ldoc_ent_t* ent;
    TAILQ_FOREACH(ent, &(nde->ents), ldoc_ent_entries)
    {
        // TODO Assumes entity type string.
        if (ent->pld.str)
            off += strlen(ent->pld.str);
    }
    
    return off;
}

ldoc_pos_t* ldoc_find_pos_trv(ldoc_nde_t* nde, uint64_t* cur, u_int64_t off)
{
    uint64_t end = ldoc_nde_ent_skip(nde, *cur);
    
    if (*cur <= off && off <= end)
    {
        ldoc_pos_t* pos = ldoc_pos_new(nde, cur, off);
        
        if (!pos)
        {
            // TODO Error.
        }
        
        return pos;
    }
    
    ldoc_nde_t* dsc;
    TAILQ_FOREACH(dsc, &(nde->dscs), ldoc_nde_entries)
    {
        ldoc_pos_t* pos = ldoc_find_pos_trv(dsc, cur, off);
        
        if (pos)
            return pos;
    }
    
    return LDOC_POS_NULL;
}

ldoc_pos_t* ldoc_find_pos(ldoc_doc_t* doc, uint64_t off)
{
    uint64_t cur = 0;
    
    return ldoc_find_pos_trv(doc->rt, &cur, off);
}

ldoc_pos_t* ldoc_find_kw(ldoc_doc_t* doc, uint64_t off, char* str)
{
    return LDOC_POS_NULL;
}

ldoc_trie_nde_arr_t* ldoc_find_mtchs(ldoc_doc_t* doc, uint64_t off, ldoc_trie_t* trie)
{
    ldoc_trie_nde_arr_t* arr = ldoc_trie_nde_arr_new();
    
    ldoc_ser_t* ser = ldoc_lkahead(doc, off, trie->max);
    
    ldoc_trie_lookup(trie, ser->pld.str, false);
    
    return NULL;
}
