#!/bin/bash

# This software is licensed under the GNU General Public License,
# version 2 (GPLv2) (see LICENSE for details).
# You are free to use this software under the terms of the GNU General
# Public License v2, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CUR_ITEM=1
CUR_ITEM_BACKUP=1
ITEMS_COUNT=3
SEL_ITEM_BG=4
BG=0
FG=2

CL_FIFO_NAME="clients_mgm_fifo"
CLIENT_ID=$(cat /dev/urandom | tr -dc _A-Z-a-z-0-9 | head -c 50)
INPUT_FIFO="${CLIENT_ID}_input"
OUTPUT_FIFO="${CLIENT_ID}_output"

#LED state
LED_STATE="on"
LED_STATE_BACKUP="on"
function state_action () {
  echo "set-led-state ${1}" > ${INPUT_FIFO}
  res=`cat ${OUTPUT_FIFO}`
}
function state_increase {
  if [[ ${LED_STATE} == "on" ]]
  then
    #LED_STATE="off"
    state_action "off";
  fi
}
function state_decrease {
  if [[ ${LED_STATE} == "off" ]]
  then
    #LED_STATE="on"
    state_action "on";
  fi
}
#LED color
LED_COLOR="red"
LED_COLOR_BACKUP="red"
function color_action {
  echo "set-led-color ${LED_COLOR}" > ${INPUT_FIFO}
  res=`cat ${OUTPUT_FIFO}`
}
function color_increase {
  case ${LED_COLOR} in
    [red]* )
      LED_COLOR="green"
      color_action;
      ;;
    [green]* )
      LED_COLOR="blue"
      color_action;
      ;;
    [blue]* )
      ;;
    * )
      echo "Wrong color ${LED_COLOR}"
      ;;
  esac
}
function color_decrease {
  case ${LED_COLOR} in
    [red]* )
      ;;
    [green]* )
      LED_COLOR="red"
      color_action;
      ;;
    [blue]* )
      LED_COLOR="green"
      color_action;
      ;;
    * )
      echo "Wrong color ${LED_COLOR}"
      ;;
  esac
}
#LED rate
LED_RATE=0
LED_RATE_BACKUP=0
LED_RATE_MIN=0
LED_RATE_MAX=5
function rate_action {
  echo "set-led-rate ${LED_RATE}" > ${INPUT_FIFO}
  res=`cat ${OUTPUT_FIFO}`
}
function rate_increase {
  if [[ ${LED_RATE} < ${LED_RATE_MAX} ]]
  then
    LED_RATE=$((${LED_RATE}+1))
    rate_action;
  fi
}
function rate_decrease {
  if [[ ${LED_RATE} > ${LED_RATE_MIN} ]]
  then
    LED_RATE=$((${LED_RATE}-1))
    rate_action;
  fi
}

