CONFILE="contiki_conf"
EXTENSION=".h"

if [ $# != 2 ]
then
	echo "Not enough parameters (2). Must specify application number and host/router. Example for application 2, Host mode: $0 2 h"
else
	FILENAME=$CONFILE"_"$1"_"$2$EXTENSION
	if [ -e $FILENAME ]
	then
		cp -f $FILENAME $CONFILE$EXTENSION
	else
		echo "Error: $FILENAME does not exist"
	fi
fi



