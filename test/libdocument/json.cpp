/*
 * Copyright (c) 2015 CODAMONO, Ontario, Canada
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <gtest/gtest.h>

#include "json.h"

const char* ldoc_json_small = "{ \"key1\" : 123, \"key2\" : \"Hello\\n\", \"key3\" : [ \"val3.1\", 3.141, null, true, false, [[ { \"nested\" : true } ]] ] }";
const char* ldoc_json_small_ref = "{\"key1\":123,\"key2\":\"Hello\\n\",\"key3\":[\"val3.1\",3.141,null,true,false,[[{\"nested\":true}]]]}";
const char* ldoc_json_big = "{}";
const char* ldoc_ldj = "{}\n{}\n{}\n";

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

TEST(ldoc_json, big_json)
{
    off_t err = 0;
    
    ldoc_doc_t* doc = ldoc_json_read((char*)ldoc_json_big, strlen(ldoc_json_big), &err);
    EXPECT_EQ(0, err);
}