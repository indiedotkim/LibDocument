/*
 * Copyright (c) 2015-2016 CODAMONO, Ontario, Canada
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at: http://mozilla.org/MPL/2.0/
 */

/**
 * @file json.h
 *
 * @brief JSON and LDJSON readers that create annotated document representations.
 *
 * The JSON/LDJSON readers can also handle JSON Linked Data (JSON-LD) objects.
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

/**
 * @brief JSON keyword types.
 */
typedef enum
{
    /**
     * Not a JSON keyword ("no keyword").
     */
    LDOC_JSON_NKW = 0,
    /**
     * JSON "null" value.
     */
    LDOC_JSON_NULL,
    /**
     * JSON "true" value.
     */
    LDOC_JSON_TRUE,
    /**
     * JSON "false" value.
     */
    LDOC_JSON_FALSE
} ldoc_json_kwval_t;
    
/**
 * @brief JSON parsing errors.
 */
typedef enum
{
    /**
     * No parsing errors.
     */
    LDOC_JSON_OK = 0,
    /**
     * Invalid JSON object.
     */
    LDOC_JSON_INV
} ldoc_json_prs_err_t;

/**
 * @brief Converts a single JSON object in string form to a document.
 *
 * @param json JSON object as a string.
 * @param len Length of the string `json`.
 * @param err If a parsing error is encountered and the pointer `err` is not `NULL`, then `*err` is set to the character offset at which the parsing error was occurred.
 * @return A document object representing the JSON object provided as string `json`, or `LDOC_DOC_NULL` if a parsing error was encountered.
 */
ldoc_doc_t* ldoc_json_read(char* json, size_t len, off_t* err);

/**
 * @brief Converts a JSON objects -- one of many -- in string form to a document.
 *
 * This function can be used to convert line-delimited JSON (LDJSON) objects to LibDocument documents.
 *
 * @param json JSON object as a string.
 * @param len Length of the string `json`.
 * @param err If a parsing error is encountered and the pointer `err` is not `NULL`, then `*err` is set to the character offset at which the parsing error was occurred.
 * @param nxt If not NULL, then `*nxt` will be set to the character offset at which the next JSON object (purportedly) begins.
 * @return A document object representing the JSON object provided as string `json`, or `LDOC_DOC_NULL` if a parsing error was encountered.
 */
ldoc_doc_t* ldoc_ldjson_read(char* ldj, size_t len, off_t* err, off_t* nxt);
    
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* defined(__ldoc__json__) */
