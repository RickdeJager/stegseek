#!/bin/bash

##################################################
# Quick and dirty test script for CI.
##################################################

# TODO
# Make stegseek throw proper exit codes

STEGSEEK=../../src/stegseek
DATADIR=data
PASSWORD="stegseek"

FAILMSG="Could not find a valid passphrase"
OKMSG="Found passphrase:"


echo "=== Stegseek tests ==="


# Remove any leftover files
rm -f $DATADIR/*.out
rm -f *.out


# Create bogus wordlist with length 1 million
printf 'Bogus\n%.0s' {1..1000000} > wordlist.tmp

# Try cracking without the correct password in the wordlist
SHOULD_FAIL=$($STEGSEEK --crack -sf $DATADIR/sun.jpg -wl wordlist.tmp)

if [[ $SHOULD_FAIL =~ $OKMSG ]]; then
   echo "Program claims to have found a password when it shouldn't have"
   rm wordlist.tmp
   exit 1
fi

echo " [1/2]"


# Add the password to the wordlist
echo $PASSWORD >> wordlist.tmp
# Add some more words
printf 'Bogus\n%.0s' {1..123456} >> wordlist.tmp

SHOULD_WORK=$($STEGSEEK --crack -sf $DATADIR/sun.jpg -wl wordlist.tmp)

if [[ $SHOULD_WORD =~ $FAILMSG ]]; then
   echo "Program could not crack the steg file even though the password was present."
   rm wordlist.tmp
   exit 1
fi

echo " [2/2]"
echo "All tests passed"
rm wordlist.tmp
