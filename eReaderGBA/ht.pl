#!/usr/bin/perl

use Getopt::Std;

# Header tool v0.01

# (c) 02 buZz

#  This tool is ment for the reading , display and possible alteration of
#  the header located in all GBA roms.
#  Written in perl for cross-platformness. ;P

#  If you find any bugs or know a kewl feature , plz mail buzz@ddsw.nl

$default_logo = "\x24\xff\xae\x51\x69\x9a\xa2\x21\x3d\x84\x82\x0a\x84\xe4\x09\xad\x11\x24\x8b\x98\xc0\x81\x7f\x21\xa3\x52\xbe\x19\x93\x09\xce\x20\x10\x46\x4a\x4a\xf8\x27\x31\xec\x58\xc7\xe8\x33\x82\xe3\xce\xbf\x85\xf4\xdf\x94\xce\x4b\x09\xc1\x94\x56\x8a\xc0\x13\x72\xa7\xfc\x9f\x84\x4d\x73\xa3\xca\x9a\x61\x58\x97\xa3\x27\xfc\x03\x98\x76\x23\x1d\xc7\x61\x03\x04\xae\x56\xbf\x38\x84\x00\x40\xa7\x0e\xfd\xff\x52\xfe\x03\x6f\x95\x30\xf1\x97\xfb\xc0\x85\x60\xd6\x80\x25\xa9\x63\xbe\x03\x01\x4e\x38\xe2\xf9\xa2\x34\xff\xbb\x3e\x03\x44\x78\x00\x90\xcb\x88\x11\x3a\x94\x65\xc0\x7c\x63\x87\xf0\x3c\xaf\xd6\x25\xe4\x8b\x38\x0a\xac\x72\x21\xd4\xf8\x07";

sub read_header {

	my ($file) = @_;
	my $buffer = "";

	my %header;

	open(ROM, $file) or die "cannot open $file for read: $!";
	binmode(ROM);

	read(ROM, $buffer, 4);
	$header{'START'} = $buffer;
	$buffer="";

	read(ROM, $buffer, 156);
	$header{'LOGO'} = $buffer;
	$buffer="";

	read(ROM, $buffer, 12);
	$header{'TITLE'} = $buffer;
	$buffer="";

	read(ROM, $buffer, 4);
	$header{'GAMECODE'} = $buffer;
	$buffer="";

	read(ROM, $buffer, 2);
	$header{'MAKER'} = $buffer;
	$buffer="";

	seek(ROM,1,1);

	read(ROM, $buffer, 1);
	$header{'UNIT'} = $buffer;
	$buffer="";
	
	read(ROM, $buffer, 1);
	$header{'DEVICE'} = $buffer;
	$buffer="";
	
	seek(ROM,7,1);

	read(ROM, $buffer, 1);
	$header{'VERSION'} = $buffer;
	$buffer="";
	
	read(ROM, $buffer, 1);
	$header{'CHECK'} = $buffer;
	$buffer="";
	
	close(ROM);

	return %header;
}

sub write_header {

	my ($file,$href) = @_;
	my %header = %{$href};

	open(ROM, '+<', $file) or die "cannot open $file for write: $!";
	binmode(ROM);

	print ROM $header{'START'};
	print ROM $header{'LOGO'};
	print ROM $header{'TITLE'};
	print ROM $header{'GAMECODE'};
	print ROM $header{'MAKER'};
	print ROM "\x96";
	print ROM $header{'UNIT'};
	print ROM $header{'DEVICE'};
	print ROM "\x00\x00\x00\x00\x00\x00\x00";
	print ROM $header{'VERSION'};
	print ROM $header{'CHECK'};

	close(ROM);

}

sub copy_file {

	my ($infile,$outfile)=@_;

	open(IN,$infile) or die "cannot open input file: $!\n";
	open(OUT,'>',$outfile) or die "cannot open output file: $!\n";
	while(<IN>)
	{ 
	 print OUT $_;
	}
	close(IN);
	close(OUT);

}

sub calc_complement {

	my ($title,$gamecode,$maker,$unit,$device,$version) = @_;

	my $temp=0;

	foreach (split(//, $title)) {
		$temp=$temp+ord($_);
	}
	foreach (split(//, $gamecode)) {
		$temp=$temp+ord($_);
	}
	foreach (split(//, $maker)) {
		$temp=$temp+ord($_);
	}

	$temp=$temp+0x96;
	$temp=$temp+ord($unit);
	$temp=$temp+ord($device);
	$temp=$temp+ord($version);
	$temp=0 - ($temp+0x19);

	$temp = $temp % 256;

	return $temp;

}

if (!@ARGV) {
	print "Header Tool v0.01 ---- (c) 02 buZz\n\n";
	print " usage :\n\n";
	print " ht.pl [-cl] [-n \"ROMNAME\"] [-o FILENAME] <FILENAME>\n\n";
	print "  <FILENAME>      The .gba romfile you want to use\n";
	print "  [-o FILENAME]   Fix errors and save rom as FILENAME\n";
	print "  [-n \"ROMNAME\"]  Change name to ROMNAME (12 chars max)\n";
	print "  [-c]            Check complement check\n";
	print "  [-l]            Check logo\n\n";
	print " examples :\n\n";
	print "  ht.pl -cl thisrom.gba\n";
	print "   Checks logo and complement check in thisrom.gba and prints\n";
	print "   the found results\n\n";
	print "  ht.pl -cl -o newrom.gba thisrom.gba\n";
	print "   Checks logo and complement check in thisrom.gba and saves a fixed\n";
	print "   version as newrom.gba\n\n";
	print "  ht.pl -n \"My game\" -o newrom.gba thisrom.gba\n";
	print "   Changes the name in the header of thisrom.gba from the current\n";
	print "   name into My game and saves the result as the new rom newrom.gba\n\n";
	exit;
}

getopts('clo:n:');

my $inputfile = shift;
my %temp = read_header($inputfile);

print "rom name : $temp{'TITLE'}\n";

if (defined $opt_n) {

	if (length $opt_n > 12) {
		print "name $opt_n is too long!\n";
		exit;
	}

	while (length $opt_n < 12) {
		$opt_n = $opt_n."\x00";
	}

	print "new name : $opt_n\n";
	
	$temp{'TITLE'}=$opt_n;

}

if (defined $opt_l) {

	print "logo: ";
	if ($temp{"LOGO"} eq $default_logo) {
		print "ok!\n";
	} else {
		print "error";
		if (defined $opt_o) {
			$temp{"LOGO"} = $default_logo;
			print " , fixed!\n";
		} else {
			print "\n";
		}
	}
}

if (defined $opt_c) {

	$calced = calc_complement($temp{"TITLE"},$temp{"GAMECODE"},$temp{"MAKER"},$temp{"UNIT"},$temp{"DEVICE"},$temp{"VERSION"});
	print "checksum: ";
	if (chr($calced) eq $temp{"CHECK"}) {
		print "ok!\n";
	} else {
		print "error";
		if (defined $opt_o) {
			$temp{"CHECK"} = chr($calced);
			print " , fixed!\n";
		} else {
			print "\n";
		}
	}
}

if (defined $opt_o) {

	print "writing to $opt_o\n";
	copy_file($inputfile,$opt_o);
	write_header($opt_o,\%temp);

}
