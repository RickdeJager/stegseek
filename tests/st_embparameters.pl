#!/usr/bin/perl -w
use STSupport;

use constant EMPTY => {e => "none", Z => "", K => "", "-check" => ""} ;

sub runEncryption {
	startSubCategory("Encryption") ;
	$ref = EMPTY ;
	%args = %$ref ;
	$args{e} = "des ecb" ;
	addTestResult(runEmbExtCmp(DATADIR . "os21x8_std.bmp", DATADIR . RND100, \%args));
	$args{e} = "arcfour stream" ;
	addTestResult(runEmbExtCmp(DATADIR . "mulaw_std.au", DATADIR . RND100, \%args));
	$args{e} = "nofb gost" ;
	addTestResult(runEmbExtCmp(DATADIR . "pcm8_std.wav", DATADIR . RND100, \%args));
	$args{e} = "blowfish" ;
	addTestResult(runEmbExtCmp(DATADIR . "win3x24_std.bmp", DATADIR . RND100, \%args));
	$args{e} = "serpent ncfb" ;
	addTestResult(runEmbExtCmp(DATADIR . "pcm16_std.au", DATADIR . RND100, \%args));
	$args{e} = "ofb" ;
	addTestResult(runEmbExtCmp(DATADIR . "win3x4_std.bmp", DATADIR . RND100, \%args));
	endSubCategory() ;
}

sub runCompression {
	startSubCategory("Compression") ;
	$ref = EMPTY ;
	%args = %$ref ;
	delete $args{Z} ;
	addTestResult(runEmbExtCmp(DATADIR . "mulaw_std.au", DATADIR . RND100, \%args));
	$args{z} = "1" ;
	addTestResult(runEmbExtCmp(DATADIR . "pcm16_std.au", DATADIR . RND100, \%args));
	$args{z} = "7" ;
	addTestResult(runEmbExtCmp(DATADIR . "os21x1_std.bmp", DATADIR . RND100, \%args));
	endSubCategory() ;
}

sub runChecksum {
	startSubCategory("Checksum") ;
	$ref = EMPTY ;
	%args = %$ref ;
	delete $args{K} ;
	addTestResult(runEmbExtCmp(DATADIR . "mulaw_std.au", DATADIR . RND100, \%args));
	addTestResult(runEmbExtCmp(DATADIR . "pcm16_std.au", DATADIR . RND100, \%args));
	addTestResult(runEmbExtCmp(DATADIR . "os21x1_std.bmp", DATADIR . RND100, \%args));
	addTestResult(runEmbExtCmp(DATADIR . "win3x24_std.bmp", DATADIR . RND100, \%args));
	addTestResult(runEmbExtCmp(DATADIR . "pcm8_std.wav", DATADIR . RND100, \%args));
	endSubCategory() ;
}

startCategory("Embedding Parameters", @ARGV) ;
runEncryption() ;
runCompression() ;
runChecksum() ;
exit endCategory() ;
