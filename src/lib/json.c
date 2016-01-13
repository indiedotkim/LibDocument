/*
 * Copyright (c) 2015-2016 CODAMONO, Ontario, Canada
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at: http://mozilla.org/MPL/2.0/
 */

#include "json.h"

static inline ldoc_json_prs_err_t ldoc_json_val(ldoc_nde_t* nde, char* ky, char** str, size_t* len);
static inline ldoc_json_prs_err_t ldoc_json_arr(ldoc_nde_t* nde, char** str, size_t* len);

static inline char* ldoc_json_skpws(char* str, size_t* len)
{
    while ((*str == ' ' || *str == '\r' || *str == '\n' || *str == '\t') && *len)
    {
        str++;
        (*len)--;
    }
    
    return str;
}

static inline bool ldoc_json_iter(char** str, size_t* len)
{
    *str = ldoc_json_skpws(*str, len);
    
    if (*len && **str == ',')
    {
        (*str)++;
        (*len)--;
        
        *str = ldoc_json_skpws(*str, len);
        
        return true;
    }
    
    return false;
}

static inline ldoc_json_kwval_t ldoc_json_kwd(char** str, size_t* len)
{
    if (*len >= 4)
    {
        if (!strncmp(*str, "null", 4))
        {
            *str += 4;
            *len -= 4;
            
            return LDOC_JSON_NULL;
        }
        else if (!strncmp(*str, "true", 4))
        {
            *str += 4;
            *len -= 4;
            
            return LDOC_JSON_TRUE;
        }
        else if (*len >= 5 && !strncmp(*str, "false", 5))
        {
            *str += 5;
            *len -= 5;
            
            return LDOC_JSON_FALSE;
        }
    }
    
    return LDOC_JSON_NKW;
}

static inline bool ldoc_json_dgts(char** str, size_t* len)
{
    if (!*len || !(**str >= '0' && **str <= '9'))
        return false;
    
    // Digits:
    while (*len && (**str >= '0' && **str <= '9'))
    {
        (*str)++;
        (*len)--;
    }
    
    return true;
}

static inline char* ldoc_json_num(char** str, size_t* len)
{
    char* bgn = *str;
    
    // Sign:
    if (**str == '-')
    {
        (*str)++;
        (*len)--;
    }

    // Digits before a decimal point:
    if (!ldoc_json_dgts(str, len))
        return NULL;
    
    // Decimal point:
    if (*len && **str == '.')
    {
        (*str)++;
        (*len)--;
        
        // Digits after a decimal point:
        if (!ldoc_json_dgts(str, len))
            return NULL;
    }
    
    // Exponent:
    if (*len > 1 && (**str == 'e' || **str == 'E'))
    {
        (*str)++;
        (*len)--;
        
        // Sign in exponent:
        if (**str == '-' || **str == '+')
        {
            (*str)++;
            (*len)--;
        }
        
        // Digits in the exponent:
        if (!ldoc_json_dgts(str, len))
            return NULL;
    }
    
    if (*len)
    {
        char* s = strndup(bgn, *str - bgn);
        
        // TODO Error handling.
        
        return s;
    }
    
    return NULL;
}

static inline char* ldoc_json_qstr(char** str, size_t* len)
{
    // Skip '"':
    (*str)++;
    (*len)--;
    
    char* bgn = *str;
    while (*len && **str != '"')
    {
        if (**str == '\\')
        {
            (*str)++;
            (*len)--;
            
            // Premature end of string:
            if (!*len)
                return NULL;
            
            // Unicode escape:
            if (**str == 'u')
            {
                if (*len < 4)
                    return NULL;
                
                *str += 4;
                *len -= 4;
            }
            else
            {
                (*str)++;
                (*len)--;
            }
        }
        else
        {
            (*str)++;
            (*len)--;
        }
    }
    
    if (*len && **str == '"')
    {
        char* s = strndup(bgn, *str - bgn);
        
        // TODO Error handling.
        
        // Skip '"':
        (*str)++;
        (*len)--;
        
        return s;
    }
    
    return NULL;
}

static inline ldoc_json_prs_err_t ldoc_json_obj(ldoc_nde_t* nde, char** str, size_t* len)
{
    // Skip '{':
    (*str)++;
    (*len)--;
    
    do
    {
        *str = ldoc_json_skpws(*str, len);
        
        if (len && **str == '}')
        {
            // Skip '}':
            (*str)++;
            (*len)--;

            return LDOC_JSON_OK;
        }
        
        if (!len || **str != '"')
            return LDOC_JSON_INV;
        
        char* ky = ldoc_json_qstr(str, len);
        
        *str = ldoc_json_skpws(*str, len);
        
        // Colon (key/value separator):
        if (!*len || **str != ':')
            return LDOC_JSON_INV;
        
        // Skip ':':
        (*str)++;
        (*len)--;

        *str = ldoc_json_skpws(*str, len);
        
        if (!*len)
            return LDOC_JSON_INV;
        
        // Object type (object/array) or some primitive?
        if (**str == '{' || **str == '[')
        {
            ldoc_nde_t* dsc = ldoc_nde_new(**str == '{' ? LDOC_NDE_UA : LDOC_NDE_OL);
            
            // TODO Error handling.
            
            dsc->mkup.anno.str = ky;
            
            ldoc_json_prs_err_t err;
            if (**str == '{')
                err = ldoc_json_obj(dsc, str, len);
            else
                err = ldoc_json_arr(dsc, str, len);
            
            if (err)
                return err;
            
            ldoc_nde_dsc_push(nde, dsc);
        }
        else
        {
            ldoc_json_prs_err_t err = ldoc_json_val(nde, ky, str, len);
            
            if (err)
                return err;
        }
    } while (ldoc_json_iter(str, len));
    
    if (!*len || **str != '}')
        return LDOC_JSON_INV;
    
    // Skip '}':
    (*str)++;
    (*len)--;
    
    return LDOC_JSON_OK;
}

