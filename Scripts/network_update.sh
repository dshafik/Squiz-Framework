#!/bin/sh
FILE_LOCATION="/var/www/mysource4/htdocs/data"

INTERFACES_FILE="${FILE_LOCATION}/interfaces"
RESOLV_FILE="${FILE_LOCATION}/resolv.conf"
HOSTNAME_FILE="${FILE_LOCATION}/hostname"
MAILNAME_FILE="${FILE_LOCATION}/mailname"
APPLIED_FLAG_FILE="${FILE_LOCATION}/network_changes_applied"

INTERFACES_FILE_OLD="${FILE_LOCATION}/interfaces_old"
RESOLV_FILE_OLD="${FILE_LOCATION}/resolv.conf_old"
HOSTNAME_FILE_OLD="${FILE_LOCATION}/hostname_old"
MAILNAME_FILE_OLD="${FILE_LOCATION}/mailname_old"
NEW_SETTING_FAILED_FLAG_FILE="${FILE_LOCATION}/network_changes_failed"

# network configuration files are written. let's do some work
if [ -f "${INTERFACES_FILE}" -o -f "${RESOLV_FILE}" -o -f "$HOSTNAME_FILE" -o -f "$MAILNAME_FILE" ]; then

    # create backup files first.
    cp /etc/network/interfaces "${INTERFACES_FILE_OLD}"
    cp /etc/resolv.conf "${RESOLV_FILE_OLD}"
    cp /etc/hostname "${HOSTNAME_FILE_OLD}"
    cp /etc/mailname "${MAILNAME_FILE_OLD}"

    # in case mv has '-i' set, get rid of it.
    unalias mv 2>/dev/null

    mv -f "${INTERFACES_FILE}" /etc/network/interfaces
    mv -f "${RESOLV_FILE}" /etc/resolv.conf
    mv -f "${HOSTNAME_FILE}" /etc/hostname
    mv -f "${MAILNAME_FILE}" /etc/mailname

    # restart the network
    /etc/init.d/networking restart

    # re-read the hostname file
    hostname -F /etc/hostname

    # need to reload the filtergen rules
    # since it references eth0
    /usr/sbin/fgadm reload

    # let's restart exim4
    /etc/init.d/exim4 restart

    # tell the client that the changes have been applied so they can delete old
    # settings if they can get to.
    touch "${APPLIED_FLAG_FILE}"
    exit
fi

# old backup file still exist! obviously the client couldn't get back to the server.
# let's put the old settings back so that the administrator can access the site again.
if [ -f "${APPLIED_FLAG_FILE}" -o -f "${INTERFACES_FILE_OLD}" -o -f "${INTERFACES_FILE_OLD}" -o -f "${INTERFACES_FILE_OLD}" -o -f "${INTERFACES_FILE_OLD}" ]; then
    mv -f "${INTERFACES_FILE_OLD}" /etc/network/interfaces
    mv -f "${RESOLV_FILE_OLD}" /etc/resolv.conf
    mv -f "${HOSTNAME_FILE_OLD}" /etc/hostname
    mv -f "${MAILNAME_FILE_OLD}" /etc/mailname

    # restart the network
    /etc/init.d/networking restart

    # re-read the hostname file
    hostname -F /etc/hostname

    # need to reload the filtergen rules
    # since it references eth0
    /usr/sbin/fgadm reload

    # let's restart exim4
    /etc/init.d/exim4 restart

    # tell the client that their previous settings failed!
    touch "${NEW_SETTING_FAILED_FLAG_FILE}"

    rm  "${APPLIED_FLAG_FILE}"
    exit;
fi
