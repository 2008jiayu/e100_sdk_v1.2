#!/usr/bin/perl

binmode STDOUT;
while (<>) {
	s@([a-zA-Z]):[\\/]@/cygdrive/$1/@g;	# d:\ => /cygdrive/d/
	s@\\([0-9_a-zA-Z\.])@/$1@g;			# \b => /b or \. =? /.
	s@\r\n@\n@g;				# dos to unix
	print "$_";				# print line to STDOUT
}
