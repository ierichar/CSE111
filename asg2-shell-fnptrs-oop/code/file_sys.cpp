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

   // Create new inode with directory_type
   file_type directory_type {file_type::DIRECTORY_TYPE};
   inode new_inode = inode(directory_type);
   inode_ptr new_inode_ptr (&new_inode);

   // Create new directory_ptr from directory named root "/"
   directory new_directory = directory();

   // inode_state initialization
   this->root = new_directory.mkdir("/");
   this->cwd = this->root;
   this->root->contents = directory_ptr;
   this->prompt_ = prompt();
   this->pathname.push_back("/");

   cout << "next_inode_nr" << this->root->next_inode_nr << endl;
   cout << "inode_nr: " << this->root->inode_nr << endl;
}

const string& inode_state::prompt() const { return prompt_; }

inode_ptr inode_state::get_root() const {
   DEBUGF ('i', "root = " << root);
   return root;
}

inode_ptr inode_state::get_cwd() const {
   DEBUGF ('i', "cwd = " << cwd);
   return cwd;
}

string inode_state::pathname_to_string() const {
   DEBUGF ('i', "pathname = " << pathname);
   string filepath = "";
   for (size_t i = 0; i < pathname.size(); i++) {
      if (pathname[i] == "/") {
         filepath += pathname[i];
      } else if (i == pathname.size() - 1){
         filepath += pathname[i];
      } else {
         filepath += pathname[i] + "/";
      }
   }
   return filepath;
}

void inode_state::set_cwd(const inode_ptr cwd_) {
   this->cwd = cwd_;
}

void inode_state::go_to_root(void) {
   // Reset pathname to "/"
   while (pathname.size() > 1) {
      pathname.pop_back();
   }
}

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

size_t inode::get_inode_nr(void) const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

size_t inode::get_next_inode(void) const {
   DEBUGF ('i', "next_inode = " << next_inode_nr);
   return next_inode_nr;
}

base_file_ptr inode::get_contents(void) const {
   DEBUGF ('i', "contents = " << contents);
   return contents;
}

void inode::set_inode_nr(const size_t inode_nr_) {
   DEBUGF ('i', "inode_nr = " << inode_nr_);
   this->inode_nr = inode_nr_;
}

void inode::set_next_inode_nr(const size_t next_inode_nr_) {
   DEBUGF ('i', "next_inode_nr = " << next_inode_nr_);
   this->next_inode_nr = next_inode_nr_;
}

void inode::set_contents(const base_file_ptr contents_) {
   DEBUGF ('i', "contents = " << contents_);
   this->contents = contents_;
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

inode_ptr base_file::get_directory_inode (const string&) {
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
      if (dirname == "." || dirname == "..") {
         throw base_file_error();
      }
      // Checks if directory does not exist and is root directory
      if (dirents.find(dirname) == dirents.end() && dirname == "/") {
         // Create new inode of directory type
         file_type directory_type {file_type::DIRECTORY_TYPE};
         inode new_inode = inode(directory_type);
         inode_ptr new_inode_ptr (&new_inode);

         cout << "mkdir(): C1 storing dirname " << dirname << endl;
         dirents.insert( pair<string,inode_ptr>(dirname, new_inode_ptr) );

         // Create subdirectories '.' and '..'
         new_inode_ptr->contents->dirents.insert( 
            pair<string,inode_ptr>(".", new_inode_ptr) 
         );
         new_inode_ptr->contents->dirents.insert(
            pair<string,inode_ptr>("..", new_inode_ptr)
         );

         return new_inode_ptr;
      } 
      else if (dirents.find(dirname) != dirents.end()) {
         throw base_file_error();
      } 
      else {
         // Create new inode of directory type
         file_type directory_type {file_type::DIRECTORY_TYPE};
         inode new_inode = inode(directory_type);
         inode_ptr new_inode_ptr (&new_inode);

         // Insert inode directory to send to fn_mkdir
         cout << "mkdir(): C2 storing dirname " << dirname << endl;
         dirents.insert( pair<string,inode_ptr>(dirname, new_inode_ptr) );

         // create subdirectories '.' and '..'
         new_inode_ptr->contents->dirents.insert(
            pair<string,inode_ptr>(".", new_inode_ptr)
         );
         new_inode_ptr->contents->dirents.insert(
            pair<string,inode_ptr>("..", this)
         );

         return new_inode_ptr;
      }
   }
   catch (base_file_error&) {
      cout << "directory error: " << dirname << " already exists";
      cout << endl;
      return nullptr;
   }
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   return nullptr;
}

inode_ptr directory::get_directory_inode (const string& dirname) {
   DEBUGF ('i', dirname);
   try {
      cout << "get_directory_inode(): passing " << dirname << endl;
      print_dirents();
      if (dirents.find(dirname) == dirents.end() && dirents.find(dirname + "/") == dirents.end()) {
         throw base_file_error();
      }
      return dirents[dirname];
   } catch (base_file_error&) {
      cout << "directory error: " << dirname << " does not exist";
      cout << endl;
      return nullptr;
   }
}

void directory::print_dirents (void) {
   for (std::map<string,inode_ptr>::iterator i = dirents.begin(); i != dirents.end(); ++i) {
      cout << "dirents.key: " << i->first << endl;
   }
}