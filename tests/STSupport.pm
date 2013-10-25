package STSupport;
require Exporter;
use File::Compare;

@ISA = qw(Exporter);
@EXPORT = qw(
	DATADIR RND100
	getArgString
	startCategory endCategory startSubCategory endSubCategory addTestResult
	runEmbExtCmp
);

#
# set this to 1 if you want the test script to print verbose output
#
$Arg_Verbosity = 0 ;

$curcategory = "";
$catstatus = 0 ;
$cursubcategory = "";
$subcatstatus = 0 ;

use constant DATADIR => "./data/" ;
use constant RND100 => "rnd100" ;

use constant TAB => "    " ;
use constant STEGHIDE => "../src/steghide" ;
use constant DEFAULTSTGFN => "stgfile" ;
use constant DEFAULTEXTFN => "extfile" ;
use constant DEFEMBARGBASE => (command => "embed", f => "", "q" => "", p => "\"a passphrase\"", sf => DEFAULTSTGFN) ;
use constant DEFEXTARGBASE => (command => "extract", f => "", "q" => "", p => "\"a passphrase\"", sf => DEFAULTSTGFN, xf => DEFAULTEXTFN) ;

#
# parse arguments for a st_*.pl script
#
sub parseArgs {
	while (@_) {
		$ARGV = shift @_ ;
		if ($ARGV eq "-v" || $ARGV eq "--verbose") {
			$Arg_Verbosity = 1 ;
		}
	}
}

#
# create a string of arguments ready to be fed to steghide
# expects a hash containing a "command" => "something" and argument/argument value pairs
# 
sub getArgString {
	my %args = @_ ;

	$retval = $args{command} ;
	delete $args{command} ;

	foreach $arg (keys %args) {
		$retval .= " -$arg $args{$arg}" ;
	}

	return $retval ;
}

#
# start a category
# first argument: category name, rest of list: arguments for execution of this category script
#
sub startCategory {
	$curcategory = shift @_ ;
	parseArgs (@_) ;
	print "$curcategory:\n" ;
	$catstatus = 1 ;
}

#
#
# return value: 1 if an error occured, 0 if everything was ok
sub endCategory {
	$curcategory = "" ;
	print "\n" ;
	if ($catstatus) { # no error
		return 0 ;
	}
	else { # an error occured
		return 1 ;
	}
}

sub startSubCategory {
	$cursubcategory = $_[0] ;
	print TAB . $cursubcategory ;
	$subcatstatus = 1 ;
}

sub endSubCategory {
	$cursubcategory = "" ;
	if ($subcatstatus) {
		print "ok\n" ;
	}
	else {
		print " !! FAILED !!\n" ;
	}
}

sub addTestResult {
	if ($_[0]) {
		print ".";
	}
	else {
		print "F";
		$subcatstatus = 0 ;
		$catstatus = 0 ;
	}
}

#
# remove all files in the list passed to it
sub remove {
	while (@_) {
		$f = shift @_ ;
		system "rm -f $f" ;
	}
}

#
# run an embed-extract-compare test
# expects 1-4 arguments: cvrfn, embfn, callerembargs, callerextargs
sub runEmbExtCmp {
	if ($Arg_Verbosity) {
		print "\n\nrunning EmbExtCmp with...\n" ;
	}

	# evaluate arguments
	$cvrfn = shift @_ ;
	$embfn = DATADIR . RND100 ;	# default
	if (@_) { $embfn = shift @_ ; }
	%callerembargs = () ;
	if (@_) {
		$ref = shift @_ ;
		%callerembargs = %$ref ;
	}
	%callerextargs = () ;
	if (@_) {
		$ref = shift @_ ;
		%callerextargs = %$ref ;
	}

	# prepare embedding
	%embargs = (
		DEFEMBARGBASE,
		cf => $cvrfn,
		ef => $embfn,
		%callerembargs
	) ;
	$embcommand = STEGHIDE . " " . getArgString(%embargs) ;

	# prepare extracting
	%extargs = (
		DEFEXTARGBASE,
		%callerextargs
	) ;
	$extcommand = STEGHIDE . " " . getArgString(%extargs) ;

	if ($Arg_Verbosity) {
		print "embcommand: $embcommand\n" ;
		print "extcommand: $extcommand\n" ;
	}

	# execute commands
	`$embcommand 2>&1` ; # don't let it print to stderr or stdout
	return 0 if $? ;
	`$extcommand 2>&1` ; # don't let it print to stderr or stdout
	return 0 if $? ;

	# compare embfile and extfile
	$filesequal = not compare ($embargs{ef}, $extargs{xf}) ;
	remove ($embargs{sf}, $extargs{xf}) ;

	return $filesequal ;
}

1;
