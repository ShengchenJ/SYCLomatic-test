#!/bin/bash


array=($(grep LLVMFuzzerTestOneInput ./*.cpp -rl))
if [ -d workspace ];
then
rm -rf workspace
fi
mkdir workspace 
for item in "${array[@]}";
do 
cp $item workspace
testCase=$(basename $item .cpp)
outputName=${testCase}.run
logFile=${testCase}.log
echo $outputName
dpcpp -fsanitize=fuzzer $item -o $outputName &> $logFile 
./$outputName -runs=1000 &>> return_log
if [ $? -ne 0 ]; then echo "$outputName Not zero" >> result ;fi
done
