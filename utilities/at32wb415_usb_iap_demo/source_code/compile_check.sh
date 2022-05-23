#!/bin/sh

# CHANGED WITH USER PATH, install path of keil
keil_exe="c:/keil_v5/uv4/uv4.exe"

# progress bar function
function progress_bar()
{
  printf "###progress###[%01d/%01d]###[%04d/%04d]\r" $1 $2 $3 $4
}

step=1
all=1
process=1

# if compile_result.txt is exsited, remove it
if [ -f ./compile_result.txt ]; then
  rm ./compile_result.txt
fi

echo "--- compile check ---"

total=$(find -name "*.uvprojx" | wc -l)
# compile and check
for file in `find -name "*.uvprojx"`
do
  progress_bar $step $all $process $total
  let process++

  # generate *.uvoptv file
  $keil_exe $file -i
  # compile
  $keil_exe -b -j0 $file -l log.txt

  # get result
  while [ 1 ]
  do
    # check compiled, log.txt has exsited
    if [ ! -f "./log.txt" ]; then
      # wait
      sleep 10
    else
      # get result
      echo $file >> ./compile_result.txt
      echo `grep -a "Error(s)" log.txt | cut -d "-" -f 2` >> ./compile_result.txt
      rm log.txt
      sync
      break
    fi
  done
done
printf "\n"

exit 1
