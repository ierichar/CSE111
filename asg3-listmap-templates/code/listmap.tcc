// $Id: listmap.tcc,v 1.16 2021-11-10 22:30:15-08 - - $

#include "listmap.h"
#include "debug.h"
#include <iostream>
using namespace std;

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
   iterator itor = begin(); 
   while (not empty())
      itor = erase(itor);
   delete(this);
}

//
// iterator listmap::insert(const value_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::insert (const value_type& pair) {
   DEBUGF ('l', &pair << "->" << pair);
   node* next = nullptr;
   node* prev = nullptr;
   node* new_node = new node(
      next,
      prev,
      pair
   );
   cout << "insert(): new node created" << endl;
   iterator itor = begin();
   bool inserted = false;
   // Insert node between anchors if empty
   if (empty()) {
      new_node->next = anchor();
      new_node->prev = anchor();
      itor.where->next = new_node;
      itor.where->prev = new_node;
      itor = new_node;
      inserted = true;
      cout << "insert(): map is empty" << endl;
   }
   // Determine where to insert node
   else {
      cout << "insert(): comparing to other keys" << endl;
      // itor.where
      for (; itor != anchor() and not inserted; ++itor) {
         cout << "insert(): checking " << pair.first << " = " << pair.second << endl;
         if (itor != anchor() and less(pair.first, itor.where->value.first)) {
            cout << "insert(): inserting before" << pair.first << endl;
            // Perform insertion
            new_node->next = itor.where;
            new_node->prev = itor.where->prev;
            itor.where->prev->next = new_node;
            itor.where->prev = new_node;
            itor = new_node;
            inserted = true;
         }
      }
   }
   // If the new key is greater than all
   if (not inserted) {
      cout << "insert(): new node is greater than all" << endl;
      new_node->next = itor.where->next;
      new_node->prev = itor.where;
      itor.where->next->prev = new_node;
      itor.where->next = new_node;
      itor = new_node;
   }
   cout << "insert(): return iterator" << endl;
   // Construct a new node
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
   for (; itor != end(); ++itor) {
      if (itor != end() and itor.where->value.first == that) {
         return itor;
      }
   }
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
   iterator next_pos = position.where->next;
   // Perform removal
   node* temp = position.where;
   position.where->next->prev = position.where->prev;
   position.where->prev->next = position.where->next;
   position.where->next = nullptr;
   position.where->prev = nullptr;
   delete (temp);
   return next_pos;
}
