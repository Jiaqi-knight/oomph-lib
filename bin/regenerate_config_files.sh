#! /bin/sh

#-------------------------------------------------------------------
# Shell script to rerun autotools coomands to generate all configuration
# scripts so that the distribution is ready to run via ./configure make
# The root directory must be passed as the first argument
#-------------------------------------------------------------------

# Test that we do have (only one) command line argument
if (test $# -lt 1); then
 echo "Path to root directory must be the first argument\
 to regenerate_config_files.sh"
 exit 1
fi

OptionRead()
{
 read Opt
 if test "$Opt" = "" ; then
  Opt=$1
 fi
 echo $Opt
}


#Set the current working directory
working_dir=`pwd`
#Set the root directory
oomph_dir=$1

#Change to the root directory
cd $oomph_dir

#Add the private directory 
if [ -d private ]; then
 #Remove the old file
 if [ -e config/configure.ac_scripts/private.dir_list ]; then
  rm config/configure.ac_scripts/private.dir_list
 fi
 #Add the directory to the list
 echo private >> config/configure.ac_scripts/private.dir_list
 #Now change to the directory
 cd private
 #Obtain a list of all files in the directory
 filelist=`ls`
 #Only echo if the file is a directory
 for file in $filelist; do
  if [ -d $file -a $file != "external_src" ]; then
    echo private/$file >> ../config/configure.ac_scripts/private.dir_list
  fi
 done
 cd ..
fi


#Generate a list of private user drivers
if [ -d private/user_drivers ]; then
 #Remove the old file
 if [ -e config/configure.ac_scripts/private_user_drivers.dir_list ]; then
  rm config/configure.ac_scripts/private_user_drivers.dir_list
 fi
 #Add the directory to the list
 #echo private/user_drivers >> config/configure.ac_scripts/private_user_drivers.dir_list
 #Now change to the directory
 cd private/user_drivers
 #Obtain a list of all files in the directory
 filelist=`ls`
 #Only echo if the file is a directory
 for file in $filelist; do
  if [ -d $file ]; then
    echo private/user_drivers/$file >> ../../config/configure.ac_scripts/private_user_drivers.dir_list
  fi
 done
 cd ../..
fi


#Generate a list of private user srcs
if [ -d private/user_src ]; then
 #Remove the old file
 if [ -e config/configure.ac_scripts/private_user_src.dir_list ]; then
  rm config/configure.ac_scripts/private_user_src.dir_list
 fi
 #Add the directory to the list
 #echo private/user_src >> config/configure.ac_scripts/private_user_src.dir_list
 #Change to the directory
 cd private/user_src
 #Obtain a list of all files in the directory
 filelist=`ls`
 #Omit the Makefiles directories
 for file in $filelist; do
  #Only echo if it's a directory
  if [ -d $file ]; then
    echo private/user_src/$file >> ../../config/configure.ac_scripts/private_user_src.dir_list
  fi
 done
 cd ../..
fi


# Regenerate configure.ac so that missing Makefile.am s don't cause
# any problems [we create dummy directories and dummy Makefile.am s]
echo "Recreating configure.ac, based on the input in:"
echo " " 
echo "   config/configure.ac_scripts/"
echo " "
echo "#==================================================================" \
    > configure.ac
echo "#AUTOMATICALLY GENERATED FILE -- DO NOT EDIT!" \
    >> configure.ac
echo "# " \
    >> configure.ac
echo "#This file was generated by autogen.sh, based on the files in:" \
    >> configure.ac
echo "# " \
    >> configure.ac
echo "#   config/configure.ac_scripts/">> configure.ac
echo "# " \
      >> configure.ac
echo "#Add any new directories containing a Makefile.am to">> configure.ac
echo "#config/configure.ac_scripts/core.dir_list and re-run autogen.sh" \
    >> configure.ac
echo "# " \
    >> configure.ac
echo "#==================================================================" \
    >> configure.ac
echo "# " \
    >> configure.ac

cat config/configure.ac_scripts/start >> configure.ac

echo "AC_CONFIG_FILES([Makefile" \
    >> configure.ac

echo " "
DIRECTORY_LIST=`cat config/configure.ac_scripts/*.dir_list`
for directory in $DIRECTORY_LIST
  do
  if (test ! -e $directory ); then
      mkdir $directory
      echo "# Dummy Makefile.am" >> $directory/Makefile.am
      echo "Making dummy directory: " $directory
  fi
  echo "   " $directory/Makefile >> configure.ac
done

echo "])" \
    >> configure.ac

cat config/configure.ac_scripts/end >> configure.ac

# Automake compatability fix
# ============================================================

# If we have automake version more recent than 1.13 then the default is to
# use the new parallel self test harness which doesn't work with
# parallel_self_tests.py (and doesn't actually run tests in parallel
# without a major rewrite of all Makefile.am s). So we need to disable it.
# However the command to disable the new test harness was only introduced
# in version 1.12 which is still very new! So it looks like the only way
# around this for now is to modify configure.ac here if the automake
# version is greater than 1.12.

# First we need a way to compare versions:
version_less_than()
{

    # Ideally I would use this but the version of coreutils on the Wulfling
    # is so old that it doesn't have "--version-sort"...
    # # Use tr rather than newlines in echo because sh and bash treat \n
    # # differently and incompatibly!
    # [ ! $(echo "$1 $2" | tr ' ' '\n' | sort --version-sort | head -1) = "$2" ]

    # Extract individual parts of the version strings
    v11=$(echo $1 | cut -d'.' -f1)
    v12=$(echo $1 | cut -d'.' -f2)
    v13=$(echo $1 | cut -d'.' -f3)

    v21=$(echo $2 | cut -d'.' -f1)
    v22=$(echo $2 | cut -d'.' -f2)
    v23=$(echo $2 | cut -d'.' -f3)

    # echo $v11 "." $v12 "." $v13 1>&2
    # echo $v21 "." $v22 "." $v23 1>&2

    # Compare them all. Nested ifs because shell conditionals are HARD...
    result=1
    if [ $v11 -lt $v21 ]; then
        result=0
        echo "in 1" 1>&2
    elif [ $v11 -eq $v21 ]; then
        if [ $v12 -lt $v22 ]; then
            result=0
            echo "in 2" 1>&2
        elif [ $v12 -eq $v22 ]; then
            if [ $v13 -lt $v23 ]; then
                result=0
                echo "in 3" 1>&2
            fi
        fi
    fi

    echo $result
}

# Now we need to get the automake version (hopefully they don't change the
# formatting of the --version output! Don't think there's any other way to
# get the version).
automake_version=$(automake --version | head -1 | tr ' ' '\n' | tail -1)

if $(version_less_than 1.12.0 $automake_version) ; then
    echo "I'm modifying configure.ac to use serial self tests in automake because you have a recent enough version of automake, see bin/regenerate_config_files.sh for why/how."
    # Enforce serial tests and require automake version 1.12 or above (just
    # in case someone does something really weird..)
    sed 's/^[ \t]*AM_INIT_AUTOMAKE[ \t]*$/AM_INIT_AUTOMAKE([1.12 serial-tests])/' \
        -i configure.ac
fi



echo " "
echo "Done. "


# Touch (=create if they don't exist yet) files which gnu expects to exist

touch NEWS README AUTHORS ChangeLog


# Libtoolize the sources (redo it to be on the safe side) 

echo " "
echo "Running libtoolize"
libtoolize --force
echo "done"


# Run aclocal to do whatever it does...
 
echo " "
echo "Running aclocal"
echo " "
echo "====================================================================="
echo "NOTE (ISSUED BY OOMPH-LIB): Warnings along the lines of"
echo " "
echo "    \"warning: underquoted definition of AM_PATH_GLIB\""
echo " "
echo "during the execution of aclocal can [usually] be ignored."
echo " " 
echo "If there really is a problem, ./configure  will fail below, "
echo "and you'll find out then... "
echo "====================================================================="
echo " "  
aclocal
echo " "  
echo "done"


# Run autoheader to (re)create "config.h.in" based on info in "configure.ac"

echo " "
echo "Running autoheader"
echo " "
echo "====================================================================="
echo "NOTE (ISSUED BY OOMPH-LIB): Warnings along the lines of"
echo " "
echo "    \"warning: do not use m4_patsubst: use patsubst or m4_bpatsubst\""
echo " "
echo "during the execution of autoheader can apparently be ignored. See eg."
echo " " 
echo "           http://monetdb.cwi.nl/GetGoing/Setup/SQL/Unix/"
echo " " 
echo "If there really is a problem, ./configure  will fail below, "
echo "and you'll find out then... "
echo "====================================================================="
echo " "  
autoheader
echo " "  
echo "done"


# Run autoconf to generate "configure" script and "config.h" file

echo " "
echo "Running autoconf"
echo " "
echo "====================================================================="
echo "NOTE (ISSUED BY OOMPH-LIB): Warnings along the lines of"
echo " "
echo "   \"warning: do not use m4_patsubst: use patsubst or m4_bpatsubst\""
echo " "
echo "during the execution of autoconf can apparently be ignored. See e.g. "
echo " " 
echo "           http://monetdb.cwi.nl/GetGoing/Setup/SQL/Unix/"
echo " " 
echo "If there really is a problem, ./configure  will fail below, "
echo "and you'll find out then... "
echo "====================================================================="
echo " "  
autoconf
echo " " 
echo "done" 


# Before running automake, build the auxillary files required in 
# /src/meshes

echo " "
echo "Building Auxillary Files in /src/meshes"
./bin/build_mesh_makefile.sh .
echo "done"

# Run automake to convert all "Makefile.am"s into proper "Makefile"s

echo " "
echo "Running automake -a"
automake -a
echo "done" 
