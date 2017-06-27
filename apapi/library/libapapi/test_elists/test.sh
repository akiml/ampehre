#!/bin/bash

export LD_LIBRARY_PATH=.

APAPI_EVENTLIST=list ./testlist
echo $? list

