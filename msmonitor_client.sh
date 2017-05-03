#!/bin/bash
ssh "$1" -f -N -L 2900:localhost:2900
./utilities/msmonitor_cs/client/build/msmonitor_client
