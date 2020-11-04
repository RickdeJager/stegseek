#!/usr/bin/perl -w
use FileHandle ;

STDOUT->autoflush(1) ;

print "-------------------------\n\n" ;
print "Running System Tests.....\n\n" ;
print "-------------------------\n\n" ;

$exitvalue = 0 ; # 0 -> everything ok, 1 -> an error occured
while (<st*.pl>) {
	# pass command line args to st_*.pl scripts
	if (system "./$_ @ARGV") {
		$exitvalue = 1 ;
	}
}
exit $exitvalue ;
