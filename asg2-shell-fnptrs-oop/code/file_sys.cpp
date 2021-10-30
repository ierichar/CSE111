// $Id: file_sys.cpp,v 1.10 2021-04-10 14:23:40-07 - - $

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <iomanip>
#include <typeinfo>

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

   inode_ptr virtual_inode_ptr = make_shared<inode>(file_type::DIRECTORY_TYPE);
   virtual_inode_ptr->contents = make_shared<directory>();
   inode_ptr root_inode_ptr = virtual_inode_ptr->contents->mkdir(*this, "/");

   // Insert inode directory
   // root_inode_ptr->inode_nr = root_inode_ptr->next_inode_nr;
   // root_inode_ptr->next_inode_nr;
   // Initialize inode_state
   this->root = root_inode_ptr; // ..
   this->cwd = root_inode_ptr;  // .
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

void inode_state::set_prompt_(const string word){
   this->prompt_ = word;
}


void inode_state::go_to_root(void) {
   // Reset pathname to "/"
   while (pathname.size() > 1) {
      pathname.pop_back();
   }
}

void inode_state::add_filepath(const string& recent_directory) {
   // push onto back of filepath the most recent directory
   pathname.push_back(recent_directory);
}

string inode_state::pop_filepath(void) {
   // pop back pathname by one and return popped string
   string last_directory = pathname[pathname.size() - 1];
   pathname.pop_back();
   return last_directory;
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

inode_ptr base_file::mkdir (inode_state&, const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkfile (inode_state&, const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::get_directory_inode (const string&) {
   throw file_error ("is a " + error_file_type());
}


inode_ptr base_file::get_file_inode (const inode_state&, const string&) {
   throw file_error ("is a " + error_file_type());
}

map<string,inode_ptr>& base_file::get_dirents (void) {
   throw file_error ("is a " + error_file_type());
}

void base_file::print_directory_ls(void) {
   throw file_error ("is a " + error_file_type());
}

void base_file::print_directory_lsr(void) {
   throw file_error ("is a " + error_file_type());
}

bool base_file::isDirectory(void) {
   throw file_error ("is a " + error_file_type());
}


size_t plain_file::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   cout << this->data;
   return data;
}

void plain_file::writefile (const wordvec& words) {
    DEBUGF ('i', words);

   //inode_ptr new_inode_ptr = state.get_cwd()->get_contents()->get_file_inode(state, words[0]);
   //wordvec newdata = words;
   //newdata.erase(newdata.begin(), newdata.begin() + 1);
   //new_inode_ptr->get_contents()->this.data = newdata;
   this->data = words;
   //get dirents[filename]
}

bool plain_file::isDirectory(void) {
   // :(
   return false;
}

size_t directory::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
}

inode_ptr directory::mkdir (inode_state& state, const string& dirname) {
   DEBUGF ('i', dirname);
   try {
      if (dirname == "." || dirname == "..") {
         throw base_file_error();
      }
      // Checks if directory does not exist and is root directory
      if (dirents.find(dirname) == dirents.end() && dirname == "/") {

         // IMPORTANT SYNTAX 10-28-2021
         inode_ptr new_inode_ptr { make_shared<inode>(file_type::DIRECTORY_TYPE) };

         // Insert root directory, "." and ".."
         cout << "mkdir(): " << dirname << " inserting at " << new_inode_ptr << endl;
         dirents.insert( pair<string,inode_ptr>(dirname, new_inode_ptr) );
         cout << "mkdir(): . inserting at " << new_inode_ptr << endl;
         new_inode_ptr->get_contents()->get_dirents().insert( pair<string,inode_ptr>(".", new_inode_ptr) );
         cout << "mkdir(): .. inserting at " << new_inode_ptr << endl;
         new_inode_ptr->get_contents()->get_dirents().insert( pair<string,inode_ptr>("..", new_inode_ptr) );

         return new_inode_ptr;
      } 
      else if (dirents.find(dirname) != dirents.end()) {
         throw base_file_error();
      } 
      else {
         // Create new inode of directory type

         // file_type directory_type {1};
         // inode* new_inode = new inode(directory_type);
         // inode_ptr new_inode_ptr (new_inode);
         // new_inode_ptr->contents = make_shared<directory>();

         inode_ptr new_inode_ptr { make_shared<inode>(file_type::DIRECTORY_TYPE) };

         // Insert inode directory to send to fn_mkdir
         cout << "mkdir(): " << dirname << " inserting at " << new_inode_ptr << endl;
         dirents.insert( pair<string,inode_ptr>(dirname, new_inode_ptr) );
         cout << "mkdir(): . inserting at " << new_inode_ptr << endl;
         new_inode_ptr->get_contents()->get_dirents().insert( pair<string,inode_ptr>(".", new_inode_ptr) );
         cout << "mkdir(): .. inserting at " << state.get_cwd() << endl;
         new_inode_ptr->get_contents()->get_dirents().insert( pair<string,inode_ptr>("..", state.get_cwd()) );

         return new_inode_ptr;
      }
   }
   catch (base_file_error&) {
      cout << "directory error: " << dirname << " already exists";
      cout << endl;
      return nullptr;
   }
}

inode_ptr directory::mkfile (inode_state& state, const string& filename) {
   DEBUGF ('i', filename);
   //cout << "mkfile() current filename is " << filename << endl;
   //create an inode with plain_type, for files

         //does file name already exist?
         if(dirents.find(filename) != dirents.end()){
            inode_ptr existing_inode = get_file_inode(state, filename);
            return existing_inode;
         }
         else{
            //create new file
            //slap it onto dirents then
            
            inode_ptr new_inode { make_shared<inode>(file_type::DIRECTORY_TYPE) };
            new_inode->contents = make_shared<plain_file>();
            dirents.insert( pair<string, inode_ptr> (filename, new_inode) );
            return new_inode;
         }
}

inode_ptr directory::get_directory_inode (const string& dirname) {
   DEBUGF ('i', dirname);
   try {
      cout << "get_directory_inode(): passing " << dirname << endl;
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

inode_ptr directory::get_file_inode (const inode_state& state, const string& filename) {
   DEBUGF ('i', filename);
   try {
      //cout << "get_file_inode(): passing " << filename << endl;
      if (dirents.find(filename) == dirents.end()) {
         throw base_file_error();
      }
      return dirents[filename];
   } catch (base_file_error&) {
      cout << "file error: " << filename << " does not exist";
      cout << endl;
      return nullptr;
   }
}

map<string,inode_ptr>& directory::get_dirents (void) {
   return dirents;
}

void directory::print_directory_ls(void) {
   for (auto i = dirents.begin(); i != dirents.end(); ++i) {
      // inode_nr
      cout << right << setw(6) << i->second->get_inode_nr();
      // size()
      cout << right << setw(6) << i->second->get_contents()->size();
      // filename + '/'
      cout << left << " " << i->first << left << '/' << endl;
   }
}

void directory::print_directory_lsr(void) {
   // pre-order print directory tree nodes
   this->print_directory_ls();

   // create testcase of type make_shared<directory> to compare contents
   // of arbitrary inode_ptrs in the directory map
   base_file_ptr testcase = make_shared<directory>();

   for (auto i = dirents.begin(); i != dirents.end(); ++i) {
      if (i->second->get_contents()->isDirectory() 
            && i->first != "." && i->first != "..") {
         // recursively access subdirectories and print them out
         // in pre-order
         cout << i->first;
         if (i->first != "/") {
            cout << "/";
         } 
         cout << ":" << endl;
         i->second->get_contents()->print_directory_lsr();
      }
   }
}

bool directory::isDirectory(void) {
   // :)
   return true;
}