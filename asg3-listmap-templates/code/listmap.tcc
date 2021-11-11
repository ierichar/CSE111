// $Id: listmap.tcc,v 1.16 2021-11-10 22:30:15-08 - - $

#include "listmap.h"
#include "debug.h"

#ifndef __LISTMAP_H__
#endif

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::listmap()
//
template <typename key_t, typename mapped_t, class less_t=xless<key_t>>
listmap<key_t, mapped_t, less_t>::listmap () {
    // Construction dealt with in header file?
    // Empty state is created
}

//
// listmap::listmap(const listmap&)
//
template <typename key_t, typename mapped_t, class less_t=xless<key_t>>
listmap<key_t, mapped_t, less_t>::listmap (const listmap& new_listmap) {
    less = new_listmap.less;
    // might want to rewrite to utilize iterator
    node* current_node = new_listmap.anchor()->next;
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

//
// listmap::operator=(const listmap&)
//
template <typename key_t, typename mapped_t, class less_t=xless<key_t>>
listmap& listmap<key_t, mapped_t, less_t>::operator= (
    const listmap& copy_listmap) {
        listmap* new_listmap = new listmap(copy_listmap);
        return new_listmap;
}

//
// listmap::~listmap()
//
template <typename key_t, typename mapped_t, class less_t>
listmap<key_t,mapped_t,less_t>::~listmap() {
   DEBUGF ('l', reinterpret_cast<const void*> (this));
   // Repeatedly call erase until map is empty
   iterator itor = begin(); 
   while (not empty()) {
      itor = erase(itor);
   }
}

//
// iterator listmap::insert(const value_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::insert (const value_type& pair) {
   DEBUGF ('l', &pair << "->" << pair);
   node* new_node = nullptr;
   node* next = nullptr;
   node* prev = nullptr;
   iterator itor = begin();
   // Insert node between anchors if empty
   if (empty()) {
      next = anchor();
      prev = anchor();
   }
   // Determine where to insert node
   else {
      for (; *itor->next != end(); itor++) {
         if (less_t(new_value->first, *itor->first)) {
            // Perform insertion
            next = itor;
            prev = itor->prev;
            itor->prev->next = new_node;
            itor->prev = new_node;
            break;
         }
      }
      // If the new key is greater than all
      if (next = nullptr && prev = nullptr) {
            next = itor->next;
            prev = itor;
            itor->next->prev = new_node;
            itor->next = new_node; 
      }
   }
   // Construct a new node
   new_node = new node(
      next,
      prev,
      new_value
   );
   return itor;
}

//
// listmap::find(const key_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::find (const key_type& that) {
   DEBUGF ('l', that);
   iterator itor = begin();
   bool found = false;
   for (; itor != end() and not found; itor++)
      if (*itor->first == that) found = true;
   if (not found) itor = end();
   return itor;
}

//
// iterator listmap::erase (iterator position)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::erase (iterator position) {
   DEBUGF ('l', &*position);
   iterator next_pos = *position->next;
   // Perform removal
   node* temp = *position;
   *position->next->prev = *position->prev;
   *position->prev->next = *position->next;
   *position->next = nullptr;
   *position->prev = nullptr;
   delete (temp);
   return next_pos;
}
