// $Id: commands.cpp,v 1.21 2021-09-26 12:41:17-07 - - $

#include "commands.h"
#include "debug.h"

command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
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

void fn_cd (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   // Change state according to words[1] (which is a directory or . or ..)
   // words[1] to directory to retrieve inode_ptr
   try {
      inode_ptr new_inode_ptr = 
         state.get_cwd()->get_contents()->get_directory_inode(words[1]);
      if (new_inode_ptr == nullptr) {
         throw runtime_error(words[1]);
      }
      cout << "state.get_cwd():" << state.get_cwd() << endl;
      state.get_cwd()->set_inode_nr(new_inode_ptr->get_inode_nr());
      state.get_cwd()->set_next_inode_nr(new_inode_ptr->get_next_inode());
      state.set_cwd(new_inode_ptr);
      state.get_cwd()->set_contents(new_inode_ptr->get_contents());

   } catch (runtime_error& error) {
      cout << "fn_cd error: " << words[1] << " does not exist" << endl;
   }
   // then the state can reset to inode_ptr
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

   inode_ptr new_inode_ptr = state.get_cwd()->get_contents()->mkdir(words[1]);
   
   // Insert new_inode_ptr to current directory's map
   state.set_cwd(new_inode_ptr);
   new_inode_ptr->increment_nr();
   cout << state << endl;
}

void fn_prompt (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_pwd (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   cout << state.get_cwd() << endl;
}

void fn_rm (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rmr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}