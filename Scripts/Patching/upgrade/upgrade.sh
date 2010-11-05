#!/bin/bash
echo -n "" ; echo -en "Running upgrade.sh" ; tput sgr0 ; echo " "

sed -i 's/magic_quotes_gpc = On/magic_quotes_gpc = Off/' /etc/php5/cli/php.ini
sed -i 's/post_max_size = 8M/post_max_size = 20M/' /etc/php5/cgi/php.ini

#echo -n "" ; echo -en "\tDescription of what you are doing..." ; tput sgr0 ; echo " "
# Shell script goes here.

