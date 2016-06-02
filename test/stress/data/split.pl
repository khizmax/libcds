#!/bin/perl

my %words ;

my $input_file = shift;
my $output_file = shift;

open( my $f, $input_file ) or die "Cannot open input file $input_file";
binmode $f ;

my $text = ''	;
$text .= $_ while (<$f>) ;
close $f ;

my @a = split /[^\w'-]/, $text ;
foreach my $w (@a) {
	$words{$w} += 1 if $w ;
}
for (my $j = 1; $j < 30; ++$j ) {
	for ( my $i = 0; $i + $j - 1 < @a; $i += 1 ) {
		my $s = '';
		for ( my $k = 0; $k < $j; ++$k ) {
			$s .= ' '.$a[$i+$k];
		}
		$s =~ /\s*(\S.+\S)\s*/;
		$s = $1 ;
		$s =~ s/\s\s+/ /g ;
		$words{$s} += 1 ;
	}
}

open (my $dst, ">$output_file") or die "Cannot open output file $output_file";
binmode $dst ;

my $nCount = 0 ;
$nCount++ foreach (keys %words) ;
print $dst $nCount, "\n" ;

print "Generate test dictionary $output_file ...\n" ;
print $dst $_, "\n" foreach (keys %words)	;

close $dst  ;
