#!/bin/sh

usage ()
{
  echo "usage: $0 driver-path file-check-path source-file"
}

if [ $# != 3 ]
then
  usage
  exit
fi

$1 -only-lex $3 | $2 $3
