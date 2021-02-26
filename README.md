# client-server-socket
A C++ client-server application with sockets

This project will implement a client/server application using sockets. A daemon (cxid) listens on a socket for client connection requests. Each connection will cause the server to fork a child process to serve the client. The daemon runs in an infinite loop listening. The server exits when the client disconnects. A client (cxi) connects to a server and can send files, receive files, and get a summary listing of all files present.

***Client Interactive Commands***

The cxi client responds to commands read from the standard output and writes out- put to the standard output and error and accesses files. In the syntax below, Courier Bold are literal characters actually typed in, while Roman Italic stands for appropriate substitutions.
  
CSE-111 • Winter 2021 • Program 4 • Client/Server and Sockets 2 of 5 exit
Quit the program. An end of file marker or Control/D is equivalent.
get filename
Copy the file named filename on the remote server and create or overwrite a file of the same name in the current directory.
help
ls
Causes the remote server to execute the command ls -l and prints the output to the user’s terminal.
put filename
Copies a local file into the socket and causes the remote server to create that file in its directory.
rm filename
Causes the remote server to remove the file.
