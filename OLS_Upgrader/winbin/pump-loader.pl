#sample PERL script to program PUMP ROM 
# Ian Lesnet 2010
# http://dangerousprototypes.com
#
# This example is for educational use and it's totally free
#knowledge shouldn't be locked up, demo code shouldn't be licensed
#released into the Public Domain or CC0, your choice.

#NOTE:
# Developed with ActivePerl: http://www.activestate.com/activeperl/
#
# For windows: configure your COM port below
# you'll also need Win32::Serialport http://search.cpan.org/~bbirth/Win32-SerialPort-0.19/lib/Win32/SerialPort.pm
#
# For others: use the Device::SerialPort version and modify the serial port name
# you'll need Device::SerialPort  http://search.cpan.org/~cook/Device-SerialPort-1.002/SerialPort.pm

#
#	CONVERT CHAR STRING TO ASCII HEX (FOR JEDEC ID)
#
sub ascii_to_hex ($){
	## Convert each ASCII character to a two-digit hex number.
	(my $str = shift) =~ s/(.|\n)/sprintf("%02lx", ord $1)/eg;
	return $str;
}

#
#	GET COMMAND LINE OPTIONS
#
use Getopt::Long;
my $results = GetOptions(
			"e"=>\my $opt_e,
            "s"=>\my $opt_s,
            "i"=>\my $opt_i,
			"v"=>\my $opt_v,
			"b"=>\my $opt_b,
            "p=s"=>\my $opt_p,
            "w=s"=> \my $opt_w,
            "r=s"=> \my $opt_r,
            "l=i"=> \my $opt_l ,
			"x"=> \my $opt_x ,);

print "PUMP-Loader v0.1\n";
			
#
# HELP SCREEN
#
if( (!(defined $opt_r)) && 
	(!(defined $opt_w)) &&
	(!(defined $opt_i)) &&
	(!(defined $opt_e)) &&
	(!(defined $opt_x)) &&
	(!(defined $opt_v)) &&
	(!(defined $opt_b)) &&
	(!(defined $opt_s)) ){
	print "# PUMP ROM programmer,\n"; 
	print "# \n";
	print "# -p <port> serial port to use (COM5 default)\n"; 
	print "# -w <file> BIN file to write\n";
	print "# -r <file> read ROM to file\n"; 
	print "# -e erase ROM\n";
	print "# -s ROM status byte\n";
	print "# -i ROM JEDEC ID\n";
	print "# -x Reset to run mode when complete\n";	
	print "# -l <pages> limit write or read to first <pages>\n";
	print "# -v Get firmware version string\n";		
	print "# -b Jump to bootloader\n";	
	print "# use: pump-loader.pl -p COM2 -w pump.bin\n\n";
	print "Nothing to do.";
	exit;
}

#
# Setup default serial port and page limits
#
my $serport;
if (defined ($opt_p)){
	$serport=$opt_p;
}else{
	$serport="COM5";
}
print "Using: $serport\n";

my $pagelimit;
if(defined ($opt_l)){
	$pagelimit=$opt_l;
}else{
	$pagelimit=2048;
}
print "Read/write: $pagelimit pages\n";

#
# Set up the serial port for Windows or Linux
#
my $port;
if ( $^O =~ m/Win32/ ){
	#setup serial port for Windows
	require Win32::SerialPort;
	$port = Win32::SerialPort->new($serport); #change to your com port
}else{
	#setup serial port for Linux
	require Device::SerialPort;
	$port = Device::SerialPort->new($serport); #change to your com port
}

#
#	Port configuration  115200/8/N/1
#
$port->{"_L_BAUD"}{921600} = 921600;
$port->databits(8);
$port->baudrate(921600); #115200
$port->parity("none");
$port->stopbits(1);
$port->buffers(1, 1); #1 byte or it buffers everything forever
$port->write_settings		|| undef $port; #set
unless ($port)			{ die "couldn't configure serial port."; }
#
# Protocol:
# 00 00 00 00 - get PUMP hardware/firmware/bootloader string (return 7bytes)
# 01 00 00 00 - get 4 byte JDEC ID (returns 4 bytes)
# 02 XX YY 00 + 264 data bytes + CRC - XX=page (upper 4 bits XXXXA987) YY=page (lower 7 bits = 6543210X), CRC = 2s compliment, (returns 1 when done, 0 for CRC error)
# 03 XX YY 00 - XX=page (upper 4 bits XXXXA987) YY=page (lower 7 bits = 6543210X), (returns 264 bytes, 1 page)
# 04 00 00 00 - erase chip (returns 1 on completion, takes up to 6 seconds)
# 05 00 00 00 - get status byte (returns 1 byte)
# $ $ $ $ - jump to bootloader

