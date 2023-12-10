#!/bin/sh

ROOT=$(pwd)
BIN=$(pwd)/webserv
TEST_FOLDER=$(pwd)/testes/integration/
DEBUG_LOG="$TEST_FOLDER"debug.log
SHOW_TEST_NAME=1

fail() {
	EXITCODE=$1
	cat $DEBUG_LOG
	echo "FAIL: $TESTNAME ❌"
	rm $DEBUG_LOG
	exit $EXITCODE
}

success() {
	echo "SUCCESS! ✅"
	rm $DEBUG_LOG
}

run() {
	TEST=$1
	TESTNAME="$(basename $(dirname $TEST))/$(basename $TEST)"
	if [ $SHOW_TEST_NAME != 0 ]; then
		echo "TEST: $TESTNAME"
	fi
	if [ ! -d $TEST ]; then
		echo "ERROR: folder $TEST" >> $DEBUG_LOG
		fail 1
	fi
	PY=$TEST/test.py
	CONF=$TEST/config.conf
	$BIN $CONF 1>$DEBUG_LOG 2>$DEBUG_LOG &
	PID=$!
	export PYTHONPATH=$PYTHONPATH:$TEST_FOLDER
	python3 $PY
	EXITCODE=$?
	# kill process
	ps -p $PID 1>/dev/null 2>/dev/null
	RUNNING=$?
	if [ $RUNNING != 0 ]; then
		echo "ERROR: webserv no run" >> $DEBUG_LOG
		fail 1
	fi
	kill $PID
	#
	if [ $EXITCODE != 0 ]; then
		fail $EXITCODE
	fi
	return $EXITCODE
}

folders() {
	for f in "$1"*; do
		if [ -d "$f" ]; then
			if [ -z $(echo $f | grep __pycache__) ]; then
				echo $f
			fi
		fi
	done
}

subfolders() {
	for f in $(folders $1); do
		folders "$f"/
	done
}

run_all_tests() {
	for f in $(subfolders $TEST_FOLDER); do
		run $f
	done
	success
}

run_single_test() {
	run $TEST_FOLDER$1
	success
}

if [ -n "$1" ]; then
	run_single_test $1
else
	run_all_tests
fi
