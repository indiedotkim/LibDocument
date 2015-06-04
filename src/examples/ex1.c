#include <document.h>

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
