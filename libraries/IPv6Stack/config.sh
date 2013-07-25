CONFILE="contiki_conf"
EXTENSION=".h"

if [ $# != 2 ]
then
	echo "Not enough parameters (2). Must specify test number and host/router mode. Example for test number 2, Host mode: $0 2 h"
else
	if [ "$2" != "h" ] && [ "$2" != "r" ]
	then
		echo "Error: second argument should be 'h' or 'r' for HOST or ROUTER mode respectively"
		exit 1
	fi
	if [ $1 -lt 13 ] && [ $1 -gt 0 ]
	then
			if [ $1 -eq 1 ] || [ $1 -eq 6 ]
			then
				TEST_NUMBER=1
			elif [ $1 -eq 2 ] || [ $1 -eq 5 ]
			then
				TEST_NUMBER=2
			elif [ $1 -eq 9 ] || [ $1 -eq 11 ]
			then
				TEST_NUMBER=3
			elif [ $1 -eq 10 ] || [ $1 -eq 12 ]
			then
				TEST_NUMBER=4
			else
				echo "Error: test $1 is not implemented"
				exit 1
			fi
			if [ "$2" != "h" ]
			then
				echo "Error: test $1 only available for HOST mode"
				exit 1
			fi
	elif [ $1 -eq 13 ] || [ $1 -eq 26 ]
	then
		TEST_NUMBER=3
	elif [ $1 -eq 14 ] || [ $1 -eq 27 ]
	then
		TEST_NUMBER=4
	elif [ $1 -eq 15 ] || [ $1 -eq 28 ]
	then
		TEST_NUMBER=5
	elif [ $1 -eq 16 ] || [ $1 -eq 29 ]
	then
		TEST_NUMBER=6
	else
		echo "Error: test $1 is not implemented"
		exit 1
	fi
		
	FILENAME=$CONFILE"_"$TEST_NUMBER"_"$2$EXTENSION
	if [ -e $FILENAME ]
	then
		cp -f $FILENAME $CONFILE$EXTENSION
	else
		echo "Error: The application does not exist"
	fi
fi
