#!/bin/bash
#
# This script will restore a complete backup
# to the specified location,
# including the import of the old database dump
# (which should be in the backup tarball)

# print_usage
#
print_usage()
{
    echo "Usage:"
    echo "$0 /path/to/backup/file.tar.gz /path/to/restore/to"
    echo ""
    echo "If that path exists, it will be removed - so be careful!"
    echo ""
    exit 1
}

print_verbose()
{
    if [ "${VERBOSE}" -eq 1 ]; then
        echo "$1"
    fi
}

# print a message to stderr
print_error()
{
    echo "$1" >&2
}

# solaris 'which' is broken
# it returns a 0 status regardless
# of whether a file exists or not
# so we have to make up our own
# if the first word is 'no'
# the file isn't there.
file_exists()
{
    case "${os}" in
        "SunOS")
            found=`which $1 | cut -d' ' -f1`
            if [ "x$found" = "xno" ]; then
                RET=1
            else
                RET=0
            fi
            return $RET
        ;;
        *)
            found=`which $1`
            return $?
        ;;
    esac
}


if [ "x$1" = "x" ]; then
    print_usage
    exit 1
else
    BACKUP_FILE="$1"
    shift 1
fi

if [ "x$1" = "x" ]; then
    print_usage
    exit 1
else
    RESTORE_FOLDER="$1"
    shift 1
fi

if [ -d "${RESTORE_FOLDER}" ]; then
    curr_dir=`pwd`
    cd "${RESTORE_FOLDER}"
    RESTORE_FOLDER=`pwd`
    cd "${curr_dir}"
fi

VERBOSE=0

while true; do
    case "$1" in
        --verbose)
            VERBOSE=1
        ;;
    esac
    if [ "x$2" = "x" ]; then
        break;
    fi

    shift
done

if [ ! -f "${BACKUP_FILE}" ]; then
    print_error "Unable to find the backup file to restore."
    print_usage
    exit 1
fi

# if the php env var hasn't been set,
# look for php-cli
# then look for php
if [ "x${PHP}" = "x" ]; then
    file_exists "php-cli"
    if [ $? -eq 0 ]; then
        PHP=`which php-cli`
    else
        file_exists "php"
        if [ $? -eq 0 ]; then
            PHP=`which php`
        else
            echo "Cannot find the php binary please be sure to install it"
            exit 1
        fi
    fi
fi

print_verbose ""
print_verbose "Found the backup file."
existing_folder=0
if [ -d "${RESTORE_FOLDER}" ]; then
    existing_folder=1

    print_verbose "Restoring to an existing folder."

    print_verbose ""
    print_verbose "Checking permissions to the parent folder."
    permission_test_file="${RESTORE_FOLDER}/../.permission_test"
    touch "${permission_test_file}" 2>&1 > /dev/null
    if [ $? -gt 0 ]; then
        print_error "Unable to create the folder ${RESTORE_FOLDER} - check permissions."
        exit 2
    fi

    rm -f "${permission_test_file}"

    print_verbose "Permissions ok."
    print_verbose ""
else
    print_verbose "Restoring to a new folder."

    mkdir -p "${RESTORE_FOLDER}" 2>&1 > /dev/null
    if [ $? -gt 0 ]; then
        print_error "Unable to create the restore path - check permissions."
        exit 3
    fi
fi

# If we're not restoring to an existing folder, it's all nice and easy.
if [ "${existing_folder}" -eq 0 ]; then
    print_verbose "Untarring file to ${RESTORE_FOLDER} .. "
    tar -xf "${BACKUP_FILE}" -C "${RESTORE_FOLDER}/.."
    print_verbose "Done"
    print_verbose ""
fi

