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
// listmap::~listmap()
//
template <typename key_t, typename mapped_t, class less_t>
listmap<key_t,mapped_t,less_t>::~listmap() {
   DEBUGF ('l', reinterpret_cast<const void*> (this));
   // Repeatedly call erase until map is empty
   iterator itor = iterator(begin()); 
   while (not empty()) {
      itor = erase(itor);
   }
   delete(this);
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
   iterator itor = iterator(begin());
   // Insert node between anchors if empty
   if (empty()) {
      next = anchor();
      prev = anchor();
   }
   // Determine where to insert node
   else {
      // itor.where
      for (; itor != end(); ++itor) {
         if (less(pair.first, pair.first)) {
            // Perform insertion
            next = itor.where;
            prev = itor.where->prev;
            itor.where->prev->next = new_node;
            itor.where->prev = new_node;
            break;
         }
      }
      // If the new key is greater than all
      if (next == nullptr && prev == nullptr) {
            next = itor.where->next;
            prev = itor.where;
            itor.where->next->prev = new_node;
            itor.where->next = new_node; 
      }
   }
   // Construct a new node
   new_node = new node(
      next,
      prev,
      pair
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
   iterator itor = iterator(begin());
   bool found = false;
   for (; itor != end() and not found; ++itor)
      if (itor.where->first == that.first) found = true;
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
   iterator next_pos = iterator(position.where->next);
   // Perform removal
   node* temp = position.where;
   position.where->next->prev = position.where->prev;
   position.where->prev->next = position.where->next;
   position.where->next = nullptr;
   position.where->prev = nullptr;
   delete (temp);
   return next_pos;
}