check_trailing_new_line_ret=""
function check_trailing_new_line () {
  str=$1
  if [[ ${str:$((${#str}-1)):1} = $'\n' ]]
  then
    check_trailing_new_line_ret=${str:$((${#str}-1))}
  else
    tput clear
    tput sgr0
    tput rc
    echo "no new line at end of command \"$1\""
    exit 1
  fi
  return 0
}

#polling function
function poll {
  
  echo "get-led-state" > ${INPUT_FIFO}
  ans=`cat ${OUTPUT_FIFO}`
  #check_trailing_new_line $ans
  #ans=$check_trailing_new_line_ret
  new_state=`expr match "${ans}" '^OK \(on\|off\).*$'`
  if [[ "$new_state" = "" ]];
  then
    tput clear
    tput sgr0
    tput rc
    echo "failed led state \"$ans\""
    exit 1
  else
    LED_STATE=$new_state
  fi

  echo "get-led-color" > ${INPUT_FIFO}
  ans=`cat ${OUTPUT_FIFO}`
  new_state=`expr match "${ans}" '^OK \(red\|green\|blue\).*$'`
  if [[ "$new_state" = "" ]];
  then
    tput clear
    tput sgr0
    tput rc
    echo "failed led color \"$ans\""
    exit 1
  else
    LED_COLOR=$new_state
  fi

  echo "get-led-rate" > ${INPUT_FIFO}
  ans=`cat ${OUTPUT_FIFO}`
  new_state=`expr match "${ans}" '^OK \(\([0-9]\+\.*[0-9]*\)\|\([0-9]\+\,*[0-9]*\)\).*$'`
  if [[ "$new_state" = "" ]];
  then
    tput clear
    tput sgr0
    tput rc
    echo "failed led rate \"$ans\""
    exit 1
  else
    LED_RATE=$new_state
  fi
}


    echo -n "$(tput setab ${BG} tput setaf ${FG})"

function repaint {
    tput clear
     
    # Set a foreground colour using ANSI escape
    tput setaf 6
    tput cup 13 15
    echo "j,k: select item"
    tput cup 14 15
    echo "h,l: change value"
    tput cup 15 15
    echo "q: quit"
    tput sgr0
     
    tput setaf 3
    tput cup 5 17
    # Set reverse video mode
    echo "CAMERA LED MANAGEMENT"
    tput sgr0
     
    # item 1
    tput cup 7 15
    if [[ ${CUR_ITEM} == "1" ]]
      then tput rev
    fi
    echo "1. LED state [${LED_STATE}]"
    tput sgr0
     
    tput cup 8 15
    if [[ ${CUR_ITEM} == "2" ]]
      then tput rev
    fi
    echo "2. LED color [${LED_COLOR}]"
    tput sgr0
     
    tput cup 9 15
    if [[ ${CUR_ITEM} == "3" ]]
      then tput rev
    fi
    echo "3. LED rate [${LED_RATE}]"
    tput sgr0
}

function menu_root {
  tput civis

	while true; do

    if [[ $LED_STATE != $LED_STATE_BACKUP ]] \
      || [[ $LED_COLOR != $LED_COLOR_BACKUP ]] \
      || [[ $LED_RATE != $LED_RATE_BACKUP ]] \
      || [[ $CUR_ITEM != $CUR_ITEM_BACKUP ]]
    then
      repaint;
    fi

    CUR_ITEM_BACKUP=$CUR_ITEM
    LED_STATE_BACKUP=$LED_STATE
    LED_COLOR_BACKUP=$LED_COLOR
    LED_RATE_BACKUP=$LED_RATE
     
    read -t 1 -n 1 KEY
    poll

		case $KEY in
			[k]* )
        if [[ ${CUR_ITEM} > 1 ]]
        then
          CUR_ITEM=$((${CUR_ITEM}-1))
        fi
        ;;
			[j]* )
        if [[ ${CUR_ITEM} < ${ITEMS_COUNT} ]]
        then
          CUR_ITEM=$((${CUR_ITEM}+1))
        fi
				;;
			[l]* )
        case ${CUR_ITEM} in
          [1]* )
            state_increase;
            ;;
          [2]* )
            color_increase;
            ;;
          [3]* )
            rate_increase;
            ;;
          * )
        esac
				;;
			[h]* )
        case ${CUR_ITEM} in
          [1]* )
            state_decrease;
            ;;
          [2]* )
            color_decrease;
            ;;
          [3]* )
            rate_decrease;
            ;;
          * )
        esac
				;;
			[q]* )
        tput clear
        tput sgr0
        tput rc
        echo "delete client ${CLIENT_ID}" > ${CL_FIFO_NAME}
        break;;
			* )
      ;;
		esac


	done
}

echo "fifo_name=${CLIENT_ID}"

if [[ ! -p ${CL_FIFO_NAME} ]]
then
  echo "Can not connec to server"
  exit 1
fi
echo "start write"
echo "add client ${CLIENT_ID}" > ${CL_FIFO_NAME}
echo "stop write"
sleep 1
if [[ ! -p ${INPUT_FIFO} ]] || [[ ! -p ${OUTPUT_FIFO} ]]
then
  echo "Can not register new client"
  exit 1
fi

poll

menu_root;
#while true; do
#  sleep 1
#done
