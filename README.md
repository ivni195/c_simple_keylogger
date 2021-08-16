# c_simple_keylogger
Simple keylogger written in C. It scans all 
`/dev/input/event*` files and tries to figure out 
which ones represent keyboards. It then proceeds to
monitor those files. It can log the pressed keys 
to a file, or it can send them to a TCP server.
# Building
In order to build, use `make`:
```bash
cd src && make
```
In order to remove the executable:
```bash
make clean
```
#Usage
In order to use the keylogger you will need root 
priviliges as you need to be able to read `/dev/input/event`
files.
```
Usage: ./keylogger [-fn] [-t target] [-p port]
   -f        - log to a file
   -n        - log to a remote server
   -t target - remote server IP address (if using -n switch) or log file path (if using -f switch).
   -p port   - remote server listening port (only has effect when used with -n).
```