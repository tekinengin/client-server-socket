// $Id: cxi.cpp,v 1.1 2020-11-22 16:51:43-08 - - $

#include <iostream>
#include <fstream>
#include <sstream>
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
struct cxi_exit: public exception {};

unordered_map<string,cxi_command> command_map {
   {"exit", cxi_command::EXIT},
   {"help", cxi_command::HELP},
   {"ls"  , cxi_command::LS  },
   {"rm"  , cxi_command::RM  },
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
   outlog << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   outlog << "received header " << header << endl;
   if (header.command != cxi_command::LSOUT) {
      outlog << "sent LS, server did not return LSOUT" << endl;
      outlog << "server returned " << header << endl;
   }else {
      size_t host_nbytes = ntohl (header.nbytes);
      auto buffer = make_unique<char[]> (host_nbytes + 1);
      recv_packet (server, buffer.get(), host_nbytes);
      outlog << "received " << host_nbytes << " bytes" << endl;
      buffer[host_nbytes] = '\0';
      cout << buffer.get();
   }
}

void cxi_rm (client_socket& server, string& filename) {
   cxi_header header;
   header.command = cxi_command::RM;
   strcpy(header.filename, filename.c_str());
   outlog << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   outlog << "received header " << header << endl;
   if (header.command != cxi_command::ACK) {
      outlog << "sent RM, server did not return ACK" << endl;
      outlog << "server returned " << header << endl;
   }else {
      outlog << "foo was deleted by server" << endl;
   }
}

void cxi_get (client_socket& server, string& filename) {
   cxi_header header;
   header.command = cxi_command::GET;
   strcpy(header.filename, filename.c_str());
   outlog << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   outlog << "received header " << header << endl;
   if (header.command != cxi_command::FILEOUT) {
      outlog << "sent GET, server did not return FILEOUT" << endl;
      outlog << "server returned " << header << endl;
   }else {
      size_t host_nbytes = ntohl (header.nbytes);
      auto buffer = make_unique<char[]> (host_nbytes + 1);
      recv_packet (server, buffer.get(), host_nbytes);
      outlog << "received " << host_nbytes << " bytes" << endl;
      //buffer[host_nbytes] = '\0';
      ofstream file(filename);
      file << buffer.get();
   }
}

void cxi_put (client_socket& server, string& filename) {
   cxi_header header;
   ifstream file (filename, ifstream::binary);
   if (file) {
        // get length of file:
        file.seekg (0, file.end);
        int length = file.tellg();
        file.seekg (0, file.beg);

        outlog << filename << " is being read" << endl;
        char * buffer = new char [length];
        file.read (buffer,length);
        if(file){
           outlog << filename << " is read" << endl;
           file.close();
           header.command = cxi_command::PUT;
           strcpy(header.filename, filename.c_str());
           header.nbytes = htonl (length);
           outlog << "sending header " << header << endl;
           send_packet (server, &header, sizeof header);
           send_packet (server, buffer, length);
           outlog << "sent " << length << " bytes" << endl;
           recv_packet (server, &header, sizeof header);
           outlog << "received header " << header << endl;

           if(header.command != cxi_command::ACK){
              outlog << "sent PUT, server did not return ACK" << endl;
              outlog << "server returned " << header << endl;
           }else{
              outlog << "file was successfuly sent" << endl;
           }
           return;
        }
   }
   outlog << "put " << header.filename << ": " << strerror (errno) << endl;
   return;
}





void usage() {
   cerr << "Usage: " << outlog.execname() << " [host] [port]" << endl;
   throw cxi_exit();
}

int main (int argc, char** argv) {
   outlog.execname (basename (argv[0]));
   outlog << "starting" << endl;
   vector<string> args (&argv[1], &argv[argc]);
   if (args.size() > 2) usage();
   string host = get_cxi_server_host (args, 0);
   in_port_t port = get_cxi_server_port (args, 1);
   outlog << to_string (hostinfo()) << endl;
   try {
      outlog << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      outlog << "connected to " << to_string (server) << endl;
      for (;;) {
         string line;
         getline (cin, line);
         if (cin.eof()) throw cxi_exit();

         vector<std::string> result;
         istringstream iss(line);
         cout << "command :";
         for(std::string s; iss >> s;) {
            result.push_back(s);
            cout << " " << s;
         }
         cout << endl;

         const auto& itor = command_map.find (result[0]);
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
            case cxi_command::RM:
               cxi_rm (server, result[1]);
               break;
            case cxi_command::GET:
               cxi_get (server, result[1]);
               break;
            case cxi_command::PUT:
               cxi_put (server, result[1]);
               break;
            default:
               outlog << line << ": invalid command" << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      outlog << error.what() << endl;
   }catch (cxi_exit& error) {
      outlog << "caught cxi_exit" << endl;
   }
   outlog << "finishing" << endl;
   return 0;
}

