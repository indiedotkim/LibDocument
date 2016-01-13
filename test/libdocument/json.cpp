/*
 * Copyright (c) 2015-2016 CODAMONO, Ontario, Canada
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at: http://mozilla.org/MPL/2.0/
 */

/**
 * @file json.cpp
 *
 * @brief JSON dot cpp.
 *
 *
 */

#include <gtest/gtest.h>

#include "json.h"

const char* ldoc_json_small = "{ \"key1\" : 123, \"key2\" : \"Hello\\n\", \"key3\" : [ \"val3.1\", 3.141, null, true, false, [[ { \"nested\" : true } ]] ] }";
const char* ldoc_json_small_ref = "{\"key1\":123,\"key2\":\"Hello\\n\",\"key3\":[\"val3.1\",3.141,null,true,false,[[{\"nested\":true}]]]}";

const char* ldoc_json_empty_list = "{ \"key1\" : [] }";
const char* ldoc_json_empty_list_ref = "{\"key1\":[]}";

const char* ldoc_ldj = "{\"key1\":123}\n{\"key2\":true}\n{\"key3\":[1,2,3]}";

TEST(ldoc_json, empty_json)
{
    off_t err = 0;
    
    ldoc_doc_t* doc = ldoc_json_read("{}", 2, &err);
    EXPECT_EQ(0, err);
}

TEST(ldoc_json, small_json)
{
    off_t err = 0;
    
    ldoc_doc_t* doc = ldoc_json_read((char*)ldoc_json_small, strlen(ldoc_json_small), &err);
    EXPECT_EQ(0, err);
    
    ldoc_ser_t* ser = ldoc_format_json(doc);
    EXPECT_NE((ldoc_ser_t*)NULL, ser);
    EXPECT_STREQ(ldoc_json_small_ref, ser->pld.str);

    ldoc_ser_free(ser);
    ldoc_doc_free(doc);
}

TEST(ldoc_json, empty_list_json)
{
    off_t err = 0;
    
    ldoc_doc_t* doc = ldoc_json_read((char*)ldoc_json_empty_list, strlen(ldoc_json_empty_list), &err);
    EXPECT_EQ(0, err);
    
    ldoc_ser_t* ser = ldoc_format_json(doc);
    EXPECT_NE((ldoc_ser_t*)NULL, ser);
    EXPECT_STREQ(ldoc_json_empty_list_ref, ser->pld.str);
    
    ldoc_ser_free(ser);
    ldoc_doc_free(doc);
}

TEST(ldoc_json, ldj)
{
    char* ldj;
    off_t err = 0;
    off_t nxt = 0;
    size_t len = strlen(ldoc_ldj);
    
    ldoc_doc_t* doc = ldoc_ldjson_read((char*)ldoc_ldj, len, &err, &nxt);
    EXPECT_EQ(0, err);

    ldoc_ser_t* ser = ldoc_format_json(doc);
    EXPECT_NE((ldoc_ser_t*)NULL, ser);
    ldj = strdup(ser->pld.str);
    EXPECT_NE((char*)NULL, ldj);
    ldoc_ser_free(ser);
    ldoc_doc_free(doc);
    
    doc = ldoc_ldjson_read((char*)ldoc_ldj, len, &err, &nxt);
    EXPECT_EQ(0, err);

    ser = ldoc_format_json(doc);
    EXPECT_NE((ldoc_ser_t*)NULL, ser);
    ldj = (char*)realloc(ldj, strlen(ldj) + strlen(ser->pld.str) + 2);
    EXPECT_NE((char*)NULL, ldj);
    strcat(ldj, "\n");
    strcat(ldj, ser->pld.str);
    ldoc_ser_free(ser);
    ldoc_doc_free(doc);
    
    doc = ldoc_ldjson_read((char*)ldoc_ldj, len, &err, &nxt);
    EXPECT_EQ(0, err);

    ser = ldoc_format_json(doc);
    EXPECT_NE((ldoc_ser_t*)NULL, ser);
    ldj = (char*)realloc(ldj, strlen(ldj) + strlen(ser->pld.str) + 2);
    EXPECT_NE((char*)NULL, ldj);
    strcat(ldj, "\n");
    strcat(ldj, ser->pld.str);
    ldoc_ser_free(ser);
    ldoc_doc_free(doc);
    
    EXPECT_STREQ(ldoc_ldj, ldj);
}
