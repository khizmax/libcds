#!/usr/bin/perl

my $nTabsFound = 0;

brush()	;

print "Tabs found: $nTabsFound\n" ;
print "Done\n" ;

exit ;

sub brush() 
{
	processDir( "./cds" ) ;
	processDir( "./src" ) ;
	processDir( "./tests/test-hdr" ) ;
	processDir( "./tests/unit" ) ;
	processDir( "./tests/cppunit" ) ;
}

sub processDir( $ )
{
	my $dirName = shift ;
	print "Process $dirName directory...\n";
	
	opendir(my $dh, $dirName) || die "can't opendir $dirName: $!";
    my @files = grep { /^[^\.]/ } readdir($dh);
    closedir $dh;
	
	foreach my $file ( @files ) {
		if ( -d "$dirName/$file" ) {
			processDir("$dirName/$file")	;
		}
		elsif ( $file =~ /\.(h|cpp)$/ ) {
			processFile( "$dirName/$file" )	;
		}
	}
}

sub processFile( $ ) 
{
	my $file = shift	;

	
	if ( open( my $fh, $file )) {
		binmode $fh	;
		my $str = ''		;
		while (<$fh>) {
			$nTabsFound += $_ =~ s/\t/    /g;
			$_ =~ s/\s+$//  ;
			$_ =~ s/\s+;$/;/;
			$str .= $_      ;
			$str .= "\n"    ;
		}
		close $fh	;
		
		if ( open( my $fh, ">$file" )) {
			binmode $fh 	;
			print $fh $str 	;
			close $fh		;
		}
	}
}



