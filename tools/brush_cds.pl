#!/usr/bin/perl

my $nTabsFound = 0;

my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
$year += 1900;

brush();

print "Tabs found: $nTabsFound\n" ;
print "Done\n" ;

exit ;

sub brush() 
{
	processDir( "../cds" ) ;
	processDir( "../src" ) ;
	processDir( "../test" ) ;
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
			processDir("$dirName/$file");
		}
		elsif ( $file =~ /\.(h|cpp)$/ ) {
			processFile( "$dirName/$file" );
		}
	}
}

sub processFile( $ ) 
{
	my $file = shift;

	if ( open( my $fh, $file )) {
		#binmode $fh	;
		my $str = '';
		while (<$fh>) {
            $nTabsFound += $_ =~ s/\t/    /g;
            $_ =~ s/\s+$//;
            $_ =~ s/\s+;$/;/;
            $_ =~ s/\)\s+\)/\)\)/g;
            $str .= $_      ;
            $str .= "\n"    ;
		}
		close $fh;
		
		if ( open( my $fh, ">$file" )) {
			#binmode $fh;
			print $fh $str;
			close $fh;
		}
	}
}



