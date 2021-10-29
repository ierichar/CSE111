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
      cout << state.get_pathname() << endl;
   } else if (words[1] == ".") {
      cout << state.get_pathname() << endl;
   } else if (words[1] == "..") {
      // need to also include a map search for parent directory
      if (state.get_cwd() != state.get_root()) {
         state.pop_filepath();
      }
      state.set_cwd(state.get_cwd()->get_contents()->get_directory_inode(".."));
      cout << state.get_pathname() << endl;
   } else {
      // cd [filepath]
      // requires searching from root to current path
      // the input is looking for
      // Suggest: using map in directory
      // maybe create a search_directory() function
      inode_ptr placeholder = state.get_cwd();

      state.set_cwd(state.get_root());
      wordvec full_filepath = split(words[1], "/");
      string directory_name;
      bool found = true;
      for (auto i = full_filepath.begin(); i != full_filepath.end() && found; ++i) {
         cout << "cd search: " << directory_name << endl;
         if (state.get_cwd()->get_contents()->get_directory_inode(directory_name) == nullptr) {
            found = false;
         } else {
            state.set_cwd(state.get_cwd()->get_contents()->get_directory_inode(directory_name));
         }
      }

      // while (full_filepath.size() > 0 && found) {
      //    directory_name = full_filepath.front();
      //    if (directory_name == "/") {
      //       directory_name = full_filepath.next();
      //    }
      //    cout << "cd search: " << directory_name << endl;
      //    if (state.get_cwd()->get_contents()->get_dirents()->get_directory_inode(directory_name) == nullptr) {
      //       found = false;
      //    } else {
      //       state.set_cwd(state.get_cwd()->get_contents()->get_directory_inode(directory_name));
      //    }
      //    full_filepath.erase(full_filepath.begin()+1);
      // }

      if (!found) {
         state.set_cwd(placeholder);
         cout << "cannot find directory" << endl;
      } else {
         full_filepath = split(words[1], "/");
         directory_name = full_filepath[full_filepath.size() - 1];
         state.set_cwd(state.get_cwd()->get_contents()->get_directory_inode(directory_name));
      }
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
}

void fn_lsr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   
   const string& full_document_filepath = "" + state.get_pathname() + words[1];
   state.get_cwd()->get_contents()->mkfile(full_document_filepath); //calls mkfile with state pathname
   //use writefile to write into the file state->contents
   state.get_cwd()->get_contents()->writefile(state, words);
   //name plain file
   //get contents of words and put it in file
   //connect it to a node
   //close the file

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
   state.set_cwd(new_inode_ptr);
   state.add_filepath(words[1]);
   new_inode_ptr->increment_nr();
   cout << state << endl;
}

void fn_prompt (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << state;
   cout << "bruh";
   //cout << endl;
   cout << "bruh x 23";
   cout << words [1];
   int i = 1;
   state.set_prompt_("");
   while (words[i] != ""){
      state.set_prompt_(words[i]);
      state.set_prompt_(" ");
      i++;
   }

}

void fn_pwd (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << state.get_pathname() << endl;
}

void fn_rm (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rmr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}
