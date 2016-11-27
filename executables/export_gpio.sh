declare -a input=(100 102 106 108 110 114 116 118 120 123 34 99 101 103 129)
declare -a output=(35 49 131 133 135 137 139 195 196 50 132 107 109 111 48 47)

for i in "${input[@]}"
do
   echo "$i" > /sys/class/gpio/export
   echo in > /sys/class/gpio/gpio"$i"/direction 
done

for i in "${output[@]}"
do
   echo "$i" > /sys/class/gpio/export
   echo out > /sys/class/gpio/gpio"$i"/direction 
done
