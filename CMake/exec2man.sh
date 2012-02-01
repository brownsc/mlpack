#!/bin/bash
# Convert the output of an MLPACK executable into a man page.  This assumes that
# the CLI subsystem is used to output help, that the executable is properly
# documented, and that the program is run in the directory that the executable
# is in.  Usually, this is used by CMake on Linux/UNIX systems to generate the
# man pages.
#
# Usage:
#  exec2man.sh executable_name output_file_name
#
# No warranties...
#
# @author Ryan Curtin
name=$1
output=$2

# Generate the synopsis.
# First, required options.
reqoptions=`./$name -h | \
  awk '/Required options:/,/Options:/' | \
  grep '^  --' | \
  sed 's/^  --/--/' | \
  sed 's/^--[A-Za-z0-9_-]* (\(-[A-Za-z0-9]\))/\1/' | \
  sed 's/\(^-[A-Za-z0-9]\) [^\[].*/\1/' | \
  sed 's/\(^-[A-Za-z0-9] \[[A-Za-z0-9]*\]\) .*/\1/' | \
  sed 's/\(^--[A-Za-z0-9_-]*\) [^[].*/\1/' | \
  sed 's/\(^--[A-Za-z0-9_-]* \[[A-Za-z0-9]*\]\) [^[].*/\1/' | \
  tr '\n' ' ' | \
  sed 's/\[//g' | \
  sed 's/\]//g'`

# Then, regular options.
options=`./$name -h | \
  awk '/Options:/,0' | \
  grep '^  --' | \
  sed 's/^  --/--/' | \
  grep -v -- '--help' | \
  grep -v -- '--info' | \
  grep -v -- '--verbose' | \
  sed 's/^--[A-Za-z0-9_-]* (\(-[A-Za-z0-9]\))/\1/' | \
  sed 's/\(^-[A-Za-z0-9]\) [^\[].*/\1/' | \
  sed 's/\(^-[A-Za-z0-9] \[[A-Za-z0-9]*\]\) .*/\1/' | \
  sed 's/\(^--[A-Za-z0-9_-]*\) [^[].*/\1/' | \
  sed 's/\(^--[A-Za-z0-9_-]* \[[A-Za-z0-9]*\]\) [^[].*/\1/' | \
  tr '\n' ' ' | \
  sed 's/\[//g' | \
  sed 's/\]//g' | \
  sed 's/\(-[A-Za-z0-9]\)\( [^a-z]\)/\[\1\]\2/g' | \
  sed 's/\(--[A-Za-z0-9_-]*\)\( [^a-z]\)/\[\1\]\2/g' | \
  sed 's/\(-[A-Za-z0-9] [a-z]*\) /\[\1\] /g' | \
  sed 's/\(--[A-Za-z0-9_-]* [a-z]*\) /\[\1\] /g'`

synopsis="$name [-h] [-v] $reqoptions $options";

# Preview the whole thing first.
./$name -h | \
  awk -v syn="$synopsis" \
      '{ if (NR == 1) print "NAME\n '$name' - "tolower($0)"\nSYNOPSIS\n "syn" \nDESCRIPTION\n" ; else print } ' | \
  sed '/^[^ ]/ y/qwertyuiopasdfghjklzxcvbnm:/QWERTYUIOPASDFGHJKLZXCVBNM /' | \
  txt2man -T -P mlpack -t $name -d 1

# Now do it.
./$name -h | \
  awk -v syn="$synopsis" \
      '{ if (NR == 1) print "NAME\n '$name' - "tolower($0)"\nSYNOPSIS\n "syn" \nDESCRIPTION\n" ; else print } ' | \
  sed '/^[^ ]/ y/qwertyuiopasdfghjklzxcvbnm:/QWERTYUIOPASDFGHJKLZXCVBNM /' | \
  txt2man -P mlpack -t $name -d 1 > $output
