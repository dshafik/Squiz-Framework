#!/bin/bash
echo -n "" ; echo -en "Running preupgrade.sh" ; tput sgr0 ; echo " "

echo -n "" ;
echo -en "\tStop the system cron service to prevent in any case upgrade scripts run more than once." ; tput sgr0 ; echo " "
echo '#* * * * * root /var/www/mysource4/htdocs/Scripts/network_update.sh
#*/5 * * * * root /usr/bin/php -f /var/www/mysource4/htdocs/Systems/Cron/Scripts/run.php &> /dev/null && /var/www/mysource4/htdocs/Scripts/fix_perms.sh
#* * * * * root /usr/bin/php -f /var/www/mysource4/htdocs/restore.php
#* * * * * root /usr/bin/php -f /var/www/mysource4/htdocs/activateProject.php' > /etc/cron.d/mysource4

#echo -n "" ;
#echo -en "\tMsg here." ; tput sgr0 ; echo " "
