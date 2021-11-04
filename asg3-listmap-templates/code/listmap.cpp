// Ian Richardson, ierichar
// Jackson Brazeal, jbrazeal
// $Id: listmap.cpp,v 1.29 2021-11-03 09:20:20:23 - - $

#include "listmap.h"

template <typename key_t, typename mapped_t, class less_t=xless<key_t>>
listmap::listmap (const listmap&) {
    less = listmap.less;
    node* current_node = listmap.anchor()->next;
    while (current_node != anchor()) {
        // Insert nodes into newly created listmap
        node* new_node = new node(
            current_node->next, 
            current_node->prev, 
            current_node->value
        );
        // Move to next node
        current_node = current_node->next;
    }
}

template <typename key_t, typename mapped_t, class less_t=xless<key_t>>
listmap& operator= (const listmap& copy_listmap) {
    listmap* new_listmap = new listmap(copy_listmap);
    return new_listmap;
}