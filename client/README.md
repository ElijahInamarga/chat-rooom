## Client Process
Users can connect to the server and chat with other users using this client process.

### Setup
cd into the `client/` directory (where you're currently at) and compile the client program
```
make client
```
After the client program compiles, run the client executable 
```
./client
```
On startup, you will be prompted with `STATUS: Enter server public IP: ` where you'll need to enter the server's address.

If you are running the client and the server on the exact same computer, enter the Loopback Address:
```
127.0.0.1
```
If the client and the server is running on different machines but on the same home network, enter the server machine's local IP address.
You can find the server macines local IP address with:
```
ip addr show
```
(look for the `inet 192.168.0.xxx` line under `wlan0: `)

If the server is hosted in a completely different network, enter the public server IP in which the server is being hosted. The server's 
public IP can be accessed with:
```
curl -4 ifconfig.me
```

On success, you'll see `STATUS: Connected to server` printed onto the terminal. 
Statements starting with `STATUS: ` are error notifications from the client process. 
Messages from other clients are shown with `Chat: `. Your own messages are shown with `You: `.
