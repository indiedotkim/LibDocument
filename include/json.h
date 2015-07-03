/*
 * Copyright (c) 2015 CODAMONO, Ontario, Canada
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __ldoc__json__
#define __ldoc__json__

#include "document.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    LDOC_JSON_NKW = 0,
    LDOC_JSON_NULL,
    LDOC_JSON_TRUE,
    LDOC_JSON_FALSE
} ldoc_json_kwval_t;
    
typedef enum
{
    LDOC_JSON_OK = 0,
    LDOC_JSON_INV
} ldoc_json_prs_err_t;

ldoc_doc_t* ldoc_json_read(char* json, size_t len, off_t* err);
ldoc_doc_t* ldoc_ldjson_read(char* ldj, size_t len, off_t* er, off_t* nxt);
    
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* defined(__ldoc__json__) */
