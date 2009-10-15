#!/bin/sh -
#
#    initialization/shutdown script for foobar package

case "$1" in
start)
	/usr/local/sbin/foo -d && echo -n ’ foo’
	;;
stop)
	kill ‘cat /var/run/foo.pid‘ && echo -n ’ foo’
	;;
*)
	echo "unknown option: $1 - should be ’start’ or ’stop’" >&2 
	;;
esac
