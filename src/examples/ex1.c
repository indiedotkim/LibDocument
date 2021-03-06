/*
 * Copyright (c) 2015-2016 CODAMONO, Ontario, Canada
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at: http://mozilla.org/MPL/2.0/
 */

#include <document.h>

/* TODO
 *
 * Needs at least one good example. Ideally more.
 *
 * Use this code to see whether the target compiles, but for actual
 * functionality examples look at the unit tests (`tests` target).
 */

int main(int argc, char** argv)
{
    ldoc_doc_t* doc = ldoc_doc_new();
    
    if (!doc)
        exit(1);
    
    ldoc_nde_t* nde = ldoc_nde_new(LDOC_NDE_UA);
    
    if (!nde)
        exit(2);
        
    ldoc_nde_dsc_push(doc->rt, nde);
    
    ldoc_ent_t* ent = ldoc_ent_new(LDOC_ENT_TXT);
    
    if (!ent)
        exit(3);
    
    ldoc_nde_ent_push(nde, ent);
    
    ldoc_doc_free(doc);
}
