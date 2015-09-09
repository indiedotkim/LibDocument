/*
 * Copyright (c) 2015 CODAMONO, Ontario, Canada
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <gtest/gtest.h>

#include "trie.h"

#define LDOC_NULLTYPE long

#define FLORA 1
#define FAUNA 2

const char* entry_cat = "cat";
const char* entry_catnip = "catnip";
const char* entry_rose = "rose";

const char* collect_sep = ", ";
const char* collect_entries = "cat, catnip, rose";

TEST(ldoc_trie, empty_trie)
{
    ldoc_trie_t* trie = ldoc_trie_new();
    
    EXPECT_NE(NULL, (LDOC_NULLTYPE)trie);
    
    ldoc_trie_free(trie);
}

TEST(ldoc_trie, small_trie)
{
    ldoc_trie_t* trie = ldoc_trie_new();
    
    EXPECT_NE(NULL, (LDOC_NULLTYPE)trie);
    
    ldoc_trie_anno_t fauna = { FAUNA, NULL };
    ldoc_trie_anno_t flora = { FLORA, NULL };
    
    ldoc_trie_add(trie, entry_cat, EN_ALPH, fauna);
    ldoc_trie_add(trie, entry_catnip, EN_ALPH, flora);
    ldoc_trie_add(trie, entry_rose, EN_ALPH, flora);
    
    ldoc_trie_free(trie);
}

TEST(ldoc_trie, lookup)
{
    ldoc_trie_nde_t* res;
    ldoc_trie_t* trie = ldoc_trie_new();
    
    EXPECT_NE(NULL, (LDOC_NULLTYPE)trie);
    
    ldoc_trie_anno_t fauna = { FAUNA, (void*)123 };
    ldoc_trie_anno_t flora = { FLORA, LDOC_TRIE_ANNO_NULL.pld };
    
    ldoc_trie_add(trie, entry_cat, EN_ALPH, fauna);
    ldoc_trie_add(trie, entry_catnip, EN_ALPH, flora);
    ldoc_trie_add(trie, entry_rose, EN_ALPH, flora);
    
    // Show the complete trie:
    // ldoc_trie_dmp(trie);

    res = ldoc_trie_lookup(trie, "ca", false);
    EXPECT_EQ(NULL, (LDOC_NULLTYPE)res);
    
    res = ldoc_trie_lookup(trie, "cat", false);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)res);
    EXPECT_EQ(FAUNA, res->anno.cat);
    EXPECT_EQ((void*)123, res->anno.pld);
    
    res = ldoc_trie_lookup(trie, "rose", false);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)res);
    EXPECT_EQ(FLORA, res->anno.cat);
    EXPECT_EQ(LDOC_TRIE_ANNO_NULL.pld, res->anno.pld);
    
    res = ldoc_trie_lookup(trie, "catnip", false);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)res);
    EXPECT_EQ(FLORA, res->anno.cat);
    EXPECT_EQ(LDOC_TRIE_ANNO_NULL.pld, res->anno.pld);
    
    res = ldoc_trie_lookup(trie, "carrot", false);
    EXPECT_EQ(NULL, (LDOC_NULLTYPE)res);
    
    res = ldoc_trie_lookup(trie, "zebra", false);
    EXPECT_EQ(NULL, (LDOC_NULLTYPE)res);
    
    ldoc_trie_free(trie);
}

TEST(ldoc_trie, lookup_prefix)
{
    ldoc_trie_nde_t* res;
    ldoc_trie_t* trie = ldoc_trie_new();
    
    EXPECT_NE(NULL, (LDOC_NULLTYPE)trie);
    
    ldoc_trie_anno_t fauna = { FAUNA, (void*)123 };
    ldoc_trie_anno_t flora = { FLORA, LDOC_TRIE_ANNO_NULL.pld };
    
    ldoc_trie_add(trie, entry_cat, EN_ALPH, fauna);
    ldoc_trie_add(trie, entry_catnip, EN_ALPH, flora);
    ldoc_trie_add(trie, entry_rose, EN_ALPH, flora);
    
    res = ldoc_trie_lookup(trie, "cat", true);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)res);
    EXPECT_EQ(FAUNA, res->anno.cat);
    EXPECT_EQ((void*)123, res->anno.pld);
    
    res = ldoc_trie_lookup(trie, "catnip", true);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)res);
    EXPECT_EQ(FLORA, res->anno.cat);
    EXPECT_EQ(LDOC_TRIE_ANNO_NULL.pld, res->anno.pld);
    
    res = ldoc_trie_lookup(trie, "ca", true);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)res);
    EXPECT_EQ(0, res->anno.cat);
    EXPECT_EQ(NULL, res->anno.pld);
    
    res = ldoc_trie_lookup(trie, "carrot", true);
    EXPECT_EQ(NULL, (LDOC_NULLTYPE)res);
    
    res = ldoc_trie_lookup(trie, "zebra", true);
    EXPECT_EQ(NULL, (LDOC_NULLTYPE)res);
    
    ldoc_trie_free(trie);
}

TEST(ldoc_trie, collect)
{
    ldoc_trie_t* trie = ldoc_trie_new();
    
    EXPECT_NE(NULL, (LDOC_NULLTYPE)trie);
    
    ldoc_trie_anno_t fauna = { FAUNA, NULL };
    ldoc_trie_anno_t flora = { FLORA, NULL };
    
    ldoc_trie_add(trie, entry_cat, EN_ALPH, fauna);
    ldoc_trie_add(trie, entry_catnip, EN_ALPH, flora);
    ldoc_trie_add(trie, entry_rose, EN_ALPH, flora);
    
    char* str = ldoc_trie_collect(trie, collect_sep);
    EXPECT_STRCASEEQ(collect_entries, str);
    
    ldoc_trie_free(trie);
}

