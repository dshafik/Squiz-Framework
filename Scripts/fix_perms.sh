#!/bin/sh

# Where are we running?
self=`readlink -f $0`
mydir=`dirname $self`
basedir=`dirname $mydir`

# Fix data and web directory ownership and permissions.
dirs="$basedir/data $basedir/web"
for dir in $dirs; do
    if [ -d "$dir" ] ; then
        find $dir ! -user www-data -exec chown www-data:www-data {} \;
        find $dir -type d ! -perm 0755 -exec chmod 755 {} \;
        find $dir -type f ! -perm 0644 -exec chmod 644 {} \;
    fi
done

#Finally, fix the perm of the error_log
if [ ! -e "$basedir/error_log" ]; then
    touch $basedir/error_log
fi
chown www-data:www-data $basedir/error_log
