#!/bin/bash

echo "*** robotcowboy unit shutdown procedure ***"

UNITD_PID=$(pidof unitd)
echo "grab unitd pid: $UNITD_PID"

echo "now shutdown unitd"
kill -s TERM $UNITD_PID

echo "*** shutdown procedure concluded ***"

exit 0
