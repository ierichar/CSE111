// Ian Richardson, ierichar
// Jackson Brazeal, jbrazeal
// $Id: main.cpp,v 1.13 2021-02-01 18:58:18-08 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <regex>
#include <cassert>
#include <fstream>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

const string cin_name = "-";

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << char (optopt) << ": invalid option"
                       << endl;
            break;
      }
   }
}

void mapfile (istream& infile, const string& filename, 
   str_str_map& map) {

   static string colons (32, ':');
   cout << colons << endl << filename << endl << colons << endl;
   regex comment_regex {R"(^\s*(#.*)?$)"}; //#
   regex trimmed_regex {R"(^\s*([^=]+?)\s*$)"}; // = 
   regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};

   str_str_map::iterator itor;     // Define main iterator
   string line;

   for(;;) {
      getline (infile, line);
      if (infile.eof()) break;
      cout << line << endl;
      cout << "input: \"" << line << "\"" << endl;
      smatch result;

      if (regex_search (line, result, comment_regex)) {
         // If hashtag comment
      } else if (regex_search (line, result, trimmed_regex)) {
         // If just a key
         itor = map.find(result[1]);
         if (itor == map.end()) 
            cout << result[1] << ": not found" << endl;
         else cout << itor->first << " = " << itor->second << endl;
      } else if (regex_search (line, result, key_value_regex)) { 
         // If = is present
         if ((result[1]=="") && (result[2]=="")) {
            // Only =
            itor = map.begin();
            for (; itor != map.end(); ++itor)
               cout << itor->first << " = " << itor->second << endl;
         } 
         else if (result[2]=="") {
            // If only key=
            itor = map.find(result[1]);
            if (itor == map.end()) 
               cout << result[1] << ": not found" << endl;
            else map.erase(itor);
         }
         else if (result[1]=="") {
            // If only =value
            itor = map.begin();
            for (; itor != map.end(); ++itor) {
               if (itor->second == result[2])
                  cout << itor->first << " = " << itor->second << endl;
            }
         } 
         else {
            // If key=value
            str_str_pair val_pair { result[1], result[2] };
            map.insert(val_pair);
         }
      }
      else {
         assert (false and "This can not happen.");
      }
   }
}

int main (int argc, char** argv) {
   sys_info::execname (argv[0]);
   scan_options (argc, argv);
   int status = 0;
   string progname (basename (argv[0]));
   vector<string> filenames (&argv[1], &argv[argc]);
   str_str_map lmap;

   if (filenames.size() == 0) filenames.push_back (cin_name);
   for (const auto& filename: filenames) {
      if (filename == cin_name) mapfile (cin, filename, lmap);
      else {
         ifstream infile (filename);
         if (infile.fail()) {
            status = 1;
            cerr << progname << ": " << filename << ": "
                 << strerror (errno) << endl;
         }else {
            mapfile (infile, filename, lmap);
            infile.close();
         }
      }
   }
   
   return status;
}

