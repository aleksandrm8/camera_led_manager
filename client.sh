#!/bin/bash

# This software is licensed under the GNU General Public License,
# version 2 (GPLv2) (see LICENSE for details).
# You are free to use this software under the terms of the GNU General
# Public License v2, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CUR_ITEM=1
ITEMS_COUNT=3
SEL_ITEM_BG=4
BG=0
FG=2

CL_FIFO_NAME="clients_namegement_fifo"

#LED state
LED_STATE="ON"
function state_increase {
  if [[ ${LED_STATE} == "ON" ]]
  then
    LED_STATE="OFF"
  else
    LED_STATE="ON"
  fi
}
function state_decrease {
  state_increase;
}
#LED color
LED_COLOR="RED"
function color_increase {
  case ${LED_COLOR} in
    [RED]* )
      LED_COLOR="GREEN"
      ;;
    [GREEN]* )
      LED_COLOR="BLUE"
      ;;
    [BLUE]* )
      LED_COLOR="RED"
      ;;
    * )
      echo "Wrong color ${LED_COLOR}"
      ;;
  esac
}
function color_decrease {
  case ${LED_COLOR} in
    [RED]* )
      LED_COLOR="BLUE"
      ;;
    [GREEN]* )
      LED_COLOR="RED"
      ;;
    [BLUE]* )
      LED_COLOR="GREEN"
      ;;
    * )
      echo "Wrong color ${LED_COLOR}"
      ;;
  esac
}
#LED rate
LED_RATE=0
LED_RATE_MIN=0
LED_RATE_MAX=5
function rate_increase {
  if [[ ${LED_RATE} < ${LED_RATE_MAX} ]]
  then
    LED_RATE=$((${LED_RATE}+1))
  fi
}
function rate_decrease {
  if [[ ${LED_RATE} > ${LED_RATE_MIN} ]]
  then
    LED_RATE=$((${LED_RATE}-1))
  fi
}

function menu_root {
  tput civis

	while true; do

    echo -n "$(tput setab ${BG} tput setaf ${FG})"
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
     
    read -t 1 -n 1 KEY

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
        rm ${fifo_name}_cmd
        rm ${fifo_name}_resp
        break;;
			* )
      ;;
		esac
	done
}

fifo_name=$(cat /dev/urandom | tr -dc _A-Z-a-z-0-9 | head -c 50)
echo "fifo_name=${fifo_name}"
mkfifo ${fifo_name}_cmd
mkfifo ${fifo_name}_resp

if [[ ! -f ${CL_FIFO_NAME} ]]
then
  echo "Can not connec to server"
  exit 1
fi

menu_root;
