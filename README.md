# Peer-Peer-game-server

READ ME: Peer-to-Peer On-line game server
Submitted By: Vaibhav, Ratnesh and Gaurav

On Server side:

Compile the server code by executing command:
g++ -lpthread server.cpp -o serv

Start the server by executing the executable:
./serv

This will start the game server.
By default the port number is set to be 2200.


On Client side:

Compile the Client code by executing command:
g++ -lpthread client.cpp -o cl1

Do same for as many clients as you want.
Make sure you make different object files for all the clients.

To run the Client, execute
<client object file> 0.0.0.0 2200 <an int value to generate port on which client will listen>

Example of command: cl1 0.0.0.0 2200 1

This will start the client to proceed further.

Now, register yourself to login.

Login with required credentials.

List of existing users:
Username: a	Password: a
Username: b	Password: b
Username: c	Password: c
Username: d	Password: d

Type help to list commands you can use.
