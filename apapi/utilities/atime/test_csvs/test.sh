#!/bin/bash

export LD_LIBRARY_PATH=.

APAPI_EVENTOPS=tokenmismatch1.csv ./testcsv
echo $? tokenmismatch1.csv

echo

APAPI_EVENTOPS=tokenmismatch2.csv ./testcsv
echo $? tokenmismatch2.csv

echo

APAPI_EVENTOPS=type1.csv ./testcsv
echo $? type1.csv

echo

APAPI_EVENTOPS=type2.csv ./testcsv
echo $? type2.csv

echo

APAPI_EVENTOPS=type3.csv ./testcsv
echo $? type3.csv

echo

APAPI_EVENTOPS=type4.csv ./testcsv
echo $? type4.csv

echo

APAPI_EVENTOPS=type5.csv ./testcsv
echo $? type5.csv

echo

APAPI_EVENTOPS=type6.csv ./testcsv
echo $? type6.csv

echo

APAPI_EVENTOPS=type7.csv ./testcsv
echo $? type7.csv

echo

APAPI_EVENTOPS=type8.csv ./testcsv
echo $? type8.csv

echo

APAPI_EVENTOPS=type9.csv ./testcsv
echo $? type9.csv

echo

APAPI_EVENTOPS=type10.csv ./testcsv
echo $? type10.csv

echo

APAPI_EVENTOPS=type11.csv ./testcsv
echo $? type11.csv

echo

APAPI_EVENTOPS=type12.csv ./testcsv
echo $? type12.csv

echo

APAPI_EVENTOPS=mul1.csv ./testcsv
echo $? mul1.csv

echo
