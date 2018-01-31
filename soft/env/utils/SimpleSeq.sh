#!/bin/sh

# count - step through numeric values until you get to the max value

if [ "$#" -lt 2 ] ; then
  echo "Usage: count {increment}"
  exit 1
fi

counter="$1"
max="$2"

if [ "$#" -eq 2 ] ; then
  step=1
else
  step=$3
fi

while [ $counter -le $max ] ; do
  echo $counter
  counter=$( expr $counter + $step )
done

exit 0
