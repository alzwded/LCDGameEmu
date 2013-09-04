#include "inputbit.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static struct _ident_mapping_s _idents[] = {
    { "alt", ALT },
    { "down", DOWN },
    { "downleft", DOWNLEFT },
    { "downright", DOWNRIGHT },
    { "fire", FIRE },
    { "left", LEFT },
    { "right", RIGHT },
    { "start", START },
    { "toggle", TOGGLE },
    { "up", UP },
    { "upleft", UPLEFT },
    { "upright", UPRIGHT },
    { NULL, 0 }
};

static int _strcmprtor(void const* a, void const* b)
{
    assert(a);
    assert(b);
    char const* left = (char const*)a;
    struct _ident_mapping_s* right = (struct _ident_mapping_s*)b;
    return strcmp(left, right->s);
}

input_bit_t str2inputbit(char const* s)
{
    struct _ident_mapping_s* rez = (struct _ident_mapping_s*)bsearch(s, _idents, sizeof(_idents) / sizeof(struct _ident_mapping_s) - 1, sizeof(struct _ident_mapping_s), &_strcmprtor);
    if(!rez) return 0;
    return rez->i;
}
