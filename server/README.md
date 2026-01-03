## Server Process
This application is designed have a single server manage a maximum of 5 clients. You can configure this limit by changing the `MAX_CLIENTS` macro found in `./server.c`. The server uses port 8080.

### Setup
If you are using a firewall like `ufw`, you must explicitly allow traffic on the server's port (default: 8080):
```
sudo ufw allow 8080/tcp
```
If you want users outside your local Wi-Fi to join, you must configure your router:
 - **Static IP:** Assign a reserved local IP to your server machine in your router settings.
 - **Port Forwarding:** Create a rule in your router (e.g., Cox Panoramic App) to forward Port 8080 (TCP) to your server's local IP.
 - **Advanced Security:** Disable ISP-level "Advanced Security" features if they block unauthorized incoming TCP requests.

cd into the `server/` directory (where you're currently at) and compile the server program
```
make server
```
After the server program compiles, run the server executable 
```
./server
```
On success, you'll see `STATUS: Server initiated` printed onto the terminal. Statements starting with `STATUS: ` are notifications from the server which can be errors or important updates.
Messages from clients are shown with `Chat: `.

### WARNING: Remove the port forward rule you have set after use!
Every open port is a potential doorway for hackers or automated bots.
