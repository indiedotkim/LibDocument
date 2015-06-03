/*
 * Copyright (c) 2015 CODAMONO, Ontario, Canada
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <gtest/gtest.h>

#include "document.h"

#define LDOC_NULLTYPE long

static const char* ldoc_big_doc_1_1 = "Heading 1";
// Lorem ipsum, 1500s:
static const char* ldoc_big_doc_1_2_1 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
// Cicero, 45BC:
static const char* ldoc_big_doc_1_2_2 = "Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo. Nemo enim ipsam voluptatem quia voluptas sit aspernatur aut odit aut fugit, sed quia consequuntur magni dolores eos qui ratione voluptatem sequi nesciunt. Neque porro quisquam est, qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit, sed quia non numquam eius modi tempora incidunt ut labore et dolore magnam aliquam quaerat voluptatem. Ut enim ad minima veniam, quis nostrum exercitationem ullam corporis suscipit laboriosam, nisi ut aliquid ex ea commodi consequatur? Quis autem vel eum iure reprehenderit qui in ea voluptate velit esse quam nihil molestiae consequatur, vel illum qui dolorem eum fugiat quo voluptas nulla pariatur?";
static const char* ldoc_big_doc_2_1 = "Heading 2";
static const char* ldoc_big_doc_2_1_1 = "Heading 3";
static const char* ldoc_big_doc_2_1_2 = "Lorem ipsum... you get the idea!";
static const char* ldoc_big_doc_2_2_1_1 = "Heading 3, ";
static const char* ldoc_big_doc_2_2_1_2 = "again";
static const char* ldoc_big_doc_2_2_1_3 = "!";
static const char* ldoc_big_doc_2_2_2 = "Introducing some ";
static const char* ldoc_big_doc_2_2_3 = "emphasized";
static const char* ldoc_big_doc_2_2_4 = " text!";

static const char* ldoc_ord_doc_hd = "Lists";
static const char* ldoc_ord_doc_1 = "Entity 1";
static const char* ldoc_ord_doc_2 = "Entity 2";
static const char* ldoc_ord_doc_3k = "Entity 3 (key)";
static const char* ldoc_ord_doc_3v = "Entity 3 (value)";
static const char* ldoc_ord_doc_4_1 = "Node 1";
static const char* ldoc_ord_doc_4_1_1k = "Key 1";
static const char* ldoc_ord_doc_4_1_1v = "Value 1";
static const char* ldoc_ord_doc_4_2 = "Node 2";
static const char* ldoc_ord_doc_4_2_1k = "Key 2.1";
static const char* ldoc_ord_doc_4_2_1v = "Value 2.1";
static const char* ldoc_ord_doc_4_2_2k = "Key 2.2";
static const char* ldoc_ord_doc_4_2_2v = "Value 2.2";
static const char* ldoc_ord_doc_4_3 = "Node 3";
static const char* ldoc_ord_doc_5_1k = "Key 4.1";
static const char* ldoc_ord_doc_5_1v = "Value 4.1";

static const char* ldoc_ord_doc_hd_1 = "List 1";
static const char* ldoc_ord_doc_hd_2 = "List 2";
static const char* ldoc_ord_doc_hd_3 = "List 3";

static const char* ldoc_ref_nr_k = "Key (Number)";
static const char* ldoc_ref_nr_v = "12345";
static const char* ldoc_ref_or_k = "Key (String)";
static const char* ldoc_ref_or_v = "String";
static const char* ldoc_ref_json = "{\"NID\":{\"Key (String)\":\"String\",\"Key (Number)\":12345}}";

static ldoc_doc_t* ldoc_big_doc()
{
    ldoc_doc_t* doc = ldoc_doc_new();
    EXPECT_NE(NULL, (LDOC_NULLTYPE)doc);
    
    ldoc_nde_t* nde = ldoc_nde_new(LDOC_NDE_H1);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)nde);
    ldoc_nde_dsc_push(doc->rt, nde);
    
    ldoc_ent_t* ent = ldoc_ent_new(LDOC_ENT_TXT);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    ent->pld.str = (char*)ldoc_big_doc_1_1;
    ldoc_nde_ent_push(nde, ent);
    
    ldoc_nde_t* nde_ = ldoc_nde_new(LDOC_NDE_PAR);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)nde_);
    ldoc_nde_dsc_push(nde, nde_);
    
    ent = ldoc_ent_new(LDOC_ENT_TXT);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    ent->pld.str = (char*)ldoc_big_doc_1_2_1;
    ldoc_nde_ent_push(nde_, ent);

    nde_ = ldoc_nde_new(LDOC_NDE_PAR);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)nde_);
    ldoc_nde_dsc_push(nde, nde_);

    ent = ldoc_ent_new(LDOC_ENT_TXT);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    ent->pld.str = (char*)ldoc_big_doc_1_2_2;
    ldoc_nde_ent_push(nde_, ent);
    
    ldoc_nde_t* nde_h2 = ldoc_nde_new(LDOC_NDE_H2);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)nde_h2);
    ldoc_nde_dsc_push(nde, nde_h2);
    nde = nde_h2;
    
    ent = ldoc_ent_new(LDOC_ENT_TXT);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    ent->pld.str = (char*)ldoc_big_doc_2_1;
    ldoc_nde_ent_push(nde, ent);

    nde_ = ldoc_nde_new(LDOC_NDE_H3);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)nde_);
    ldoc_nde_dsc_push(nde, nde_);
    nde = nde_;
    
    ent = ldoc_ent_new(LDOC_ENT_TXT);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    ent->pld.str = (char*)ldoc_big_doc_2_1_1;
    ldoc_nde_ent_push(nde, ent);
    
    nde_ = ldoc_nde_new(LDOC_NDE_PAR);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)nde_);
    ldoc_nde_dsc_push(nde, nde_);
    nde = nde_;
    
    ent = ldoc_ent_new(LDOC_ENT_TXT);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    ent->pld.str = (char*)ldoc_big_doc_2_1_2;
    ldoc_nde_ent_push(nde, ent);
    
    nde = ldoc_nde_new(LDOC_NDE_H3);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)nde);
    ldoc_nde_dsc_push(nde_h2, nde);
    
    ent = ldoc_ent_new(LDOC_ENT_TXT);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    ent->pld.str = (char*)ldoc_big_doc_2_2_1_1;
    ldoc_nde_ent_push(nde, ent);

    ent = ldoc_ent_new(LDOC_ENT_EM1);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    ent->pld.str = (char*)ldoc_big_doc_2_2_1_2;
    ldoc_nde_ent_push(nde, ent);
    
    ent = ldoc_ent_new(LDOC_ENT_TXT);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    ent->pld.str = (char*)ldoc_big_doc_2_2_1_3;
    ldoc_nde_ent_push(nde, ent);
    
    nde_ = ldoc_nde_new(LDOC_NDE_PAR);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)nde_);
    ldoc_nde_dsc_push(nde, nde_);
    nde = nde_;
    
    ent = ldoc_ent_new(LDOC_ENT_TXT);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    ent->pld.str = (char*)ldoc_big_doc_2_2_2;
    ldoc_nde_ent_push(nde, ent);

    ent = ldoc_ent_new(LDOC_ENT_EM1);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    ent->pld.str = (char*)ldoc_big_doc_2_2_3;
    ldoc_nde_ent_push(nde, ent);
    
    ent = ldoc_ent_new(LDOC_ENT_TXT);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    ent->pld.str = (char*)ldoc_big_doc_2_2_4;
    ldoc_nde_ent_push(nde, ent);
    
    return doc;
}

static ldoc_doc_t* ldoc_ord_doc()
{
    ldoc_doc_t* doc = ldoc_doc_new();
    EXPECT_NE(NULL, (LDOC_NULLTYPE)doc);
    
    ldoc_nde_t* nde = ldoc_nde_new(LDOC_NDE_OL);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)nde);
    nde->mkup.anno.str = (char*)ldoc_ord_doc_hd_1;
    ldoc_nde_dsc_push(doc->rt, nde);
    
    ldoc_ent_t* ent = ldoc_ent_new(LDOC_ENT_TXT);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    ent->pld.str = (char*)ldoc_ord_doc_1;
    ldoc_nde_ent_push(nde, ent);
    
    ent = ldoc_ent_new(LDOC_ENT_TXT);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    ent->pld.str = (char*)ldoc_ord_doc_2;
    ldoc_nde_ent_push(nde, ent);
    
    ent = ldoc_ent_new(LDOC_ENT_OR);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    ent->pld.pair.anno.str = (char*)ldoc_ord_doc_3k;
    ent->pld.pair.dtm.str = (char*)ldoc_ord_doc_3v;
    ldoc_nde_ent_push(nde, ent);
    
    ldoc_nde_t* nde_ = ldoc_nde_new(LDOC_NDE_UA);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)nde_);
    nde_->mkup.anno.str = (char*)ldoc_ord_doc_4_1;
    ldoc_nde_dsc_push(nde, nde_);
    
    ent = ldoc_ent_new(LDOC_ENT_OR);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    ent->pld.pair.anno.str = (char*)ldoc_ord_doc_4_1_1k;
    ent->pld.pair.dtm.str = (char*)ldoc_ord_doc_4_1_1v;
    ldoc_nde_ent_push(nde_, ent);
    
    nde_ = ldoc_nde_new(LDOC_NDE_UA);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)nde_);
    nde_->mkup.anno.str = (char*)ldoc_ord_doc_4_2;
    ldoc_nde_dsc_push(nde, nde_);
    
    ent = ldoc_ent_new(LDOC_ENT_OR);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    ent->pld.pair.anno.str = (char*)ldoc_ord_doc_4_2_1k;
    ent->pld.pair.dtm.str = (char*)ldoc_ord_doc_4_2_1v;
    ldoc_nde_ent_push(nde_, ent);
    
    ent = ldoc_ent_new(LDOC_ENT_OR);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    ent->pld.pair.anno.str = (char*)ldoc_ord_doc_4_2_2k;
    ent->pld.pair.dtm.str = (char*)ldoc_ord_doc_4_2_2v;
    ldoc_nde_ent_push(nde_, ent);
    
    nde_ = ldoc_nde_new(LDOC_NDE_UA);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)nde_);
    nde_->mkup.anno.str = (char*)ldoc_ord_doc_4_3;
    ldoc_nde_dsc_push(nde, nde_);
    
    nde = ldoc_nde_new(LDOC_NDE_OL);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)nde);
    nde->mkup.anno.str = (char*)ldoc_ord_doc_hd_2;
    ldoc_nde_dsc_push(doc->rt, nde);
    
    nde_ = ldoc_nde_new(LDOC_NDE_OL);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)nde);
    nde_->mkup.anno.str = (char*)ldoc_ord_doc_hd_3;
    ldoc_nde_dsc_push(nde, nde_);
    
    ent = ldoc_ent_new(LDOC_ENT_OR);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    ent->pld.pair.anno.str = (char*)ldoc_ord_doc_5_1k;
    ent->pld.pair.dtm.str = (char*)ldoc_ord_doc_5_1v;
    ldoc_nde_ent_push(nde_, ent);
    
    return doc;
}

static ldoc_doc_t* ldoc_mul_ord_doc()
{
    ldoc_doc_t* doc = ldoc_doc_new();
    EXPECT_NE(NULL, (LDOC_NULLTYPE)doc);
    
    ldoc_nde_t* nde = ldoc_nde_new(LDOC_NDE_OL);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)nde);
    nde->mkup.anno.str = (char*)ldoc_ord_doc_hd_1;
    ldoc_nde_dsc_push(doc->rt, nde);
    
    ldoc_ent_t* ent = ldoc_ent_new(LDOC_ENT_TXT);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    ent->pld.str = (char*)ldoc_ord_doc_1;
    ldoc_nde_ent_push(nde, ent);

    nde = ldoc_nde_new(LDOC_NDE_OL);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)nde);
    nde->mkup.anno.str = (char*)ldoc_ord_doc_hd_2;
    ldoc_nde_dsc_push(doc->rt, nde);
    
    ent = ldoc_ent_new(LDOC_ENT_TXT);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    ent->pld.str = (char*)ldoc_ord_doc_2;
    ldoc_nde_ent_push(nde, ent);

    nde = ldoc_nde_new(LDOC_NDE_OL);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)nde);
    nde->mkup.anno.str = (char*)ldoc_ord_doc_hd_3;
    ldoc_nde_dsc_push(doc->rt, nde);
    
    ent = ldoc_ent_new(LDOC_ENT_OR);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    ent->pld.pair.anno.str = (char*)ldoc_ord_doc_3k;
    ent->pld.pair.dtm.str = (char*)ldoc_ord_doc_3v;
    ldoc_nde_ent_push(nde, ent);
    
    ldoc_nde_t* nde_ = ldoc_nde_new(LDOC_NDE_UA);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)nde_);
    nde_->mkup.anno.str = (char*)ldoc_ord_doc_4_1;
    ldoc_nde_dsc_push(nde, nde_);
    
    ent = ldoc_ent_new(LDOC_ENT_OR);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    ent->pld.pair.anno.str = (char*)ldoc_ord_doc_4_1_1k;
    ent->pld.pair.dtm.str = (char*)ldoc_ord_doc_4_1_1v;
    ldoc_nde_ent_push(nde_, ent);

    nde_ = ldoc_nde_new(LDOC_NDE_UA);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)nde_);
    nde_->mkup.anno.str = (char*)ldoc_ord_doc_4_2;
    ldoc_nde_dsc_push(nde, nde_);
    
    ent = ldoc_ent_new(LDOC_ENT_OR);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    ent->pld.pair.anno.str = (char*)ldoc_ord_doc_4_2_1k;
    ent->pld.pair.dtm.str = (char*)ldoc_ord_doc_4_2_1v;
    ldoc_nde_ent_push(nde_, ent);
    
    ent = ldoc_ent_new(LDOC_ENT_OR);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    ent->pld.pair.anno.str = (char*)ldoc_ord_doc_4_2_2k;
    ent->pld.pair.dtm.str = (char*)ldoc_ord_doc_4_2_2v;
    ldoc_nde_ent_push(nde_, ent);
    
    nde_ = ldoc_nde_new(LDOC_NDE_UA);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)nde_);
    nde_->mkup.anno.str = (char*)ldoc_ord_doc_4_3;
    ldoc_nde_dsc_push(nde, nde_);
    
    return doc;
}

TEST(ldoc_document, empty_document)
{
    ldoc_doc_t* doc = ldoc_doc_new();
    EXPECT_NE(NULL, (LDOC_NULLTYPE)doc);
    
    ldoc_doc_free(doc);
}

TEST(ldoc_document, empty_node)
{
    ldoc_nde_t* nde = ldoc_nde_new(LDOC_NDE_PAR);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)nde);
    // Parent gets assigned when using descend push/shift functions:
    EXPECT_EQ(LDOC_NDE_NULL, nde->prnt);
    EXPECT_EQ(LDOC_NDE_PAR, nde->tpe);
    
    ldoc_nde_free(nde);
}

TEST(ldoc_document, empty_entity)
{
    ldoc_ent_t* ent = ldoc_ent_new(LDOC_ENT_TXT);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    EXPECT_EQ(LDOC_ENT_TXT, ent->tpe);
    
    ldoc_ent_free(ent);
}

TEST(ldoc_document, small_document)
{
    ldoc_doc_t* doc = ldoc_doc_new();
    EXPECT_NE(NULL, (LDOC_NULLTYPE)doc);
    
    ldoc_nde_t* nde = ldoc_nde_new(LDOC_NDE_UA);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)nde);
    ldoc_nde_dsc_push(doc->rt, nde);
    
    ldoc_ent_t* ent = ldoc_ent_new(LDOC_ENT_TXT);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    
    EXPECT_EQ(0, nde->dsc_cnt);
    EXPECT_EQ(0, nde->ent_cnt);
    ldoc_nde_ent_push(nde, ent);
    EXPECT_EQ(0, nde->dsc_cnt);
    EXPECT_EQ(1, nde->ent_cnt);
    
    ldoc_doc_free(doc);
}

TEST(ldoc_document, big_document)
{
}

TEST(ldoc_document, format_html)
{
    ldoc_doc_t* doc = ldoc_big_doc();
    
    ldoc_vis_nde_ord_t* vis_nde = ldoc_vis_nde_ord_new();
    vis_nde->vis_setup = ldoc_vis_setup_html;
    vis_nde->vis_teardown = ldoc_vis_teardown_html;
    ldoc_vis_nde_uni(&(vis_nde->pre), ldoc_vis_nde_pre_html);
    ldoc_vis_nde_uni(&(vis_nde->infx), ldoc_vis_nde_infx_html);
    ldoc_vis_nde_uni(&(vis_nde->post), ldoc_vis_nde_post_html);
    
    ldoc_vis_ent_t* vis_ent = ldoc_vis_ent_new();
    ldoc_vis_ent_uni(vis_ent, ldoc_vis_ent_html);
    
    ldoc_ser_t* ser = ldoc_format(doc, vis_nde, vis_ent);
    
    printf("%s", ser->pld.str);
    
    ldoc_doc_free(doc);
}

TEST(ldoc_document, format_json)
{
    ldoc_doc_t* doc = ldoc_big_doc();
    
    ldoc_vis_nde_ord_t* vis_nde = ldoc_vis_nde_ord_new();
    vis_nde->vis_setup = ldoc_vis_setup_json;
    vis_nde->vis_teardown = ldoc_vis_teardown_json;
    ldoc_vis_nde_uni(&(vis_nde->pre), ldoc_vis_nde_pre_json);
    ldoc_vis_nde_uni(&(vis_nde->infx), ldoc_vis_nde_infx_json);
    ldoc_vis_nde_uni(&(vis_nde->post), ldoc_vis_nde_post_json);
    
    ldoc_vis_ent_t* vis_ent = ldoc_vis_ent_new();
    ldoc_vis_ent_uni(vis_ent, ldoc_vis_ent_json);
    
    ldoc_ser_t* ser = ldoc_format(doc, vis_nde, vis_ent);
    
    printf("%s\n", ser->pld.str);
    
    ldoc_doc_free(doc);
}

TEST(ldoc_document, format_json_custom_ids)
{
    ldoc_doc_t* doc = ldoc_doc_new();
    EXPECT_NE(NULL, (LDOC_NULLTYPE)doc);
    
    ldoc_nde_t* nde = ldoc_nde_new(LDOC_NDE_UA);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)nde);
    nde->mkup.anno.str = strdup("NID");
    ldoc_nde_dsc_push(doc->rt, nde);
    
    ldoc_ent_t* ent = ldoc_ent_new(LDOC_ENT_OR);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    
    ent->pld.pair.anno.str = strdup("CID");
    ent->pld.pair.dtm.str = strdup("data");
    
    ldoc_nde_ent_push(nde, ent);
    
    ldoc_vis_nde_ord_t* vis_nde = ldoc_vis_nde_ord_new();
    vis_nde->vis_setup = ldoc_vis_setup_json;
    vis_nde->vis_teardown = ldoc_vis_teardown_json;
    ldoc_vis_nde_uni(&(vis_nde->pre), ldoc_vis_nde_pre_json);
    ldoc_vis_nde_uni(&(vis_nde->infx), ldoc_vis_nde_infx_json);
    ldoc_vis_nde_uni(&(vis_nde->post), ldoc_vis_nde_post_json);
    
    ldoc_vis_ent_t* vis_ent = ldoc_vis_ent_new();
    ldoc_vis_ent_uni(vis_ent, ldoc_vis_ent_json);
    
    ldoc_ser_t* ser = ldoc_format(doc, vis_nde, vis_ent);
    
    EXPECT_STREQ("{\"NID\":{\"CID\":\"data\"}}", ser->pld.str);
    
    ldoc_doc_free(doc);
}

TEST(ldoc_document, format_json_null)
{
    ldoc_doc_t* doc = ldoc_doc_new();
    EXPECT_NE(NULL, (LDOC_NULLTYPE)doc);
    
    ldoc_nde_t* nde = ldoc_nde_new(LDOC_NDE_UA);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)nde);
    nde->mkup.anno.str = strdup("NID");
    ldoc_nde_dsc_push(doc->rt, nde);
    
    // Simple entity:
    ldoc_ent_t* ent = ldoc_ent_new(LDOC_ENT_TXT);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    
    ent->pld.str = NULL;
    
    ldoc_nde_ent_push(nde, ent);

    // Custom annotation:
    ent = ldoc_ent_new(LDOC_ENT_OR);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    
    ent->pld.pair.anno.str = strdup("CID");
    ent->pld.pair.dtm.str = NULL;
    
    ldoc_nde_ent_push(nde, ent);

    ldoc_vis_nde_ord_t* vis_nde = ldoc_vis_nde_ord_new();
    vis_nde->vis_setup = ldoc_vis_setup_json;
    vis_nde->vis_teardown = ldoc_vis_teardown_json;
    ldoc_vis_nde_uni(&(vis_nde->pre), ldoc_vis_nde_pre_json);
    ldoc_vis_nde_uni(&(vis_nde->infx), ldoc_vis_nde_infx_json);
    ldoc_vis_nde_uni(&(vis_nde->post), ldoc_vis_nde_post_json);
    
    ldoc_vis_ent_t* vis_ent = ldoc_vis_ent_new();
    ldoc_vis_ent_uni(vis_ent, ldoc_vis_ent_json);
    
    ldoc_ser_t* ser = ldoc_format(doc, vis_nde, vis_ent);
    
    // Count number of "null" occurrences (without the quotes):
    char* str = ser->pld.str;
    uint32_t null_cnt = 0;
    while (*str)
    {
        if (!strncmp(str++, "null", 4))
            null_cnt++;
    }
    
    EXPECT_EQ(2, null_cnt);
    
    ldoc_doc_free(doc);
}

TEST(ldoc_document, format_json_references)
{
    ldoc_doc_t* doc = ldoc_doc_new();
    EXPECT_NE(NULL, (LDOC_NULLTYPE)doc);
    
    ldoc_nde_t* nde = ldoc_nde_new(LDOC_NDE_UA);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)nde);
    nde->mkup.anno.str = strdup("NID");
    ldoc_nde_dsc_push(doc->rt, nde);
    
    // Ontology reference:
    ldoc_ent_t* ent = ldoc_ent_new(LDOC_ENT_OR);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    
    ent->pld.pair.anno.str = strdup(ldoc_ref_or_k);
    ent->pld.pair.dtm.str = strdup(ldoc_ref_or_v);
    
    ldoc_nde_ent_push(nde, ent);
    
    // Number reference:
    ent = ldoc_ent_new(LDOC_ENT_NR);
    EXPECT_NE(NULL, (LDOC_NULLTYPE)ent);
    
    ent->pld.pair.anno.str = strdup(ldoc_ref_nr_k);
    ent->pld.pair.dtm.str = strdup(ldoc_ref_nr_v);
    
    ldoc_nde_ent_push(nde, ent);
    
    ldoc_vis_nde_ord_t* vis_nde = ldoc_vis_nde_ord_new();
    vis_nde->vis_setup = ldoc_vis_setup_json;
    vis_nde->vis_teardown = ldoc_vis_teardown_json;
    ldoc_vis_nde_uni(&(vis_nde->pre), ldoc_vis_nde_pre_json);
    ldoc_vis_nde_uni(&(vis_nde->infx), ldoc_vis_nde_infx_json);
    ldoc_vis_nde_uni(&(vis_nde->post), ldoc_vis_nde_post_json);
    
    ldoc_vis_ent_t* vis_ent = ldoc_vis_ent_new();
    ldoc_vis_ent_uni(vis_ent, ldoc_vis_ent_json);
    
    ldoc_ser_t* ser = ldoc_format(doc, vis_nde, vis_ent);
    
    EXPECT_STREQ(ldoc_ref_json, ser->pld.str);
    
    ldoc_doc_free(doc);
}

TEST(ldoc_document, format_json_lists)
{
    ldoc_doc_t* doc = ldoc_ord_doc();
    EXPECT_NE(NULL, (LDOC_NULLTYPE)doc);
    
    ldoc_vis_nde_ord_t* vis_nde = ldoc_vis_nde_ord_new();
    vis_nde->vis_setup = ldoc_vis_setup_json;
    vis_nde->vis_teardown = ldoc_vis_teardown_json;
    ldoc_vis_nde_uni(&(vis_nde->pre), ldoc_vis_nde_pre_json);
    ldoc_vis_nde_uni(&(vis_nde->infx), ldoc_vis_nde_infx_json);
    ldoc_vis_nde_uni(&(vis_nde->post), ldoc_vis_nde_post_json);
    
    ldoc_vis_ent_t* vis_ent = ldoc_vis_ent_new();
    ldoc_vis_ent_uni(vis_ent, ldoc_vis_ent_json);
    
    ldoc_ser_t* ser = ldoc_format(doc, vis_nde, vis_ent);
    
    EXPECT_STREQ("{\"List 1\":[\"Entity 1\",\"Entity 2\",{\"Entity 3 (key)\":\"Entity 3 (value)\"},{\"Key 1\":\"Value 1\"},{\"Key 2.1\":\"Value 2.1\",\"Key 2.2\":\"Value 2.2\"},{}],\"List 2\":[[{\"Key 4.1\":\"Value 4.1\"}]]}", ser->pld.str);
    
    ldoc_doc_free(doc);
}

TEST(ldoc_document, format_json_multiple_lists)
{
    ldoc_doc_t* doc = ldoc_mul_ord_doc();
    EXPECT_NE(NULL, (LDOC_NULLTYPE)doc);
    
    ldoc_vis_nde_ord_t* vis_nde = ldoc_vis_nde_ord_new();
    vis_nde->vis_setup = ldoc_vis_setup_json;
    vis_nde->vis_teardown = ldoc_vis_teardown_json;
    ldoc_vis_nde_uni(&(vis_nde->pre), ldoc_vis_nde_pre_json);
    ldoc_vis_nde_uni(&(vis_nde->infx), ldoc_vis_nde_infx_json);
    ldoc_vis_nde_uni(&(vis_nde->post), ldoc_vis_nde_post_json);
    
    ldoc_vis_ent_t* vis_ent = ldoc_vis_ent_new();
    ldoc_vis_ent_uni(vis_ent, ldoc_vis_ent_json);
    
    ldoc_ser_t* ser = ldoc_format(doc, vis_nde, vis_ent);
    
    EXPECT_STREQ("{\"List 1\":[\"Entity 1\"],\"List 2\":[\"Entity 2\"],\"List 3\":[{\"Entity 3 (key)\":\"Entity 3 (value)\"},{\"Key 1\":\"Value 1\"},{\"Key 2.1\":\"Value 2.1\",\"Key 2.2\":\"Value 2.2\"},{}]}", ser->pld.str);
    
    ldoc_doc_free(doc);
}

//
// Python
//

TEST(ldoc_document, format_py)
{
    ldoc_doc_t* doc = ldoc_big_doc();
    
    ldoc_vis_nde_ord_t* vis_nde = ldoc_vis_nde_ord_new();
    vis_nde->vis_setup = ldoc_vis_setup_py;
    vis_nde->vis_teardown = ldoc_vis_teardown_py;
    ldoc_vis_nde_uni(&(vis_nde->pre), ldoc_vis_nde_pre_py);
    ldoc_vis_nde_uni(&(vis_nde->infx), ldoc_vis_nde_infx_py);
    ldoc_vis_nde_uni(&(vis_nde->post), ldoc_vis_nde_post_py);
    
    ldoc_vis_ent_t* vis_ent = ldoc_vis_ent_new();
    ldoc_vis_ent_uni(vis_ent, ldoc_vis_ent_py);
    
    ldoc_ser_t* ser = ldoc_format(doc, vis_nde, vis_ent);
    EXPECT_NE(LDOC_SER_NULL, ser);
    EXPECT_EQ(LDOC_SER_PY_DCT, ser->tpe);
    EXPECT_EQ((PyObject*)NULL, ser->pld.py.anno);
    EXPECT_NE((PyObject*)NULL, ser->pld.py.dtm);
    
    Py_Initialize();
    
    char* s = ldoc_py2str(ser->pld.py.dtm);
    EXPECT_NE((char*)NULL, s);
    EXPECT_NE(0, strlen(s));
    
    printf("%s\n", s);
    
    Py_Finalize();
    
    ldoc_doc_free(doc);
}

TEST(ldoc_document, format_py_multiple_lists)
{
    ldoc_doc_t* doc = ldoc_mul_ord_doc();
    EXPECT_NE(NULL, (LDOC_NULLTYPE)doc);
    
    ldoc_vis_nde_ord_t* vis_nde = ldoc_vis_nde_ord_new();
    vis_nde->vis_setup = ldoc_vis_setup_py;
    vis_nde->vis_teardown = ldoc_vis_teardown_py;
    ldoc_vis_nde_uni(&(vis_nde->pre), ldoc_vis_nde_pre_py);
    ldoc_vis_nde_uni(&(vis_nde->infx), ldoc_vis_nde_infx_py);
    ldoc_vis_nde_uni(&(vis_nde->post), ldoc_vis_nde_post_py);
    
    ldoc_vis_ent_t* vis_ent = ldoc_vis_ent_new();
    ldoc_vis_ent_uni(vis_ent, ldoc_vis_ent_py);
    
    ldoc_ser_t* ser = ldoc_format(doc, vis_nde, vis_ent);
    EXPECT_NE(LDOC_SER_NULL, ser);
    EXPECT_EQ(LDOC_SER_PY_DCT, ser->tpe);
    EXPECT_EQ((PyObject*)NULL, ser->pld.py.anno);
    EXPECT_NE((PyObject*)NULL, ser->pld.py.dtm);
    
    Py_Initialize();
    
    char* s = ldoc_py2str(ser->pld.py.dtm);
    EXPECT_NE((char*)NULL, s);
    EXPECT_NE(0, strlen(s));
    
    // Ordering of keys changes in Python, so only do a superficial check here:
    EXPECT_EQ(156, strlen(s));
    
    printf("%s\n", s);
    
    Py_Finalize();
    
    ldoc_doc_free(doc);
}

TEST(ldoc_document, py_import)
{
    Py_Initialize();
    
    PyObject* nme = PyUnicode_FromString("testpy.test");
    PyObject* mod = PyImport_Import(nme);
    Py_DECREF(nme);
    
    if (!mod)
    {
        // TODO Error handling.
    }
    
    PyObject* fn = PyObject_GetAttrString(mod, "example_dict");
    
    if (fn && PyCallable_Check(fn))
    {
        PyObject* dict = PyObject_CallObject(fn, NULL);
        EXPECT_NE((PyObject*)NULL, dict);
        
        ldoc_doc_t* doc = ldoc_pydict2doc(dict);
        EXPECT_NE((ldoc_doc_t*)NULL, doc);
        
        ldoc_vis_nde_ord_t* vis_nde = ldoc_vis_nde_ord_new();
        vis_nde->vis_setup = ldoc_vis_setup_json;
        vis_nde->vis_teardown = ldoc_vis_teardown_json;
        ldoc_vis_nde_uni(&(vis_nde->pre), ldoc_vis_nde_pre_json);
        ldoc_vis_nde_uni(&(vis_nde->infx), ldoc_vis_nde_infx_json);
        ldoc_vis_nde_uni(&(vis_nde->post), ldoc_vis_nde_post_json);
        
        ldoc_vis_ent_t* vis_ent = ldoc_vis_ent_new();
        ldoc_vis_ent_uni(vis_ent, ldoc_vis_ent_json);
        
        ldoc_ser_t* ser = ldoc_format(doc, vis_nde, vis_ent);
        EXPECT_NE((ldoc_ser_t*)NULL, ser);
        
        /* Python returns:
         { 'a' : True,
         'b' : 123,
         'c' : 12.3,
         'd' : { 'd1' : 'd2', 'd3' : 456 },
         'e' : [ 'e1', { 'e2' : 'e3' }]
         }
         */
        printf("%s\n", ser->pld.str);
        
        // Keys can be of any order in Python; superficial test for now:
        EXPECT_EQ(75, strlen(ser->pld.str));
        
        ldoc_doc_free(doc);
    }
    
    Py_Finalize();
}

////

TEST(ldoc_document, find_by_position)
{

}

TEST(ldoc_document, find_by_keyword)
{

}


