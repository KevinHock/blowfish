Made a file utility that encrypts with blowfish in cfb mode. It runs on CentOS and FreeBSD.

The "cipher" program has the following usage:

	cipher [-disevh] [-p PASSWD] infile outfile

The program will prompt the user to enter a password (or pass-phrase), then read infile, and then produce outfile.
If the -e option is given, the program should encrypt infile onto outfile, using the supplied user
password.  
If the -i option is given you be asked to supply an IV. This is a good idea. 
If the -i option is given you be asked to supply your password twice. This is a dumb idea security wise but was a point of extra credit.
If the -d option is given, the reverse should happen: decrypt infile onto outfile, using the supplied password.
Either the -d or -e options (but not both) must be supplied (an exactly once). Of course, if
you use the same password to encrypt and then decrypt a file, you should get
back the same exact file data you started with.
