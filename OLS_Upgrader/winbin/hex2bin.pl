#sample PERL script to convert intel HEX to byte format
# Ian Lesnet 2010
# http://dangerousprototypes.com
#
# This example is for educational use and it's totally free
#knowledge shouldn't be locked up, demo code shouldn't be licensed
#released into the Public Domain or CC0, your choice.
#use strict;

#
#	Get command line options
#
use Getopt::Long;
my $results = GetOptions(
            "w=s"=> \my $opt_w,
            "r=s"=> \my $opt_r );

if( (!defined ($opt_r) ) || (!defined ($opt_w)) ){
	print "# Stupid HEX2BIN converter,\n"; 
	print "# \n";
	print "# -r <file> HEX file to read\n";
	print "# -w <file> BIN file to write\n#\n"; 
	print "# use: hex2bin.pl -r text.hex -w test.bin\n\n";
	print "No file specified.";
}

#
#	Setup files
#
open FILE, "<", $opt_r or die $!;
open(OUTPUT, ">", $opt_w) or die $!;
binmode OUTPUT; 

#
#	Process each line
#
while (my $line = <FILE>) { 
	chomp($line);#delect line break
	my @chars=split(//,$line);#split into an array of characters

	#check for start char
	if($chars[0] ne ":"){
		die "Error in file format!";
	}
	#only read record type 00
	if( ($chars[7] ne "0") || ($chars[8] ne "0") ) {
		next; #skip other record types
	}
	
	#get byte count
	my $bytecount=(hex($chars[1]) *16) + hex($chars[2]);

	#ignore hex address, assume continous

	#get bytecount bytes, convert to binary and save to file
	for(my $i=0; $i<($bytecount*2); $i+=2){
		my $bv=(hex($chars[9+$i]) *16) + hex($chars[10+$i]);#convert hex ASCII characters to byte
		print OUTPUT chr($bv); #output to the file
		#print "$bv"; #debug, print to screen
	}
	#print "\n"; #debug output to screen
}
