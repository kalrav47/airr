#!/bin/bash

#declare -a input=(100 102 106 108 110 114 116 118 120 123 34 99 101 103 129)
#declare -a output=(35 49 131 133 135 137 139 195 196 50 132 107 109 111 48 47)                   
                      
input=""              
input="${input} 100"                 
input="${input} 102"                           
input="${input} 106"                           
input="${input} 108"                           
input="${input} 110"                           
input="${input} 114"                           
input="${input} 116"                           
input="${input} 118"                            
input="${input} 120"                            
input="${input} 123"                            
input="${input} 34"                             
input="${input} 99"                             
input="${input} 101"                            
input="${input} 103"                            
input="${input} 129"                            
                                                
output=""                                       
output="${output} 35"                           
output="${output} 49"                           
output="${output} 131"                          
output="${output} 133"                          
output="${output} 135"                          
output="${output} 137"                          
output="${output} 139"                          
output="${output} 195"                          
output="${output} 196"                          
output="${output} 50"                           
output="${output} 132"                          
output="${output} 107"                          
output="${output} 109"                          
output="${output} 111"                          
output="${output} 48"                           
output="${output} 47"

for i in ${input}
do
   echo "$i" > /sys/class/gpio/export
   echo in > /sys/class/gpio/gpio"$i"/direction 
done

for i in ${output}
do
   echo "$i" > /sys/class/gpio/export
   echo out > /sys/class/gpio/gpio"$i"/direction 
done
sync
sync
sync
