#!/usr/bin/perl -w
use STSupport;

use constant EMPTY => {e => "none", Z => "", K => "", "-check" => ""} ;

sub runAu {
	startSubCategory("au") ;
	addTestResult(runEmbExtCmp(DATADIR . "mulaw_std.au", DATADIR . RND100, EMPTY));
	addTestResult(runEmbExtCmp(DATADIR . "pcm8_std.au", DATADIR . RND100, EMPTY));
	addTestResult(runEmbExtCmp(DATADIR . "pcm16_std.au", DATADIR . RND100, EMPTY));
	endSubCategory() ;
}

sub runBmp {
	startSubCategory("bmp") ;
	addTestResult(runEmbExtCmp(DATADIR . "os21x1_std.bmp", DATADIR . RND100, EMPTY));
	addTestResult(runEmbExtCmp(DATADIR . "os21x4_std.bmp", DATADIR . RND100, EMPTY));
	addTestResult(runEmbExtCmp(DATADIR . "os21x8_std.bmp", DATADIR . RND100, EMPTY));
	addTestResult(runEmbExtCmp(DATADIR . "os21x24_std.bmp", DATADIR . RND100, EMPTY));
	addTestResult(runEmbExtCmp(DATADIR . "win3x1_std.bmp", DATADIR . RND100, EMPTY));
	addTestResult(runEmbExtCmp(DATADIR . "win3x4_std.bmp", DATADIR . RND100, EMPTY));
	addTestResult(runEmbExtCmp(DATADIR . "win3x8_std.bmp", DATADIR . RND100, EMPTY));
	addTestResult(runEmbExtCmp(DATADIR . "win3x24_std.bmp", DATADIR . RND100, EMPTY));
	endSubCategory() ;
}

sub runJpeg {
	startSubCategory("jpeg") ;
	addTestResult(runEmbExtCmp(DATADIR . "std.jpg", DATADIR . RND100, EMPTY));
	addTestResult(runEmbExtCmp(DATADIR . "prog.jpg", DATADIR . RND100, EMPTY));
	endSubCategory() ;
}

sub runWav {
	startSubCategory("wav") ;
	addTestResult(runEmbExtCmp(DATADIR . "pcm8_std.wav", DATADIR . RND100, EMPTY));
	addTestResult(runEmbExtCmp(DATADIR . "pcm16_std.wav", DATADIR . RND100, EMPTY));
	endSubCategory() ;
}

startCategory("File Formats", @ARGV) ;
runAu() ;
runBmp() ;
runJpeg() ;
runWav() ;
exit endCategory() ;
