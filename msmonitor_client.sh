#!/bin/bash
ssh "$1" -f -N -L $2:localhost:$2
./utilities/msmonitor_cs/client/build/msmonitor_client
