#!/bin/bash
#
# See print_usage for what the script does.

function print_usage()
{
cat <<EOF
Backup a Squiz Suite system using this script.
The script will dump the database and tar & gzip the root folder.

Pass a Squiz Suite folder name as the first argument, eg:
$0 /path/to/mysource4

If you want to, pass a folder name to place the backups in as the second argument, eg:
$0 /path/to/mysource4 /path/to/backup/folder

It defaults to the current dir.

You can view progress of the script by using --verbose:
$0 /path/to/mysource4 [/path/to/backup/folder] --verbose

If you want to, you can also name your backup files a particular way.
If you do this, you *must* specify where to put the file (even if it's the current directory)
otherwise the backup filename will be mistaken for the backup directory.
You must also specify the extension(s), eg 'filename.tar.gz' or 'filename.tgz'

$0 /path/to/mysource4 /path/to/backup/folder filename.tar.gz

You can specify a user to ssh to a remote server and do a backup through.
This is used for oracle backups since the 'exp' and 'expdb' utilities are server only (not in the oracle client package)
$0 /path/to/mysource4 [/path/to/backup/folder] [--verbose] --remotedb=username@hostname

It will try to ssh to the remote server using username@hostname

EOF
	exit 1
}

# print_info
# Prints a message if the script isn't being run via cron.
function print_info()
{
	if [[ ${CRON_RUN} -eq 0 ]]; then
		echo $1
	fi
}
function print_verbose()
{
	if [[ ${VERBOSE} -eq 1 ]]; then
		echo $1
	fi
}

# Usage:
# pg_dbdump $dbname $username $pass $host $port
#
# even if pass/host/port are empty.
#
# It does a full db dump in one go
# because a schema-only then data-only dump
# would cause issues with foreign keys
# doing it in one go doesn't have this issue
#
function pg_dbdump()
{
	db=$1
	username=$2
	pass=$3
	host=$4
	port=$5

	if [[ "${host}" == "" ]]; then
		host='localhost'
	fi

	if [[ "${port}" == "" ]]; then
		port=5432
	fi

	if [[ ${username} == "" ]]; then
		print_verbose ""
		echo "You can't create a backup of database ${db} without a database username."
		echo "The database has not been included in the backup."
		print_verbose ""
		return
	fi

	pgdump=$(which pg_dump)
	if [[ $? -gt 0 ]]; then
		print_verbose ""
		echo "Unable to create postgres dump."
		echo "Make sure 'pg_dump' is in your path."
		echo "The database has not been included in the backup."
		print_verbose ""
		return
	fi

        # set a timeout for pg_dump
        # just in case it's on a remote server and it's down
        # or otherwise unavailable
        PGCONNECT_TIMEOUT=10
        export PGCONNECT_TIMEOUT

	pgpass_filename="${SYSTEM_ROOT}/.pgpass_${db}"

	print_verbose "Creating pgpass file (${pgpass_filename})"

	if [[ -f ${pgpass_filename} ]]; then
		print_info "pgpass file (${pgpass_filename}) already exists. Removing"
		rm -f ${pgpass_filename}
	fi

	pgpass_string="${host}:${port}:${db}:${username}"
	if [[ "${pass}" != "" ]]; then
		pgpass_string="${pgpass_string}:${pass}"
	fi

	print_verbose "Finished creating pgpass file."

	args="--ignore-version -p ${port} -U ${username}"
	if [[ "${host}" != "localhost" ]]; then
		args="${args} -h ${host}"
	fi

	dumpfileprefix=${db}

	args="${args} --no-owner --no-privileges"
	dumpfileprefix="${dumpfileprefix}-data"

	dumpfile=${SYSTEM_ROOT}/${dumpfileprefix}-`date +%Y-%m-%d_%H-%M`.dump

	echo "${dumpfile}" >> ${SYSTEM_ROOT}/.extra_backup_files

	echo $pgpass_string > ${pgpass_filename}

	chmod 600 ${pgpass_filename}
	oldpassfile=$(echo $PGPASS)
	export PGPASSFILE=${pgpass_filename}

	print_verbose "Dumping database out to ${dumpfile} .. "

	outputfile="${SYSTEM_ROOT}/pgdumpoutput"
	${pgdump} ${args} "${db}" > ${dumpfile} 2>${outputfile}

	if [[ $? -gt 0 ]]; then
		print_verbose ""
		echo "*** Unable to create dumpfile ${dumpfile}."
		echo ""
		(cat ${outputfile})
		print_verbose ""
	else
		print_verbose "Finished dumping database."
	fi

	rm -f ${outputfile}

	print_verbose "Cleaning up temp pgpass file .. "

	export PGPASSFILE=${oldpassfile}
	rm -f ${pgpass_filename}

	print_verbose "Finished cleaning up temp pgpass file."
}

if [[ -z $1 ]]; then
	print_usage
	exit 1
fi

SYSTEM_ROOT=$(readlink -f "$1")

shift

if [[ ! -f ${SYSTEM_ROOT}/DAL/DALConf.inc ]]; then
	echo "The directory you supplied is not a mysource4 system."
	echo ""
	print_usage
	exit 1
fi

backupdir=""
REMOTE_USER=""
VERBOSE=0
backupfilename_prefix=$(basename $SYSTEM_ROOT)
backupfilename="${backupfilename_prefix}-`date +%Y-%m-%d_%H-%M`-backup.tar.gz"

while true; do
	case "$1" in
		--verbose)
			VERBOSE=1
		;;
		--remotedb=*)
			REMOTE_USER=$1
		;;
		*)
			if [ "x$backupdir" = "x" ]; then
				backupdir=$1
			else
				backupfilename=$1
			fi
		;;
	esac
	shift

	if [[ -z $1 ]]; then
		break;
	fi
