#!/bin/bash

echo Compute all Returns:
 #read -p 'Month: ' MONTH
 read -p 'Start Year: ' SY
 read -p 'End Year: '   EY
 read -p '1st body of interest: ' P1
 read -p '2nd body of interest: ' P2


if [ ! -z P1 ]; then

       if [ ! -z P2 ]; then


            for i in `seq $SY $EY`; do  ./astrolog/astrolog -ty  $i     -R0 1 2 3 4 5 6 7 8 9 10 11 22 31    -qa  4 8 1972 00:00 -12 176:14E 38:08S  |  grep  -w "$P1" | grep -w "$P2"  ;done

        else

            for i in `seq $SY $EY`; do  ./astrolog/astrolog  -ty  $i      -R0 1 2 3 4 5 6 7 8 9 10 11 22 31   -qa  4 8 1972 00:00 -12 176:14E 38:08S  |  grep  -w "$P1" ;done
       fi

else


        for i in `seq $SY $EY`; do  ./astrolog/astrolog  -ty   $i      -R0 1 2 3 4 5 6 7 8 9 10 11 22 31    -qa  4 8 1972 00:00 -12 176:14E 38:08S ; done

fi





#./astrolog/astrolog  -o "mychart" -qb 4 8 1972 00:00 0 12E 176:14E 38:08S

#./astrolog/astrolog   -j  -a0 -R0 1 2 3 4 5 6 7 8 9 10 11 22 31 -qb 4 8 1972 00:00 0 12E 176:14E 38:08S -Yi ~/astrolog/





# ./astrolog/astrolog  -X -R0 1 2 3 4 5 6 7 8 9 10 11 22 31 -qb 4 8 1972 00:00 0 12E 176:14E 38:08S




# ./astrolog/astrolog  -j -a0 -R0 1 2 3 4 5 6 7 8 9 10 11 22 31 -qb 4 8 1972 00:00 0 12E 176:14E 38:08S -Yi ~/astrolog/ &
