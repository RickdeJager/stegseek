Introduction :
==============

Steghide is a steganography program that is able to hide data in various kinds
of image- and audio-files. The color- respectivly sample-frequencies are not
changed thus making the embedding resistant against first-order statistical
tests.

The current version of steghide is 0.5.1

Features:
*) compression of embedded data
*) encryption of embedded data
*) embedding of a checksum to verify the integrity of the extraced data
*) support for JPEG, BMP, WAV and AU files

Steganography :
===============

Steganography literally means covered writing. Its goal is to hide the fact
that communication is taking place. This is often achieved by using a (rather
large) cover file and embedding the (rather short) secret message into this
file. The result is a innocuous looking file (the stego file) that contains
the secret message.

Compilation and Installation :
==============================

Dependencies :
--------------
You should have the following libraries installed to use steghide.

* libmhash
  A library that provides various hash algorithms and cryptographic key
  generation algorithms. Steghide needs this library to convert a passphrase
  into a form that can be used as input for cryptographic and steganographic
  algorithms.
  Available at: http://mhash.sourceforge.net/

* libmcrypt  
  A library that provides a lot of symmetric encryption algorithms. If you
  compile steghide without libmcrypt you will not be able to use steghide to
  encrypt data before embedding nor to extract encrypted data (even if you know
  the correct passphrase).
  Available at: http://mcrypt.sourceforge.net/

* libjpeg
  A library implementing jpeg image compression. Without this library you will
  not be able to embed data in jpeg files nor to extract data from jpeg files.
  Available at: http://www.ijg.org/

* zlib
  A lossless data compression library. If you compile steghide without having
  this library installed you will not be able to use steghide to compress data
  before embedding nor to extract compressed data from a stego-file.
  Available at: http://www.gzip.org/zlib/

Libmhash is absolutely required to compile steghide. While you can compile it
without the other libraries they are highly recommended as major functionality
will not be available without them.

Linux / Unix :
--------------
After unpacking the source distribution, enter the following commands:

1) ./configure 
2) make
3) make check
4) make install (as root)

For more information see the generic installation instructions in the file
INSTALL that came with the distribution.

If any of these commands fails, please send a mail to the steghide development
mailing list (steghide-devel@lists.sourceforge.net) describing the error.
 
Windows :
---------
The easiest way is to download the precompiled binary (including Windows
versions of the necessary libraries) from the steghide website at:
http://steghide.sourceforge.net/index.php

If you want to compile the sources yourself you need a C++ compiler. How you
need to compile the source code depends on the compiler you are using: Please
consult your compiler's documentation.

Steghide can be compiled with gcc in the cygwin environment
(http://www.cygwin.com/) which is a unix emulation layer for Windows using the
procedure mentioned above for the Linux/Unix compilation.

Quick-Start :
=============

Here are some examples of how steghide can be used. Take a look at these to get
a first impression. If you want more detailed information please read the
manpage.

The basic usage is as follows:

  $ steghide embed -cf picture.jpg -ef secret.txt
  Enter passphrase:
  Re-Enter passphrase:
  embedding "secret.txt" in "picture.jpg"... done

This command will embed the file secret.txt in the cover file picture.jpg.

After you have embedded your secret data as shown above you can send the file
picture.jpg to the person who should receive the secret message. The receiver
has to use steghide in the following way:

  $ steghide extract -sf picture.jpg
  Enter passphrase:
  wrote extracted data to "secret.txt".

If the supplied passphrase is correct, the contents of the original file
secret.txt will be extracted from the stego file picture.jpg and saved
in the current directory.

If you have received a file that contains embedded data and you want to get
some information about it before extracting it, use the info command:

  $ steghide info received_file.wav
  "received_file.wav":
    format: wave audio, PCM encoding
    capacity: 3.5 KB
  Try to get information about embedded data ? (y/n) y
  Enter passphrase:
    embedded file "secret.txt":
      size: 1.6 KB
      encrypted: rijndael-128, cbc
      compressed: yes

After printing some general information about the stego file (format, capacity) you will be
asked if steghide should try to get information about the embedded data. If you answer with
yes you have to supply a passphrase. Steghide will then try to extract the embedded data
with that passphrase and - if it succeeds - print some information about it.

Contact :
=========

Website :
---------
You can get the latest version of steghide as well as some additional
information and documentation from the steghide website at:
http://steghide.sourceforge.net/

Mailing Lists :
---------------
If you have found a bug or if you have questions, comments, suggestions, etc.
please send a mail to the development mailing list:
steghide-devel@lists.sourceforge.net
To receive mails sent to this list, subscribe to it at:
http://lists.sourceforge.net/lists/listinfo/steghide-devel

If you want to be informed, when a new version of steghide is released please
subscribe to the steghide announcement mailing list at:
http://lists.sourceforge.net/lists/listinfo/steghide-announce

Anonymous CVS access :
----------------------

You can access the most recent development source code via anonymous cvs. Just
type the following lines:

$ cvs -d:pserver:anonymous@cvs.steghide.sourceforge.net:/cvsroot/steghide login
CVS password:  [ Hit RETURN here ]

$ cvs -z3 -d:pserver:anonymous@cvs.steghide.sourceforge.net:/cvsroot/steghide co steghide

You can also browse the cvs repository on the web:
http://cvs.sourceforge.net/cgi-bin/viewcvs.cgi/steghide/

Author :
--------
You can contact me (Stefan Hetzl) via e-mail: shetzl@chello.at
