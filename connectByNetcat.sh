#!/bin/bash

# Connect to the server using nc
nc 127.0.0.1 6667

# Wait for a short time to ensure the connection is established
sleep 1

# Send the required commands
echo "PASS 6667"
sleep 1
echo "NICK $1"
sleep 1
echo "USER $1 0 * :realname"
sleep 1