# If we are restoring to an existing folder, it's a bit harder.
#
# In case something goes wrong during a restore, we should unpack to a temp location
# This will
# - create a temp folder
# - untar
# - switch the folders
if [ "${existing_folder}" -eq 1 ]; then
    print_verbose "Making a temp dir to restore the files to .. "
    temp_folder="${RESTORE_FOLDER}/../restore_temp"
    temp_folder_old="${RESTORE_FOLDER}.old/../restore_temp"

    if [ -d "${temp_folder}" ]; then
        print_verbose "Found an old temp folder, deleting .. "
        rm -rf "${temp_folder}"
        print_verbose "Done"
        print_verbose ""
    fi

    if [ -d "${RESTORE_FOLDER}.old" ]; then
        print_verbose "Found an old restore attempt, deleting .. "
        rm -rf "${RESTORE_FOLDER}.old"
        print_verbose "Done"
        print_verbose ""
    fi

    mkdir "${temp_folder}"
    print_verbose "Untarring file .. "
    tar -xf "${BACKUP_FILE}" -C "${temp_folder}"
    if [ $? -gt 0 ]; then
        print_error "Unable to untar folder to ${temp_folder}"
        exit 4
    fi
    print_verbose "Done"
    print_verbose ""

    # we need just the last folder name
    last_folder=`basename "${RESTORE_FOLDER}"`

    currdir=`pwd`
    cd "${RESTORE_FOLDER}/.."

    print_verbose "Renaming current folder to new folder name .. "
    mv "${RESTORE_FOLDER}" "${RESTORE_FOLDER}.old"
    if [ $? -gt 0 ]; then
        print_error "Unable to move current system out of the way (${RESTORE_FOLDER} to ${RESTORE_FOLDER}.old)"
        exit 5
    fi
    print_verbose "Done"
    print_verbose ""

    print_verbose "Moving temp folder to original folder name .. "
    # Now we need to use the '.old' version
    mv "${temp_folder_old}/${last_folder}" "${RESTORE_FOLDER}"
    if [ $? -gt 0 ]; then
        print_error "Unable to move temp folder to the new location (${temp_folder} to ${RESTORE_FOLDER}"
        exit 6
    fi
    print_verbose "Done"
    print_verbose ""

    print_verbose "Cleaning up .. "
    rmdir "${temp_folder}"
    print_verbose "Done"
    print_verbose ""

    if [ -d "${RESTORE_FOLDER}.old/data/Backup" ]; then
        print_verbose "Found old backups, copying to the new folder .. "
        mkdir -p "${RESTORE_FOLDER}/data/Backup"
        cp -fr ${RESTORE_FOLDER}.old/data/Backup/* ${RESTORE_FOLDER}/data/Backup/
        print_verbose "Done"
    fi

    cd "${currdir}"
fi

print_verbose "Restoring database .. "
data_file=`ls ${RESTORE_FOLDER}/*data*.dump`

DB_PORT="5432"
DB_HOST="localhost"
# OK, what we are doing here is using PHP to do the parsing of the DSN for us (much less error prone :)
php_code="<?php
require_once '${RESTORE_FOLDER}/DAL/DALConf.inc';
function splitdsn(\$input_dsn, \$prefix='DB_')
{
    \$start_pos = strpos(\$input_dsn['DSN'], ':') + 1;
    \$dsn = preg_split('/[\s;]/', substr(\$input_dsn['DSN'], \$start_pos));
    foreach(\$dsn as \$dsn_v) {
        list(\$k, \$v) = explode('=', \$dsn_v);
        \$var = \$prefix . strtoupper(\$k);
        echo \$var .  '=\"' . addslashes(\$v) . '\";';
        echo 'export ' . \$var . ';';
    }

    \$var = \$prefix . 'USERNAME';
        echo \$var . '=\"' . addslashes(\$input_dsn['user']) . '\";';
        echo 'export ' . \$var.';';

    \$var = \$prefix . 'PASSWORD';
        echo \$var . '=\"' . addslashes(\$input_dsn['password']) . '\";';
        echo 'export ' . \$var.';';
}

    \$var = 'DB_TYPE';
    echo \$var . '=\"'.\$conf['type'].'\";';
        echo 'export ' . \$var.';';

if (\$conf['type'] === 'pgsql') {
    splitdsn(\$conf);
}
"

psql=`which psql`

eval `echo "${php_code}" | $PHP`

pgpass_filename="${RESTORE_FOLDER}/.pgpass_${DB_DBNAME}"

print_verbose "Creating pgpass file (${pgpass_filename})"

if [ -f ${pgpass_filename} ]; then
    print_verbose "pgpass file (${pgpass_filename}) already exists. Removing"
    rm -f ${pgpass_filename}
fi

pgpass_string="${DB_HOST}:${DB_PORT}:${DB_DBNAME}:${DB_USERNAME}"
if [ "${DB_PASSWORD}" != "" ]; then
    pgpass_string="${pgpass_string}:${DB_PASSWORD}"
fi

print_verbose "Finished creating pgpass file."

args="-d ${DB_DBNAME} -p ${DB_PORT} -U ${DB_USERNAME}"
if [ "${DB_HOST}" != "localhost" ]; then
    args="${args} -h ${DB_HOST}"
fi

echo $pgpass_string > ${pgpass_filename}

chmod 600 ${pgpass_filename}
oldpassfile=$(echo $PGPASS)
export PGPASSFILE=${pgpass_filename}

# set a timeout for psql
# just in case it's on a remote server and it's down
# or otherwise unavailable
PGCONNECT_TIMEOUT=10
export PGCONNECT_TIMEOUT

print_verbose "Re-creating database .. "
dropdb -U postgres ${DB_DBNAME}
createdb -U postgres -O ${DB_USERNAME} -E SQL_ASCII ${DB_DBNAME}

print_verbose "Importing database .. "
${psql} -q ${args} < ${data_file} 2>&1 > restore_data_output
if [ $? -gt 0 ]; then
    echo "Error restoring data:"
    cat restore_data_output
    rm -f restore_data_output
    exit 7
fi
rm -f restore_data_output
print_verbose "Done"
print_verbose ""

print_verbose "Cleaning up .. "

export PGPASSFILE=${oldpassfile}
rm -f ${pgpass_filename}

# clean up database dump
rm -f ${data_file}

print_verbose "Finished cleaning up."

print_verbose "System restored."

