#!/bin/bash
rm -f AirrClientShould
javac AirrClientShould.java

while true;
do
needTOBreak="false"
	for i in {10..99..1}
	do
 		arg="0"
		arg+=$i
		arg+="61"
		reply=`java AirrClientShould $arg`
        	if [ "$reply" != "Got respnse TURNEDON" ]; then
			needTOBreak+="true"
			break;
		fi
		arg="0"
                arg+=$i
                arg+="41"
                reply=`java AirrClientShould $arg`
                if [ "$reply" != "Got respnse TURNEDOFF" ]; then
                        needTOBreak+="true"
                        break;
                fi

	done

	if [ "$needTOBreak" != "false" ]; then

                        break;
        fi

done
