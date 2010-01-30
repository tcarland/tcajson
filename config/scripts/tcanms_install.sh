#!/bin/bash
#
#  tcanms_install.sh
#
VERSION="0.32"
AUTHOR="tcarland@gmail.com"

#PNAME=${0/#.\//}
PNAME=${0##*\/}
RSYNC="rsync"
CONFIGDIR=
PREFIX=
SUBDIRS="bin sbin etc run tmp logs"

CURDIR=`dirname $0`
RETVAL=0

# ------------------------------------------
#  Setup
if [ "$CURDIR" == "." ]; then
    CURDIR=${PWD}
fi
CONFIGDIR=$CURDIR

echo ""
if [ -z "$RC_TCANMS_BASHRC" ]; then
    if [ -e $CONFIGDIR/tcanmsrc ]; then
        echo "$PNAME: using rc: $CONFIGDIR/tcanmsrc"
        source $CONFIGDIR/tcanmsrc
    elif [ -e $HOME/tcanmsrc ]; then
        echo "$PNAME: using rc from: $HOME/tcanmsrc"
        source $HOME/tcanmsrc
    else
        echo "$PNAME: Error: Failed to locate rc file: tcanmsrc"
        exit 1
    fi
fi
echo ""

# we remap to our destination
PREFIX=$TCANMS_PREFIX
export TCANMS_HOME="${PREFIX}"
export TCANMS_BIN=${TCANMS_HOME}/bin
export TCANMS_SBIN=${TCANMS_HOME}/sbin
export TCANMS_TMP=${TCANMS_HOME}/tmp
export TCANMS_LOGS=${TCANMS_HOME}/logs

# paths to create
for path in $SUBDIRS; do
    PATHLIST+="${PREFIX}/$path "
done

# globals
ENVIR=$TCANMS_ENV
HOST=$TCANMS_HOST
# flags
FORCE=

# ------------------------------------------

usage()
{
    echo ""
    echo "Usage: $PNAME [-fhv] [-e environment] [-t targethost]"
    echo ""
    #echo "   -D | --database    : generate dbcreate scripts, exec and exit "
    echo "   -f | --force       : force overwrite of install target '${TCANMS_PREFIX}'"
    echo "   -h | --help        : display this help and exit"
    echo "   -v | --version     : display verion info and exit"
    echo "   -e | --environment : environment name from which to sync configs "
    echo "                         ( or the TCANMS_ENV var (optional) )"
    echo "   -t | --target      : name of the target host (or TCANMS_HOST)"
    echo "                         (requires -e or TCANMS_ENV to be set)"
    echo ""
    echo "   If the 'environment' and 'target' flags will sync the configs for the provided "
    echo "   environment and host within 'config/environment/envname/target'"
    echo "   If not provided, vars TCANMS_ENV and TCANMS_HOST will be used if set." 
    echo "   The script requires the environment variable TCANMS_PREFIX as the "
    echo "   target path to install. The current settings are as follows: "
    echo ""
    echo "   TCANMS_PREFIX = '$TCANMS_PREFIX'"
    echo "   TCANMS_ENV    = '$TCANMS_ENV'"
    echo "   TCANMS_HOST   = '$TCANMS_HOST'"
    echo ""
    version
}

version()
{
    echo "$PNAME : Version $VERSION, by $AUTHOR"
    echo ""
}


# ------------------------------------------

create_subdirs()
{
    local subdir=

    for subdir in $PATHLIST; do
        if [ ! -d $subdir ]; then
            mkdir -p $subdir
            RETVAL=$?
        fi
        if [ $RETVAL -eq 1 ]; then
            break
        fi
        #chown $TCANMS_USER $subdir
        #chgrp $TCANMS_GROUP $subdir
        #chmod g+w $subdir
    done

    return $RETVAL
}

init_env_configs()
{
    local cfgenv=$1
    local cfghost=$2

    if [ -z "$cfgenv" ] || [ -z "$cfghost" ]; then
        echo "  Invalid environment settings"
        return 1
    fi

    local cfgpath="$CONFIGDIR/../environment/$cfgenv/$cfghost/"
    local target="$PREFIX/"

    echo "  Syncing configs from '$cfgenv/$cfghost'"

    if [ -d $cfgpath ]; then
        echo "      $RSYNC -r $cfgpath $target"
        $RSYNC -r $cfgpath $target
    else
        echo " Error, path not valid: $cfgpath"
        return 1
    fi

    return 0
}


# --------------------------
#  MAIN


while [ $# -gt 0 ]; do
    case "$1" in
        -e|--environment)
            ENVIR="$2"
            shift
            ;;
        -f|--force)
            FORCE="true"
            ;;
        -t|--target)
            HOST="$2"
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        -v|--version)
            version
            exit 0
            ;;

    esac
    shift
done


if [ -z "$PREFIX" ]; then
    echo "Error: TCANMS_PREFIX not set."
    usage
    exit 1
fi


if [ -d $PREFIX ]; then
    if [ -n "$FORCE" ]; then
        echo "  Install directory '$PREFIX' already exists."
        echo "     '--force' is enabled, continuing..."
    else
        echo "Error: Install directory '$PREFIX' exists."
        echo "Use the '--force' option to overwrite"
        exit 1
    fi
fi

echo "  Creating directory structure in '$PREFIX'"
create_subdirs


if [ -n "$ENVIR" ] && [ -n "$HOST" ]; then
    init_env_configs $ENVIR $HOST
else
    echo "Target environment and host not provided, configs NOT sync'd."
fi

if [ $RETVAL -eq 1 ]; then
    echo "$PNAME: finished with errors."
else
    echo ""
    echo "$PNAME: finished successfully."
fi
echo ""


exit $RETVAL

