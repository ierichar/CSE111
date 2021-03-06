// $Id: cxi.cpp,v 1.6 2021-11-08 00:01:44-08 - - $

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "debug.h"
#include "logstream.h"
#include "protocol.h"
#include "socket.h"

logstream outlog (cout);
struct cxi_exit: public exception {};

unordered_map<string,cxi_command> command_map {
   {"exit", cxi_command::EXIT},
   {"help", cxi_command::HELP},
   {"ls"  , cxi_command::LS  },
   {"get" , cxi_command::GET }, 
   {"put" , cxi_command::PUT },
};

static const char help[] = R"||(
exit         - Exit the program.  Equivalent to EOF.
get filename - Copy remote file to local host.
help         - Print help summary.
ls           - List names of files on remote server.
put filename - Copy local file to remote host.
rm filename  - Remove file from remote server.
)||";

void cxi_help() {
   cout << help;
}

void cxi_ls (client_socket& server) {
   cxi_header header;
   header.command = cxi_command::LS;
   DEBUGF ('h', "sending header " << header << endl);
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   DEBUGF ('h', "received header " << header << endl);
   if (header.command != cxi_command::LSOUT) {
      outlog << "sent LS, server did not return LSOUT" << endl;
      outlog << "server returned " << header << endl;
   }else {
      size_t host_nbytes = ntohl (header.nbytes);
      auto buffer = make_unique<char[]> (host_nbytes + 1);
      recv_packet (server, buffer.get(), host_nbytes);
      DEBUGF ('h', "received " << host_nbytes << " bytes");
      buffer[host_nbytes] = '\0';
      cout << buffer.get();
   }
}

void cxi_get (client_socket& server, const string& filename) {
   cxi_header header;
   header.command = cxi_command::GET;
   // assign the filename to header.filename
   for (size_t i = 0; i < filename.length(); i++) {
      header.filename[i] = filename[i];
   }
   DEBUGF ('h', "sending header " << header << endl);
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   DEBUGF ('h', "received header " << header << endl);
   if (header.command != cxi_command::FILEOUT) {
      outlog << "sent GET, server did not return FILEOUT" << endl;
      outlog << "server returned " << header << endl;
   }else {
      size_t host_nbytes = ntohl (header.nbytes);
      auto buffer = make_unique<char[]> (host_nbytes + 1);
      recv_packet (server, buffer.get(), host_nbytes);
      DEBUGF ('h', "received " << host_nbytes << " bytes");
      buffer[host_nbytes] = '\0';
      cout << buffer.get();
   }
}

void cxi_put (client_socket& server, const string& filename) {
   cxi_header header;
   header.command = cxi_command::PUT;
   // assign the filename to header.filename
   for (size_t i = 0; i < filename.length(); i++) {
      header.filename[i] = filename[i];
   }
   // read from file
   ifstream file (header.filename, ios::binary);
   if (!file.is_open()) {
      outlog << "put " << header.filename;
      outlog << ": " << strerror (errno) << endl;
      return;
   }
   string put_output;
   char buffer[0x5000];
   file.read (buffer, sizeof buffer);
   put_output.append (buffer);
   file.close();
   header.nbytes = htonl (put_output.size());

   memset (header.filename, 0, FILENAME_SIZE);
   DEBUGF ('h', "sending header " << header << endl);
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   DEBUGF ('h', "received header " << header << endl);
   if (header.command != cxi_command::ACK) {
      outlog << "sent GET, server did not return FILEOUT" << endl;
      outlog << "server returned " << header << endl;
   }else {
      size_t host_nbytes = ntohl (header.nbytes);
      auto buffer = make_unique<char[]> (host_nbytes + 1);
      recv_packet (server, buffer.get(), host_nbytes);
      DEBUGF ('h', "received " << host_nbytes << " bytes");
      cout << "request successfully completed" << endl;
   }
}


void usage() {
   cerr << "Usage: " << outlog.execname() << " host port" << endl;
   throw cxi_exit();
}

pair<string,in_port_t> scan_options (int argc, char** argv) {
   for (;;) {
      int opt = getopt (argc, argv, "@:");
      if (opt == EOF) break;
      switch (opt) {
         case '@': debugflags::setflags (optarg);
                   break;
      }
   }
   if (argc - optind != 2) usage();
   string host = argv[optind];
   in_port_t port = get_cxi_server_port (argv[optind + 1]);
   return {host, port};
}

int main (int argc, char** argv) {
   outlog.execname (basename (argv[0]));
   outlog << to_string (hostinfo()) << endl;
   try {
      auto host_port = scan_options (argc, argv);
      string host = host_port.first;
      in_port_t port = host_port.second;
      outlog << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      outlog << "connected to " << to_string (server) << endl;
      for (;;) {
         string line;
         getline (cin, line);
         if (cin.eof()) throw cxi_exit();

         // Seperate line into command and filename (if needed)
         // Reminiscient of "split" in asg2
         vector<string> wordvec;
         size_t end {0};
         for (;;) {
            size_t start {line.find_first_not_of (' ', end)};
            if (start == string::npos) break;
            end = line.find_first_of (' ', start);
            wordvec.push_back (line.substr (start, end - start));
         }
         // Now using wordvec[0] instead of line to find command
         outlog << "command " << wordvec[0] << endl;
         const auto& itor = command_map.find (wordvec[0]);

         cxi_command cmd = itor == command_map.end()
                         ? cxi_command::ERROR : itor->second;
         switch (cmd) {
            case cxi_command::EXIT:
               throw cxi_exit();
               break;
            case cxi_command::HELP:
               cxi_help();
               break;
            case cxi_command::LS:
               cxi_ls (server);
               break;
            case cxi_command::GET:
               // Pass the filename as well (if given)
               if (wordvec.size() > 1) cxi_get (server, wordvec[1]);
               else outlog << line << ": no filename given" << endl;
               break;
            case cxi_command::PUT:
               if (wordvec.size() > 1) cxi_put (server, wordvec[1]);
               else outlog << line << ": no filename given" << endl;
               break;
            default:
               outlog << line << ": invalid command" << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      outlog << error.what() << endl;
   }catch (cxi_exit& error) {
      DEBUGF ('x', "caught cxi_exit");
   }
   return 0;
}