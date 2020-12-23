#!/bin/bash

##################################################
# Quick and dirty test script for CI.
##################################################

# TODO
#  * Make stegseek throw proper exit codes
#  * Move away from this God-forbidden excuse for a
#    test script

# Exit on first error
set -e

STEGSEEK=../../src/stegseek
DATADIR=data
PASSWORD="stegseek"

FAILMSG="Could not find a valid passphrase"
OKMSG="Found passphrase:"


echo "=== Stegseek tests ==="


# Remove any leftover files
rm -f $DATADIR/*.out
rm -f *.out


echo "Creating bogus wordlist with 1 million entries..."
# Create bogus wordlist with length 1 million
printf 'Bogus\n%.0s' {1..1000000} > wordlist.tmp

# Try cracking without the correct password in the wordlist
echo " [1/3] correct password not present..."
echo "       allowed to run for 2 seconds"
echo ""
SHOULD_FAIL=$(timeout 2s $STEGSEEK --crack -sf $DATADIR/sun.jpg -wl wordlist.tmp -xf -)
echo $SHOULD_FAIL

if [[ ! $SHOULD_FAIL =~ "$FAILMSG" ]]; then
   echo ""
   echo "Program claims to have found a password when it shouldn't have"
   rm wordlist.tmp
   exit 1
fi



echo "Adding correct password to the wordlist..."
# Add the password to the wordlist
echo $PASSWORD >> wordlist.tmp
# Add some more words
printf 'Bogus\n%.0s' {1..123456} >> wordlist.tmp

echo " [2/3] correct password present..."
echo "       allowed to run for 2 seconds"
echo ""
SHOULD_WORK=$(timeout 2s $STEGSEEK --crack -sf $DATADIR/sun.jpg -wl wordlist.tmp -xf -)
echo $SHOULD_WORK

if [[ ! "$SHOULD_WORK" =~ "$OKMSG" ]]; then
   echo ""
   echo "Program could not crack the steg file even though the password was present."
   rm wordlist.tmp
   exit 1
fi

#
# This next test is special, because it requires rng collisions to be detected in the selector
# Normally, steghide does some bookkeeping to correct when this happens. Stegseek only needs to
# detect this case, and restart when it occurs.
#
# Collisions are dependant on the number of samples / seed used.

echo " [3/3] correct password present (RNG collision)..."
echo "       allowed to run for 2 seconds"
# Add the password to the wordlist
echo "iluv1josh" >> wordlist.tmp
echo ""
SHOULD_WORK=$(timeout 2s $STEGSEEK --crack -sf $DATADIR/rngcollision.jpg -wl wordlist.tmp -xf -)
echo $SHOULD_WORK


if [[ ! $SHOULD_WORK =~ "$OKMSG" ]]; then
   echo ""
   echo "Program could not crack the steg file even though the password was present."
   rm wordlist.tmp
   exit 1
fi

echo "All tests passed"
rm wordlist.tmp
