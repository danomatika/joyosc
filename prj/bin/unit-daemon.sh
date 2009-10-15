#!/bin/sh

### BEGIN INIT INFO
# Provides:          rc_test
# Required-Start:    
# Required-Stop:     
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: start unit-daemon daemon
### END INIT INFO


. /lib/lsb/init-functions

case "$1" in
	start)
		log_begin_msg "Starting unit-daemon..."

		if start-stop-daemon --start --exec /home/dano/thesis/src/unit-daemon/bin/Debug/unit-daemon -- -D; then
		log_end_msg 0
		else
		log_end_msg $?
		fi
		;;
	stop)
		log_begin_msg "Stopping unit-daemon..."

		if start-stop-daemon --stop --exec /home/dano/thesis/src/unit-daemon/bin/Debug/unit-daemon; then
		log_end_msg 0
		else
		log_end_msg $?
		fi
		;;
	restart)
		$0 stop
		sleep 1
		$0 start
		;;
	*)
		log_success_msg "Usage: /etc/init.d/unit-daemon {start|stop|restart}"
		exit 1
		;;
esac

exit 0
