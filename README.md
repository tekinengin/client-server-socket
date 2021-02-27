# client-server-socket
A C++ client-server application with sockets

This project will implement a client/server application using sockets. A daemon (cxid) listens on a socket for client connection requests. Each connection will cause the server to fork a child process to serve the client. The daemon runs in an infinite loop listening. The server exits when the client disconnects. A client (cxi) connects to a server and can send files, receive files, and get a summary listing of all files present.

**Usage**
- Build: ``make``
- Running Server: ``./cxid port``
- Running Client: ``./cxi host port``

**Client Interactive Commands**
<pre>

exit         - Exit the program.  Equivalent to EOF.
get filename - Copy remote file to local host.
help         - Print help summary.
ls           - List names of files on remote server.
put filename - Copy local file to remote host.
rm filename  - Remove file from remote server.
</pre>
