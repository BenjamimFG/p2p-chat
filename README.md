[Português](README.pt.md) | English
<hr />  

# P2P Chat

A simple peer-to-peer chat for the Linux command line that I made to practice sockets programming after reading [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/).

This application lets 2 users chat with each other drectly in the command line.

# Dependencies
* NCursesW
* Pthreads

# Build
With the [dependencies](#dependencies) installed cd into the root folder of the project and run:
```bash
make
```

# Running the application

To run the application run this command in the same folder the binary `p2p_chat` is located:  
```bash
./p2p_chat PORT USERNAME
```
Where `PORT` is a number between 0 and 65535 the program should bind the socket to. This will be used by a peer when connecting to you.  
And `USERNAME` is the name that will be displayed to your peer in the chatroom.