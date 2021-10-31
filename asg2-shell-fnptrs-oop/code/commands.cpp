// Ian Richardson, ierichar
// Jackson Brazeal, jbrazeal
// $Id: commands.cpp,v 1.21 2021-09-26 12:41:17-07 - - $

#include "commands.h"
#include "debug.h"


command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"#"     , fn_pound },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
   {"rmr"   , fn_rmr   },
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   const auto result {cmd_hash.find (cmd)};
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such command");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int status {exec::status()};
   cout << exec::execname() << ": exit(" << status << ")" << endl;
   return status;
}

void fn_cat (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   wordvec newdata = words;
   newdata.erase(newdata.begin(), newdata.begin() + 1);
   size_t i = 1;
   while (i < words.size()){
      state.get_cwd()->get_contents()->\
      get_file_inode(state, words[i])->get_contents()->readfile();
      i++;
   }
   cout << endl;
}

void fn_pound (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_cd (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   // Case 1: cd
   if (words.size() == 1) {
      state.go_to_root();
      state.set_cwd(state.get_root());
      cout << state.pathname_to_string() << endl;
   }
   // Case 2: cd / 
   else if (words[1] == "/") {
      state.go_to_root();
      state.set_cwd(state.get_root());
      cout << state.pathname_to_string() << endl;
   } 
   // Case 3: cd .
   else if (words[1] == ".") {
      cout << state.pathname_to_string() << endl;
   }
   // Case 4: cd .. 
   else if (words[1] == "..") {
      // need to also include a map search for parent directory
      if (state.get_cwd() != state.get_root()) {
         state.pop_filepath();
      }
      state.set_cwd(state.get_cwd()->get_contents()->\
      get_directory_inode(".."));
      cout << state.pathname_to_string() << endl;
   }
   // Case 5: cd [filepath]
   else {
      // Save cwd
      inode_ptr placeholder = state.get_cwd();

      // Initialize search variables
      wordvec full_filepath = split(words[1], "/");
      cout << "cd full_filepath is " << full_filepath << endl;
      string directory_name;
      bool found = true;
      for (auto i = full_filepath.begin(); 
           i != full_filepath.end() && found; ++i) {
         directory_name = *i;
         cout << "cd search: " << directory_name << endl;
         cout << "cwd is: " << state.get_cwd() << endl;
         if (state.get_cwd()->get_contents()->\
            get_directory_inode(directory_name) == nullptr) {
            found = false;
         } else {
            state.set_cwd(state.get_cwd()->get_contents()->\
            get_directory_inode(directory_name));
            state.add_filepath(directory_name);
         }
      }

      if (!found) {
         state.set_cwd(placeholder);
         cout << "cannot find directory" << endl;
      }
      cout << "current state is: " << state.get_cwd() << endl;
   }
}

void fn_echo (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   try {
      // Case 1: ls
      if (words.size() == 1) {
         cout << state.pathname_to_string();
         if (state.get_cwd() != state.get_root()) {
            cout << "/";
         }
         cout << ":" << endl;
         state.get_cwd()->get_contents()->print_directory_ls();
      }
      // Case 2: ls [filepath]
      else {
          // Initialize search variables
         inode_ptr placeholder = state.get_cwd();
         wordvec full_filepath = split(words[1], "/");
         string directory_name;
         bool found = true;

         // Loop through subdirectories to find beginning for lsr
         for (auto i = full_filepath.begin();
            i != full_filepath.end() && found; ++i) {

            directory_name = *i;
            cout << "lsr search: " << directory_name << endl;
            cout << "cwd is: " << state.get_cwd() << endl;
            if (placeholder->get_contents()->\
                  get_directory_inode(directory_name) == nullptr) {
               found = false;
            } else {
               placeholder = placeholder->get_contents()->\
                  get_directory_inode(directory_name);
            }
         }
         
         // If issues, throw errors
         if (!found) {
            throw runtime_error(directory_name);
         }
         if (placeholder == nullptr) {
            throw runtime_error(words[1]);
         }

         // Print first directory with proper format, 
         // then call recursive print lsr
         cout << words[1];
         if (placeholder != state.get_root()) {
            cout << "/";
         }
         cout << ":" << endl;
         placeholder->get_contents()->print_directory_lsr();
      }
   } catch (runtime_error& error) {
      cout << "ls error: " << error.what();
      cout << " path not valid" << endl;
   }
}

void fn_lsr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   try {
      // Case 1: lsr 
      if (words.size() == 1) {
         state.get_cwd()->get_contents()->print_directory_lsr();
      } 
      // Case 2: lsr [filepath]
      else {
         // Initialize search variables
         inode_ptr placeholder = state.get_cwd();
         wordvec full_filepath = split(words[1], "/");
         string directory_name;
         bool found = true;

         // Loop through subdirectories to find beginning for lsr
         for (auto i = full_filepath.begin();
            i != full_filepath.end() && found; ++i) {

            directory_name = *i;
            cout << "lsr search: " << directory_name << endl;
            cout << "cwd is: " << state.get_cwd() << endl;
            if (placeholder->get_contents()->\
                  get_directory_inode(directory_name) == nullptr) {
               found = false;
            } else {
               placeholder = placeholder->get_contents()->\
                  get_directory_inode(directory_name);
            }
         }
         
         // If issues, throw errors
         if (!found) {
            throw runtime_error(directory_name);
         }
         if (placeholder == nullptr) {
            throw runtime_error(words[1]);
         }

         // Print first directory with proper format, 
         // then call recursive print lsr
         cout << words[1];
         if (placeholder != state.get_root()) {
            cout << "/";
         }
         cout << ":" << endl;
         placeholder->get_contents()->print_directory_lsr();
      }
   } catch (runtime_error& error) {
      cout << "lsr error: " << error.what();
      cout << " path not valid" << endl;
   }
}