done

if [ "x$backupdir" = "x" ]; then
	backupdir="."
fi

# do this - should clean up passing in stuff like "." and "path/"
# so we get a consistent path (no trailing / - stuff like that)
backupdir=$(readlink -f "${backupdir}")

if [[ ! -d "${backupdir}" ]]; then
	mkdir -p "${backupdir}"
	if [[ $? -gt 0 ]]; then
		echo "Unable to create backup dir (${backupdir})."
		echo "Aborting"
		exit 1
	fi
fi

if [[ -f "${SYSTEM_ROOT}/.extra_backup_files" ]]; then
	rm -f "${SYSTEM_ROOT}/.extra_backup_files"
fi

touch "${SYSTEM_ROOT}/.extra_backup_files"

CRON_RUN=0
tty -s
if [[ $? -gt 0 ]]; then
	CRON_RUN=1
fi


if [[ -n ${PHP} ]] && [[ -e ${PHP} ]];then
	PHP=${PHP}
elif which php-cli 2>/dev/null >/dev/null;then
	PHP="php-cli"
elif which php 2>/dev/null >/dev/null;then
	PHP="php"
else
	echo "Cannot find the php binary please be sure to install it"
	exit 1
fi

# OK, what we are doing here is using PHP to do the parsing of the DSN for us (much less error prone :)
php_code="<?php
require_once '${SYSTEM_ROOT}/DAL/DALConf.inc';
function splitdsn(\$input_dsn, \$prefix='DB_')
{
        \$start_pos = strpos(\$input_dsn['DSN'], ':') + 1;
        \$dsn = preg_split('/[\s;]/', substr(\$input_dsn['DSN'], \$start_pos));
        foreach(\$dsn as \$dsn_v) {
                list(\$k, \$v) = explode('=', \$dsn_v);
                echo \$prefix .strtoupper(\$k).'=\"'.addslashes(\$v).'\";';
        }

        echo \$prefix . 'USERNAME=\"'.\$input_dsn['user'].'\";';
        echo \$prefix . 'PASSWORD=\"'.\$input_dsn['password'].'\";';
}

echo 'DB_TYPE=\"'.\$conf['type'].'\";';

if (\$conf['type'] === 'pgsql') {
	splitdsn(\$conf);

} else {
	echo 'DB_HOST=\"'.\$conf['DSN'].'\";';
	echo 'DB_USERNAME=\"'.\$conf['user'].'\";';
	echo 'DB_PASSWORD=\"'.\$conf['password'].'\";';
}
"

eval `echo "${php_code}" | $PHP`

