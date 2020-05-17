// $Id: cix.cpp,v 1.9 2019-04-05 15:04:28-07 - - $
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"
#include "logstream.h"
#include "sockets.h"

logstream outlog (cout);
struct cix_exit: public exception {};

// taken from asg2
using wordvec = vector<string>;
wordvec split (const string& line, const string& delimiters) {
   wordvec words;
   size_t end = 0;

   // Loop over the string, splitting out words, and for each word
   // thus found, append it to the output wordvec.
   for (;;) {
      size_t start = line.find_first_not_of (delimiters, end);
      if (start == string::npos) break;
      end = line.find_first_of (delimiters, start);
      words.push_back (line.substr (start, end - start));
   }
   return words;
}

unordered_map<string,cix_command> command_map {
   {"exit", cix_command::EXIT},
   {"get" , cix_command::GET },
   {"help", cix_command::HELP},
   {"ls"  , cix_command::LS  },
   {"put" , cix_command::PUT },
   {"rm"  , cix_command::RM  },
};

static const char help[] = R"||(
exit         - Exit the program.  Equivalent to EOF.
get filename - Copy remote file to local host.
help         - Print help summary.
ls           - List names of files on remote server.
put filename - Copy local file to remote host.
rm filename  - Remove file from remote server.
)||";

void cix_help() {
   cout << help;
}
// ifstream::read()
// ofstream::write() <--- client will write
void cix_get(client_socket& server, string& filename){
   cix_header header;
   header.command = cix_command::GET;
   //char* slash = strchr(header.filename, '/');
   // server will check for slashes again for security
   // -Clark 
   if(strchr(filename.c_str(), '/') != nullptr){
      outlog << "/'s are prohibite" << endl;
      return;
   }

   strncpy(header.filename, filename.c_str(), FILENAME_SIZE-1);

   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   if (header.command != cix_command::FILEOUT) {
      outlog << "sent GET, server did not return FILEOUT" << endl;
      return;
   }

   auto buffer = make_unique<char[]> (header.nbytes);
   recv_packet (server, buffer.get(), header.nbytes);
   outlog << "received " << header.nbytes << " bytes" << endl;

   // bit wise or to run both inputs "flags"
   ofstream file(filename, ofstream::out|ofstream::binary);
   if(!file.is_open()){
      outlog << "Unable to open file" << endl;
      return;
   }
   file.write(buffer.get(), header.nbytes);
   file.close();
}
void cix_ls (client_socket& server) {
   cix_header header;
   header.command = cix_command::LS;

   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   outlog << "received header " << header << endl;
   if (header.command != cix_command::LSOUT) {
      outlog << "sent LS, server did not return LSOUT" << endl;

   }else {
      auto buffer = make_unique<char[]> (header.nbytes + 1);
      recv_packet (server, buffer.get(), header.nbytes);
      outlog << "received " << header.nbytes << " bytes" << endl;
      buffer[header.nbytes] = '\0';
      cout << buffer.get();
   }
}
// ifstream::read() <--- client will read
// ofstream::write() 
void cix_put(client_socket& server, string filename){
   cix_header header;
   header.command = cix_command::PUT;
   // check the name for slashes before opening
   int size;
   ifstream file(filename, ios::in|ios::binary|ios::ate);
   // dont need to check for slash in name explitcitly
   // file open will fail.
   // security wise the slash checks need to be client side.
   if (!file.is_open()) { 
      outlog << "put: failed to open read" << endl;
      return;
   }
   size = file.tellg();

   header.command = cix_command::PUT;
   header.nbytes = size;
   strncpy(header.filename, filename.c_str(), FILENAME_SIZE-1);


   send_packet(server, &header, sizeof header);

   auto buffer = make_unique<char[]> (size);
   file.seekg (0, ios::beg);
   file.read (buffer.get(), size);
   file.close();
   send_packet (server, buffer.get(), size);
   outlog << "sent " << size << " bytes" << endl;
   //check header reply after sending file packet
   recv_packet (server, &header, sizeof header);
   if(header.command != cix_command::ACK){
      outlog << "sent PUT, server did not return ACK" << endl;
      return;
   }
   outlog << "put: ACK" << endl;
}
void cix_rm(client_socket& server, string filename){
   cix_header header;
   header.command = cix_command::RM;
   strncpy(header.filename, filename.c_str(), FILENAME_SIZE-1);

   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   if (header.command != cix_command::ACK) {
      outlog << "sent RM, server did not return ACK" << endl;
   }else {
      outlog << filename << " has been rm" << endl;
   }
}


void usage() {
   cerr << "Usage: " << outlog.execname() << " [host] [port]" << endl;
   throw cix_exit();
}

int main (int argc, char** argv) {
   outlog.execname (basename (argv[0]));
   outlog << "starting" << endl;
   vector<string> args (&argv[1], &argv[argc]);
   if (args.size() > 2) usage();
   string host = get_cix_server_host (args, 0);
   in_port_t port = get_cix_server_port (args, 1);
   outlog << to_string (hostinfo()) << endl;
   try {
      outlog << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      outlog << "connected to " << to_string (server) << endl;
      for (;;) {
         string line;
         string filename;
         getline (cin, line);
         if (cin.eof()) throw cix_exit();
         wordvec words = split(line, " \t");
         if(words.empty()){
            continue;
         }
         outlog << "command " << line << endl;
         const auto& itor = command_map.find (words[0]);
         cix_command cmd = itor == command_map.end()
                         ? cix_command::ERROR : itor->second;
         // checking for a filename.
         // also pass in filename into get, put, and rm
         if(cmd == cix_command::GET || cmd == cix_command::PUT || 
            cmd == cix_command::RM  ){
            if(words.size() < 2 )
               continue;
            else
               filename = words[1];
         }
         switch (cmd) {
            case cix_command::EXIT:
               throw cix_exit();
               break;
            case cix_command::HELP:
               cix_help();
               break;
            case cix_command::GET:
               cix_get (server, filename);
               break;               
            case cix_command::LS:
               cix_ls (server);
               break;
            case cix_command::PUT:
               cix_put (server, filename);
               break;
            case cix_command::RM:
               cix_rm (server, filename);
               break;                              
            default:
               outlog << line << ": invalid command" << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      outlog << error.what() << endl;
   }catch (cix_exit& error) {
      outlog << "caught cix_exit" << endl;
   }
   outlog << "finishing" << endl;
   return 0;
}