void fn_make (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   
   // Calls mkfile with state pathname
   state.get_cwd()->get_contents()->mkfile(state, words[1]); 

   // Create a newdata wordvec ignoring the first 2 elements
   wordvec newdata = words;
   newdata.erase(newdata.begin(), newdata.begin() + 2);

   // Use writefile to write into the file state->contents
   state.get_cwd()->get_contents()->\
   get_file_inode(state, words[1])->get_contents()->writefile(newdata);
}

void fn_mkdir (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // Create new inode and ptr
   inode_ptr new_inode_ptr \
   { make_shared<inode>(file_type::DIRECTORY_TYPE) };

   new_inode_ptr = 
      state.get_cwd()->get_contents()->mkdir(state, words[1]);
   
   // Insert new_inode_ptr to current directory's map
   size_t new_nr = state.get_cwd()->get_next_inode() + 1;
   new_inode_ptr->set_inode_nr(state.get_cwd()->get_next_inode());
   new_inode_ptr->set_next_inode_nr(new_nr);

   cout << state << endl;
}

void fn_prompt (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // Set prompt from given input from words
   wordvec newdata = words;
   newdata.erase(newdata.begin(), newdata.begin() + 1);
   string newPrompt = "";
   size_t i = 0;
   while (i < words.size()-1){
      newPrompt += newdata[i];
      newPrompt += " ";
      i++;
   }

   state.set_prompt_(newPrompt);
}

void fn_pwd (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   // Print working directory
   cout << state.pathname_to_string() << endl;
}

void fn_rm (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   // 1. Check if searched base file exists
   // 2. Check if directory
   //    True: Check if  is empty (only contains . and ..)
   //       True: Delete directory
   //    False: Delete file
}

void fn_rmr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   // 1. Check if searched base file exists
   // 2. Check if directory
   //    True: Check if  is empty (only contains . and ..)
   //       True: Delete directory
   //    False: Delete file
}
