// $Id: listmap.tcc,v 1.15 2019-10-30 12:44:53-07 - - $

#include "listmap.h"
#include "debug.h"

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
   while (not empty()) {
      erase(begin());
   }
}

//
// iterator listmap::insert (const value_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::insert (const value_type& pair) {
   node* currN = anchor()->next;
   while(currN != anchor()){
      // !less is the same as >=
      // Current Node needs to have a larger value
      if(!less(currN->value.first,pair.first)){
         if(!less(pair.first,currN->value.first)){
            currN->value.second = pair.second;
            return currN;
         }
         else
            break;
      }
      currN = currN->next;
   }
   // new entry needed
   node* newN =  new node(currN, currN->prev, pair);
   currN->prev->next = newN;
   currN->prev = newN;

   DEBUGF ('l', &pair << "->" << pair);
   return newN;
}

//
// listmap::find(const key_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::find (const key_type& that) {
   DEBUGF ('l', that);
   listmap<key_t,mapped_t,less_t>::iterator rit = begin();
   while (rit != end()){
      if(rit->first == that)
         return rit;
      ++rit;
   }
   return end();
}

//
// iterator listmap::erase (iterator position)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::erase (iterator position) {
   DEBUGF ('l', &*position);
   position.erase();
   return iterator();
}