static inline ldoc_json_prs_err_t ldoc_json_arr(ldoc_nde_t* nde, char** str, size_t* len)
{
    // Skip '[':
    (*str)++;
    (*len)--;
    
    *str = ldoc_json_skpws(*str, len);
    
    if (!len)
        return LDOC_JSON_INV;
    
    // Empty array, skip ']', then return:
    if (**str == ']')
    {
        (*str)++;
        (*len)--;
        
        return LDOC_JSON_OK;
    }
    
    do
    {
        if (**str == '{' || **str == '[')
        {
            ldoc_nde_t* dsc = ldoc_nde_new(**str == '{' ? LDOC_NDE_UA : LDOC_NDE_OL);
            
            // TODO Error handling.
            
            dsc->mkup.anno.str = strdup("NA");
            
            if (**str == '{')
                ldoc_json_obj(dsc, str, len);
            else
                ldoc_json_arr(dsc, str, len);
            
            ldoc_nde_dsc_push(nde, dsc);
        }
        else
        {
            ldoc_json_prs_err_t err = ldoc_json_val(nde, NULL, str, len);
            
            if (err)
                return err;
        }
    } while (ldoc_json_iter(str, len));

    if (!*len || **str != ']')
        return LDOC_JSON_INV;
    
    // Skip ']':
    (*str)++;
    (*len)--;
    
    return LDOC_JSON_OK;
}

static inline ldoc_json_prs_err_t ldoc_json_val(ldoc_nde_t* nde, char* ky, char** str, size_t* len)
{
    ldoc_ent_t* ent;
    
    if (**str == '"')
    {
        char* val = ldoc_json_qstr(str, len);
        
        if (!val)
            return LDOC_JSON_INV;
        
        ent = ldoc_ent_new(ky ? LDOC_ENT_OR : LDOC_ENT_TXT);
        
        // TODO Error handling.
        
        if (ky)
        {
            ent->pld.pair.anno.str = ky;
            ent->pld.pair.dtm.str = val;
        }
        else
            ent->pld.str = val;
        
        ldoc_nde_ent_push(nde, ent);
        
        return LDOC_JSON_OK;
    }
    else if (**str == '-' || (**str >= '0' && **str <= '9'))
    {
        char* val = ldoc_json_num(str, len);
        
        if (!val)
            return LDOC_JSON_INV;
        
        ent = ldoc_ent_new(ky ? LDOC_ENT_NR : LDOC_ENT_NUM);
        
        // TODO Error handling.
        
        if (ky)
        {
            ent->pld.pair.anno.str = ky;
            ent->pld.pair.dtm.str = val;
        }
        else
            ent->pld.str = val;
        
        ldoc_nde_ent_push(nde, ent);
        
        return LDOC_JSON_OK;
    }
    else
    {
        ldoc_json_kwval_t kwval = ldoc_json_kwd(str, len);
        
        ldoc_content_t tpe;
        switch (kwval)
        {
            case LDOC_JSON_NULL:
                tpe = ky ? LDOC_ENT_OR : LDOC_ENT_TXT;
                break;
            case LDOC_JSON_FALSE:
            case LDOC_JSON_TRUE:
                tpe = ky ? LDOC_ENT_BR : LDOC_ENT_BL;
                break;
            default:
                return LDOC_JSON_INV;
        }
        
        ent = ldoc_ent_new(tpe);
        
        // TODO Error handling.

        if (ky)
            ent->pld.pair.anno.str = ky;
        
        switch (kwval)
        {
            case LDOC_JSON_NULL:
                if (ky)
                    ent->pld.pair.dtm.str = NULL;
                else
                    ent->pld.str = NULL;
                break;
            case LDOC_JSON_FALSE:
                if (ky)
                    ent->pld.pair.dtm.bl = false;
                else
                    ent->pld.bl = false;
                break;
            case LDOC_JSON_TRUE:
                if (ky)
                    ent->pld.pair.dtm.bl = true;
                else
                    ent->pld.bl = true;
                break;
            default:
                // TODO This would be an internal error.
                break;
        }
        
        ldoc_nde_ent_push(nde, ent);
        
        return LDOC_JSON_OK;
    }
    
    return LDOC_JSON_INV;
}

ldoc_doc_t* ldoc_json_read_doc(char* json, size_t len, off_t* err, off_t* nxt)
{
    char* obj = ldoc_json_skpws(json + (nxt ? *nxt : 0), &len);

    if (!len || *obj != '{')
    {
        if (err)
            *err = obj - json;
        
        return NULL;
    }
    
    ldoc_doc_t* doc = ldoc_doc_new();
    
    // TODO Error handling.
    
    ldoc_json_obj(doc->rt, &obj, &len);
    
    if (nxt)
        *nxt = obj - json;
    
    return doc;
}

ldoc_doc_t* ldoc_json_read(char* json, size_t len, off_t* err)
{
    return ldoc_json_read_doc(json, len, err, NULL);
}

ldoc_doc_t* ldoc_ldjson_read(char* ldj, size_t len, off_t* err, off_t* nxt)
{
    if (*nxt >= len)
        return NULL;
    
    return ldoc_json_read_doc(ldj, len, err, nxt);
}
