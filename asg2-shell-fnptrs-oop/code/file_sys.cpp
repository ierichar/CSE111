// $Id: file_sys.cpp,v 1.10 2021-04-10 14:23:40-07 - - $

#include <cassert>
#include <iostream>
#include <stdexcept>

using namespace std;

#include "debug.h"
#include "file_sys.h"

size_t inode::next_inode_nr {1};

// inode exceptions
class inode_error : public exception {};
void do_error (inode&, const string) {
   throw inode_error();
}

// base_file exceptions
class base_file_error : public exception {};
void do_error (base_file&, const string) {
   throw base_file_error();
}
class base_file_quit : public exception {};
void do_quit (base_file&, const string) {
   throw base_file_quit();
}

ostream& operator<< (ostream& out, file_type type) {
   switch (type) {
      case file_type::PLAIN_TYPE: out << "PLAIN_TYPE"; break;
      case file_type::DIRECTORY_TYPE: out << "DIRECTORY_TYPE"; break;
      default: assert (false);
   };
   return out;
}

inode_state::inode_state() {
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");
   // initialize inode_state AND inode attributes
   // INODE_STATE
   // inode_ptr root = inode_state (initial inode state)
   // inode_ptr cwd = root;
   // string prompt_ = prompt()

   // Initialize inode_ptr from inode
   // base_file_ptr contents = 
   //    (a directory of <string = "/", inode_ptr = itself)

   // Create new node with directory_type
   file_type directory_type {1};
   inode* new_inode = new inode(directory_type);
   inode_ptr new_inode_ptr (new_inode);
   // Create new directory_ptr from directory named root "/"
   directory* new_directory = new directory();
   new_directory->mkdir("/");
   directory_ptr new_directory_ptr (new_directory);

   // Insert inode directory
   new_inode->contents = new_directory_ptr;
   new_inode->inode_nr = new_inode->next_inode_nr;
   new_inode->next_inode_nr++;
   // Initialize inode_state
   this->root = new_inode_ptr;
   this->cwd = new_inode_ptr;
   this->prompt_ = prompt();
}

inode_ptr inode_state::get_root() const {
   DEBUGF ('i', "root = " << root);
   return root;
}

inode_ptr inode_state::get_cwd() const {
   DEBUGF ('i', "cwd = " << cwd);
   return cwd;
}

void inode_state::set_cwd(inode_ptr cwd) {
   this->cwd = cwd;
}

const string& inode_state::prompt() const { return prompt_; }

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           break;
      default: assert (false);
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

size_t inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

size_t inode::get_next_inode() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return next_inode_nr;
}

void inode::increment_nr() {
   size_t temp = next_inode_nr;
   inode_nr = temp;
   next_inode_nr++;
}

void inode::decrement_nr() {
   try {
      if (inode_nr == 1) {
         throw ysh_error();
      }
      next_inode_nr--;
      inode_nr--;
   } catch (ysh_error&) {
      cout << "directory error: " << dirname << " already exists" << endl;
   }
}


file_error::file_error (const string& what):
            runtime_error (what) {
}

const wordvec& base_file::readfile() const {
   throw file_error ("is a " + error_file_type());
}

void base_file::writefile (const wordvec&) {
   throw file_error ("is a " + error_file_type());
}

void base_file::remove (const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkdir (const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkfile (const string&) {
   throw file_error ("is a " + error_file_type());
}


size_t plain_file::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
}

size_t directory::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
}

inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);
   try {
      // Checks if directory already exists
      if (dirents.find(dirname) != dirents.end()) {
         throw base_file_error();
      }
      // Create new inode of directory type
      file_type directory_type {1};
      inode* new_inode = new inode(directory_type);
      inode_ptr new_inode_ptr (new_inode);
      // Insert inode directory to send to fn_mkdir
      dirents.insert( pair<string,inode_ptr>(dirname, new_inode_ptr) );
      return new_inode_ptr;
   }
   catch (base_file_error&) {
      cout << "directory error: " << dirname << " already exists" << endl;
      return nullptr;
   }
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   return nullptr;
}