# Usage:
# oracle_dbdump $remote_user $user $pass $hostspec
# the hostspec is
# //localhost|ip.addr/dbname
# which is broken up inside the fn to do the right thing.
function oracle_dbdump()
{
	remote_user=$1
	username=$2
	pass=$3
	hostspec=$4

	# The oracle dsn is in the format of:
	# //localhost|ip.addr/dbname
	# Split it up so we just get the dbname, then set the oracle_sid to the right thing.
	db=$(echo $hostspec | awk -F'/' '{ print $NF }')
	old_oracle_sid=$(echo $ORACLE_SID)
	if [[ -z ${remote_user} ]]; then
		export ORACLE_SID=$db
	fi

	# Also need the hostname to see if we need to do a remote dump
	dbhost=$(echo $hostspec | awk -F'/' '{ print $1 }')
	print_verbose ""
	print_verbose "Found a dbhost of ${dbhost}"
	print_verbose ""
	if [ ! $dbhost == "localhost" ]; then
		if [[ -z $remote_user ]]; then
			print_verbose ""
			echo "To do remote oracle backups, please supply '--remotedb=username@hostname'"
			echo "The database has not been included in the backup."
			print_verbose ""
			return
		fi
	fi

	if [[ -z $remote_user ]]; then
		oracle_exp=$(which exp)
	else
		oracle_exp=$(ssh "${remote_user}" 'which exp')
	fi

	if [[ $? -gt 0 ]]; then
		print_verbose ""
		echo "Unable to create oracle dump."
		echo "Make sure 'exp' is in your path."
		echo "The database has not been included in the backup."
		print_verbose ""
		return
	fi

	if [[ -z $remote_user ]]; then
		home=$(echo $ORACLE_HOME)
	else
		home=$(ssh "${remote_user}" 'echo $ORACLE_HOME')
	fi
	if [[ $? -gt 0 ]] || [[ "${home}" == "" ]]; then
		print_verbose ""
		echo "Unable to create oracle dump."
		echo "Make sure the 'ORACLE_HOME' environment variable is set"
		echo "The database has not been included in the backup."
		print_verbose ""
		return
	fi

	print_verbose "Creating oracle db dump .. "

	dump_args="${username}/${pass}@localhost/${db}"

	dumpfileprefix=${db}

	if [[ -z ${remote_user} ]]; then
		dumpfilepath=${SYSTEM_ROOT}
	else
		dumpfilepath='.'
	fi

	dumpfile=${dumpfilepath}/${dumpfileprefix}-`date +%Y-%m-%d_%H-%M`.dump

	print_verbose "Dumping database out to ${dumpfile} .. "

	oracle_args="consistent=y"

	outputfile="${SYSTEM_ROOT}/oracleoutput"
	if [[ -z $remote_user ]]; then
		$(${oracle_exp} ${dump_args} ${oracle_args} File=${dumpfile} 2> ${outputfile})
	else
		outputfile='./oracleoutput'
		ssh "${remote_user}" "${oracle_exp} ${dump_args} ${oracle_args} File=${dumpfile} 2> ${outputfile}"
	fi

	if [[ $? -gt 0 ]]; then
		echo "The oracle dump may have contained errors. Please check it's ok"
		if [[ -z $remote_user ]]; then
			output=$(cat ${outputfile})
		else
			output=$(ssh "${remote_user}" cat ${outputfile})
		fi
		echo $output
	fi

	if [[ -z $remote_user ]]; then
		rm -f ${outputfile}
	else
		scp -q "${remote_user}:${dumpfile}" "${SYSTEM_ROOT}/${dumpfile}"
		if [[ $? -gt 0 ]]; then
			echo "Unable to copy the oracle dump file back."
			echo "Tried to run"
			echo "scp ${remote_user}:${dumpfile} ${SYSTEM_ROOT}/${dumpfile}"
			echo "The database has not been included in the backup."
			return
		fi
		ssh "${remote_user}" 'rm -f ${outputfile} ${dumpfile}'
	fi

	if [[ -z ${remote_user} ]]; then
		export ORACLE_SID=$old_oracle_sid
	fi

	print_verbose "Finished dumping database."
}

case "${DB_TYPE}" in
	"pgsql")
		pg_dbdump "${DB_DBNAME}" "${DB_USERNAME}" "${DB_PASSWORD}" "${DB_HOST}" "${DB_PORT}"
	;;

	"oci")
		oracle_dbdump "${REMOTE_USER}" "${DB_USERNAME}" "${DB_PASSWORD}" "${DB_HOST}"
	;;

	*)
		echo "ERROR: DATABASE TYPE '${DB_TYPE}' NOT KNOWN" >&2;
		exit 6
esac

#
# We do the tar and gzip separately so systems like Solaris
# that don't support gzip in tar will work.
#

#
# -C `dirname $SYSTEM_ROOT` means we change to the directory below this one.
# The `basename $SYSTEM_ROOT` means we tar up this directory only (no full paths) in the tarball
# If we're not specifying the file location, then we need to make sure this won't be a recursive tarball!
# Hence the --exclude....
#

sysroot_base=$(basename "${SYSTEM_ROOT}")
sysroot_dir=$(dirname ${SYSTEM_ROOT})

# So we get the right relative paths for the exclude file,
# so solaris tar excludes them properly:
#
# go to the dir under the SYSTEM_ROOT
# then do a find
# so the paths end up as:
# folder/filename
#

