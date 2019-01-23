#! /bin/csh
# Note BEEDIR must be set in you .cshrc 

if($#argv != 1 && $#argv != 2) then
  echo "usage: rec.csh word [volume]"
  exit
endif

if($#argv == 2)  then
  set vol = $2
else
  set vol = "80" 
endif

# prompt user 
echo "Type return then say the word then hit return: word = $1"
set retn = $< 

# start recording
#audiorecord -v $vol $BEEDIR/etc/Sounds/$1.au &
audiorecord -t 5 -v $vol $BEEDIR/etc/Sounds/$1.au &

# get PID 
set myPID  = `/usr/ucb/ps -aux | grep "audiorecord" | grep -v "grep" | awk '{print $2}'`

# wait for return then kill program 
set retn = $< 
sleep 1
kill -2 $myPID
echo "$1 recorded"

# play back word 
# sleep 1
# audioplay -p headphone ./etc/Sounds/$2.au
