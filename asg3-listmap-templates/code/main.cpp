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

void mapfile (istream& infile, const string& filename) {
   static string colons (32, ':');
   cout << colons << endl << filename << endl << colons << endl;
   regex comment_regex {R"(^\s*(#.*)?$)"}; //#
   regex trimmed_regex {R"(^\s*([^=]+?)\s*$)"}; // = 
   regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"}; 
   for(;;) {
      string line;
      getline (infile, line);
      if (infile.eof()) break;
      cout << line << endl;
      cout << "input: \"" << line << "\"" << endl;
      smatch result;
      if (regex_search (line, result, comment_regex)) {
         cout << "Comment or empty line." << endl; //if hashtag comment
      }else if (regex_search (line, result, trimmed_regex)) {
         if (result[1]=="="){
            cout << "query: \"" << result[1] << "\"" << endl; //if just an equals side note this is useless            
         }
         else{
            cout << "key: \"" << result[1] << "\"" << "just the val baby" << endl; //if just an equals 
         }
      }else if (regex_search (line, result, key_value_regex)) { 
         cout << "key  : \"" << result[1] << "\"" << endl;
         cout << "value: \"" << result[2] << "\"" << endl; //if key and value
         if ((result[1]=="")&&(result[2]=="")){
            cout << "both are empty"; //and run some code
         }
         else if (result[2]==""){
            cout << "value empty"; //and run some code
         }
         else if (result[1]==""){
            cout << "key empty"; //and run some code
         } else{
            cout << "both are full"; //and run some code
         }
      }
      else {
         assert (false and "This can not happen.");
      }
      //you idget, this is where you put the code cout << line << endl;
   }
}

int main (int argc, char** argv) {
   sys_info::execname (argv[0]);
   scan_options (argc, argv);
   int status = 0;
   string progname (basename (argv[0]));
   vector<string> filenames (&argv[1], &argv[argc]);
   if (filenames.size() == 0) filenames.push_back (cin_name);
   for (const auto& filename: filenames) {
      if (filename == cin_name) mapfile (cin, filename);
      else {
         ifstream infile (filename);
         if (infile.fail()) {
            status = 1;
            cerr << progname << ": " << filename << ": "
                 << strerror (errno) << endl;
         }else {
            mapfile (infile, filename);
            infile.close();
         }
      }
   }
   return status;

   // str_str_map test;
   // cout << test << endl;
   // for (char** argp = &argv[optind]; argp != &argv[argc]; ++argp) {
   //    str_str_pair pair (*argp, to_string<int> (argp - argv));
   //    cout << "Before insert: " << pair << endl;
   //    test.insert (pair);
   // }

   // cout << test.empty() << endl;
   // for (str_str_map::iterator itor = test.begin();
   //      itor != test.end(); ++itor) {
   //    cout << "During iteration: " << *itor << endl;
   // }

   // str_str_map::iterator itor = test.begin();
   // test.erase (itor);

   // cout << "EXIT_SUCCESS" << endl;
   // return EXIT_SUCCESS;
}