#
#	Grab hardware/firmware/bootloader version string
#
print "Reading firmware version:\n";
$port->write("\x00\x00\x00\x00"); #send get id command
select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive #sleep(1);
my $H=$port->read(1);
my $Hnum=ord($port->read(1));
my $F=$port->read(1);
my $Fmaj=ord($port->read(1));
my $Fmin=ord($port->read(1));
my $B=$port->read(1);
my $Bnum=ord($port->read(1));
print "Hardware: $Hnum, Firmware: $Fmaj.$Fmin, Bootloader: $Bnum\n";

#
#	Jump to bootloader
#
if(defined $opt_b){
	print "Jumping to bootloader!\n";
	$port->write("\x24\x24\x24\x24"); 
	select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive #sleep(1);
	$port->close || warn "close failed";
	undef $port;
	exit;
}

#
#	Grab ROM JEDEC chip ID
#
print "Reading JEDEC ID: 0x";
$port->write("\x01\x00\x00\x00"); #send get id command
select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive #sleep(1);
my $JEDECid=$port->read(4);
print ascii_to_hex $JEDECid;
print "\n";

if($JEDECid eq "\x1f\x24\x00\x00"){
	print "Found ATMEL AT45DB041D\n";
}else{
	die "Incorrect flash, or flash not found!\n";
}

#
#	Get status byte
#
if(defined $opt_s){
	print "Read status byte: ";
	$port->write("\x05\x00\x00\x00"); #read status byte command

	select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive #sleep(1);
	printf "0x%02X ", ord($port->read(1));
	print "\ndone.\n";
}

#
#	Erase entire chip
#
if(defined $opt_e){
	print "Erasing ROM chip (may take 12 seconds): \n";
	$port->write("\x04\x00\x00\x00");
	
	my $eraseRetry=0;
	while(1){
		select(undef,undef,undef, .2); #sleep for 12seconds while the chip erases
		my($count_in, $string_in);
		($count_in, $string_in)=$port->read(1);
		$eraseRetry++;
		#print "loop.\n"; #debug
		if( ($count_in==0)){
			if($eraseRetry==200){
				die "Erase timed out!\n";
			}
			next; 
		}
		
		if($string_in eq "\x01"){
			print "done.\n";
		}else{
			die "Erase failed!\n";
		}
		last;
	}

}

#
#	Write a page of data
#
my $pagecnt=0;
if(defined $opt_w){
	print "Write page: \n";
	
	#open the file
	open FILE, $opt_w or die $!; 
	binmode FILE; 
	
	#loop through the page data write 264 bytes at a time
	my ($buf, $data, $n, $i); 
	while (($n = read FILE, $data, 264) != 0) { 
	
		if($pagecnt> ($pagelimit-1) ){ #skip data after the page limit
			next;
		}
		
		print "$pagecnt\n"; 	
		
		#send page write command and page address
		$port->write("\x02");#address is 4 don't care+upper 4, 7lsb + 1 don't care, 0
		my $pagecntbits=(($pagecnt>>7)&0xff);#PAGE to write (upper bits of page number)
		$port->write(chr $pagecntbits);
		$pagecntbits=(($pagecnt<<1)&0xff); #lower 7 bits of page number
		$port->write(chr $pagecntbits);
		$port->write("\x00");
		$port->write($data); 

		#pad short pages with 00s
		$n=264-$n;
		for($i=0; $i<$n; $i++){
			$port->write("\x00"); 	
		}

		#calculate and send the CRC
		my $crc = unpack "%8C*", $data; 
		$crc = (0x100 - $crc) % 0x100; 
		$port->write(chr $crc);

		select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive #sleep(1);
		if($port->read(1) ne "\x01"){
			die "Write failed!\n";
		}
		
		#increment the page
		$pagecnt++;
	} 

	#cleanup
	close(FILE); 
	print "done.\n";
	
}

#
#	Read a page of memory (264 bytes)
# 	
if(defined $opt_r){

	print "Read page: \n";
	
	#setup read page limit
	my $readpages=$pagelimit;
	
	#open file for output
	open(OUTPUT, ">", $opt_r) or die $!;
	binmode OUTPUT; 
	
	#read data until the end (or limit)
	for (my $i=0; $i<$readpages; $i++){
		
		#display read page number
		print "$i\n"; 
	
		#send read page command and read address
		$port->write("\x03");#read page command
		my $pagecntbits=(($i>>7)&0xff); #PAGE to start reading (upper bits)
		$port->write(chr $pagecntbits);
		$pagecntbits=(($i<<1)&0xff); #lower 7 bits of page #
		$port->write(chr $pagecntbits);
		$port->write("\x00");
		$port->write($data); 
		
		#get data and save to output file
		select(undef,undef,undef, .02); #sleep for fraction of second for data to arrive #sleep(1);
		my $readflash=$port->read(264);
		print OUTPUT $readflash; 
		
	}
	#success
	print "done.\n";
	
}

if(defined $opt_x){
	print "Reset PUMP to run mode.\n";
	$port->write("\xFF");
	$port->write("\xFF");
	$port->write("\xFF");
	$port->write("\xFF");
}

$port->close || warn "close failed";
undef $port;