mydir=$(pwd)
cd "${sysroot_dir}/"

exclude_file="${backupdir}/.tar_exclude_list"
print_verbose "Creating an exclude file .. "

# The exclude list should have a relative path (in case a relative path is supplied)
# and a full path (in case of that)
# just build both at the same time and don't worry about which is really supplied
#
echo "${sysroot_base}/.extra_backup_files" > ${exclude_file}
echo "${SYSTEM_ROOT}/.extra_backup_files" >> ${exclude_file}

# We need a relative path to exclude for the backup filename
# Simple in php, not so simple in bash.
php_code="<?php
echo 'relative_path=\"'. str_replace('${sysroot_dir}/', '', '${backupdir}') . '\";';
";

eval `echo "${php_code}" | $PHP`

echo "${relative_path}/${backupfilename}" >> ${exclude_file}
echo "${backupdir}/${backupfilename}" >> ${exclude_file}

# ignore all *-backup.tar* files.
for file in `find "${sysroot_base}" -name cache -o -name data -prune -o -type f -name '*-backup.tar*' -print`; do
	echo "${file}" >> "${exclude_file}"
	echo "${sysroot_dir}/${file}" >> "${exclude_file}"
done

# the my4 cron job puts things in the sysroot_base/data/Backup folder, so exclude everything in there.
echo "${sysroot_base}/data/Backup/*" >> ${exclude_file}

echo "${sysroot_base}/cache/*" >> ${exclude_file}
echo "${SYSTEM_ROOT}/cache/*" >> ${exclude_file}

print_verbose "Done"
print_verbose ""

cd "${mydir}"

# Of course the tar syntax is slightly different for different os's
os=`uname`

# if tar supports gzip itself, we will do everything in one step
# if it doesn't (solaris tar for example does not)
# it will be done in two steps
# - 1) tar the system
# - 2) gzip the tarball.

tar_gzip=0
case "${os}" in
	"SunOS")
		file_exists "gtar"
		if [ $? -eq 0 ]; then
			tar_gzip=1
			tar_command=`which gtar`
		fi
	;;

	*)
		tar_gzip=1
		tar_command=`which tar`
esac

if [ "${tar_gzip}" -eq 0 ]; then
	# if gtar is not present, use the solaris tar & then gzip the tarball.
	# solaris tar doesn't support gzipping in the same process.
	print_verbose "Tar'ing up the ${SYSTEM_ROOT} folder to ${backupdir}/${backupfilename} .. "

	tar -cfX "${backupdir}/${backupfilename}" "${exclude_file}" -C `dirname ${SYSTEM_ROOT}` "${sysroot_base}"

	print_verbose "Gzipping ${backupdir}/${backupfilename} .. "

	gzip -f ${backupdir}/${backupfilename}

	# gzip *always* adds a .gz extension. You can't stop it.
	backupfilename="${backupfilename}.gz"

	if [ $? -gt 0 ]; then
		print_verbose ""
		print_error "*** Unable to gzip tarball ${backupdir}/${backupfilename}."
		print_verbose ""
	else
		print_verbose "Finished gzipping up ${backupdir}/${backupfilename}."
	fi
else
	print_verbose "Tar'ing & gzipping up the ${SYSTEM_ROOT} folder to ${backupdir}/${backupfilename} .. "
	"${tar_command}" -czf "${backupdir}/${backupfilename}" -X "${exclude_file}" -C `dirname ${SYSTEM_ROOT}` "${sysroot_base}"
	print_verbose "Finished Tar'ing & gzipping up the ${SYSTEM_ROOT} folder to ${backupdir}/${backupfilename}."
fi

print_verbose "Finished Tar'ing up the ${SYSTEM_ROOT} folder to ${backupdir}/${backupfilename}."

print_verbose ""
print_verbose "Removing tar exclude list .. "
rm -f ${exclude_file}
print_verbose "Done"
print_verbose ""

files=$(cat ${SYSTEM_ROOT}/.extra_backup_files)
for file in $files; do
	rm -f "${file}"
	if [[ $? -gt 0 ]]; then
		print_verbose ""
		echo "Unable to clean up file ${file}."
		print_verbose ""
	fi
done

file="${SYSTEM_ROOT}/.extra_backup_files"
rm -f "${file}"
if [[ $? -gt 0 ]]; then
	print_verbose ""
	echo "Unable to clean up file ${file}."
	print_verbose ""
fi

print_verbose "Finishing cleaning up."

print_info ""
print_info "Your system is backed up to ${backupdir}/${backupfilename}"
print_info ""

exit 0

