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
   size_t i = 0;
   while (i < words.size()-1){
      state.get_cwd()->get_contents()->get_file_inode(state, words[i])->get_contents()->readfile();
      i++;
   }
}

void fn_pound (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_cd (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   // Change state according to words[1] (which is a directory or . or ..)
   // words[1] to directory to retrieve inode_ptr
   // . and .. are interpreted by fn_cd
   if (words[1] == "/") {
      state.go_to_root();
      state.set_cwd(state.get_root());
      cout << state.pathname_to_string() << endl;
   } else if (words[1] == ".") {
      cout << state.pathname_to_string() << endl;
   } else if (words[1] == "..") {
      // need to also include a map search for parent directory
      if (state.get_cwd() != state.get_root()) {
         state.pop_filepath();
      }
      state.set_cwd(state.get_cwd()->get_contents()->get_directory_inode(".."));
      cout << state.pathname_to_string() << endl;
   } else {
      // cd [filepath]
      inode_ptr placeholder = state.get_cwd();

      wordvec full_filepath = split(words[1], "/");
      cout << "cd full_filepath is " << full_filepath << endl;
      string directory_name;
      bool found = true;
      for (auto i = full_filepath.begin(); i != full_filepath.end() && found; ++i) {
         directory_name = *i;
         cout << "cd search: " << directory_name << endl;
         cout << "cwd is: " << state.get_cwd() << endl;
         if (state.get_cwd()->get_contents()->get_directory_inode(directory_name) == nullptr) {
            found = false;
         } else {
            state.set_cwd(state.get_cwd()->get_contents()->get_directory_inode(directory_name));
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
      if (words.size() == 1) {
         cout << state.pathname_to_string();
         if (state.get_cwd() != state.get_root()) {
            cout << "/";
         }
         cout << ":" << endl;
         state.get_cwd()->get_contents()->print_directory_ls();
      } else {
         inode_ptr placeholder = state.get_cwd()->get_contents()->get_directory_inode(words[1]);
         if (placeholder == nullptr) {
            throw runtime_error(words[1]);
         }
         cout << words[1];
         if (placeholder != state.get_root()) {
            cout << "/";
         }
         cout << ":" << endl;
         placeholder->get_contents()->print_directory_ls();
      }
   } catch (runtime_error& error) {
      cout << "ls error: " << error.what() << " path not valid" << endl;
   }
}

void fn_lsr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   try {
      if (words.size() == 1) {
         state.get_cwd()->get_contents()->print_directory_lsr();
      } else {
         inode_ptr placeholder = state.get_cwd()->get_contents()->get_directory_inode(words[1]);
         if (placeholder == nullptr) {
            throw runtime_error(words[1]);
         }
         cout << words[1];
         if (placeholder != state.get_root()) {
            cout << "/";
         }
         cout << ":" << endl;
         placeholder->get_contents()->print_directory_lsr();
      }
   } catch (runtime_error& error) {
      cout << "lsr error: " << error.what() << " path not valid" << endl;
   }
}

void fn_make (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   
   state.get_cwd()->get_contents()->mkfile(state, words[1]); 
   //calls mkfile with state pathname

   // create a newdata wordvec ignoring the first 2 elements
   wordvec newdata = words;
   newdata.erase(newdata.begin(), newdata.begin() + 1);

   //use writefile to write into the file state->contents
   
   state.get_cwd()->get_contents()->get_file_inode(state, words[1])->get_contents()->writefile(newdata);
}

void fn_mkdir (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   // inode_state& state is changed by:
   //     wordvec& words-> directory()-> inode_ptr directory::mkdir()
   // inode_ptr is added to inode_state&
   // we need to know:
   //    size_t curr_size = state.get_inode_nr();
   //    inode

   inode_ptr new_inode_ptr { make_shared<inode>(file_type::DIRECTORY_TYPE) };
   new_inode_ptr = state.get_cwd()->get_contents()->mkdir(state, words[1]);
   
   // Insert new_inode_ptr to current directory's map
   size_t new_nr = state.get_cwd()->get_next_inode() + 1;
   new_inode_ptr->set_inode_nr(state.get_cwd()->get_next_inode());
   new_inode_ptr->set_next_inode_nr(new_nr);
   // state.set_cwd(new_inode_ptr);
   // state.add_filepath(words[1]);
   cout << state << endl;
}

void fn_prompt (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

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
   cout << state.pathname_to_string() << endl;
}

void fn_rm (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rmr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}
