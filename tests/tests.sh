#! /bin/bash

CMDFILE="command.lst"
WDIR="$(pwd)"
VALGRIND="valgrind"
RET=0;

case "$2" in
 dev)
    XMLDIFF="../src/xmldiff"
    ;;
 prev)
    XMLDIFF=xmldiff
    ;;
 *)
    XMLDIFF="../src/xmldiff"
    ;;
esac


echo "Test v0.1 : automated tests for libxmldiff"
echo "Using : $XMLDIFF"
case "$1" in
 clean)
    find . -name "out-*" -exec rm '{}' \;
    find . -name "diff-*" -exec rm '{}' \;
    find . -name "difflog-*" -exec rm '{}' \;
    find . -name "memcheck-*" -exec rm '{}' \;
    ;;
 clean-exp)
    find . -name "exp-*" -exec rm '{}' \;
    ;;
 exp)
    echo "Generating expected results"
    find . | grep "$CMDFILE$" | while read FILE
    do
        DIR=$(dirname "$FILE")
        cd $DIR
        cat $CMDFILE | while read LINE
        do
            ID=$(echo $LINE | cut -d'#' -f 1)
            ARG=$(echo $LINE | cut -d'#' -f 2)
            FILES=$(echo $LINE | cut -d'#' -f 3)
            echo "Generating $DIR - $ID ..."
            echo $FILES | tr "," "\n" | while read FIL
            do
              rm -f $FIL exp-$ID-$FIL >& /dev/null
            done
            CMD="$XMLDIFF $ARG"
            (
             echo $CMD
             if [ -x "$WDIR/$XMLDIFF" ]
             then
                $WDIR/$CMD
             else
                $CMD
             fi
            ) >& "exp-$ID.log"
            echo $FILES | tr "," "\n" | while read FIL
            do
                mv "$FIL" "exp-$ID-$FIL" >& /dev/null
            done
        done
        cd $WDIR
    done
    ;;
 diff)
    echo "Test current results against expected"
    find . | grep -v '.svn' | grep "$CMDFILE" | { while read FILE
    do
        DIR=$(dirname "$FILE")
        cd $DIR
		# Loop in "$CMDFILE"
        while read LINE
        do
            ID=$(echo $LINE | cut -d'#' -f 1)
            ARG=$(echo $LINE | cut -d'#' -f 2)
            FILES=$(echo $LINE | cut -d'#' -f 3)
            echo -n "Comparing $DIR - $ID : "
            rm -f out-$ID.log
            echo $FILES | tr "," "\n" | while read FIL
            do
              rm -f $FIL out-$ID-$FIL >& /dev/null
            done
            MEMCHKLOG="memcheck-$ID.log"
            MEMCHK="$VALGRIND --trace-children=yes --leak-check=full -q --log-file=$MEMCHKLOG"
            CMD="$XMLDIFF $ARG"
            (
             echo $CMD
             if [ -x "$WDIR/$XMLDIFF" ]
             then
                $MEMCHK $WDIR/$CMD
             else
                $MEMCHK $CMD
             fi
            ) >& out-$ID.log
            STATUT="OK"
            REASON=""
            #MEMCHKLOG=$(ls | grep memcheck-$ID.log.pid)
            if [ -s "$MEMCHKLOG" ]
            then
              STATUT="LEAK"
              REASON="$REASON (See $MEMCHKLOG)"
            fi
            if [ -f exp-$ID.log ]
            then 
                diff --ignore-matching-lines="xmldiff " -u exp-$ID.log out-$ID.log > difflog-$ID.txt
                if [ -s difflog-$ID.txt ]
                then
                    STATUT="LOG"
                    REASON="$REASON (Log Differ, see difflog-$ID.txt)" 
                fi
                SUBSHELL=$(
                echo $FILES | tr "," "\n" | while read FIL
                do
                    mv $FIL out-$ID-$FIL >& /dev/null
                    if [ -e out-$ID-$FIL ]
                    then
                     if [ -e exp-$ID-$FIL ]
                     then
                       # Standart case
                       diff -u exp-$ID-$FIL out-$ID-$FIL > diff-$ID-$FIL.txt
                       if [ -s diff-$ID-$FIL.txt ]
                       then
                        echo "See diff-$ID-$FIL.txt for analysis"
                       else
                        rm -f diff-$ID-$FIL.txt
                       fi
                     else
                       echo "No expected file $FIL"
                     fi
                    else
                     if [ -e exp-$ID-$FIL ]
                     then
                       echo "File does not exists $FIL"
                     fi
                    fi
                done
                )
                if [ ! "x$SUBSHELL" = "x" ]
                then
                 STATUT="KO"
                 REASON="($SUBSHELL) $REASON"
                 RET=-1
                fi
            else
                STATUT="? $STATUT"
                REASON="No expected log"
            fi
            echo "$STATUT" "$REASON"
        done < "$CMDFILE" 
        cd $WDIR
    done
	exit $RET
	}
    ;;
 help)
    echo "Possible actions are :"
    echo " help  : this help text."
    echo " exp   : generate expected results."
    echo " diff  : do non-regression tests."
    ;;
 *)
    echo "Unknown command."
    ;;
esac

exit $RET;
