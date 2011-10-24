#!/bin/bash

(for((i=0;i<20;i++))
do
    echo test
    sleep 1
done) | nc -w 3 -u localhost 2525