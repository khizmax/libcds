

sub processDir( $ )
{
	my $dirName = shift ;
	
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
		my $bTabified = 0	;
		while (<$fh>) {
			my $s = $_	;
			if ( $str eq '' ) {
				unless ( $s =~ m#//\$\$CDS-header\$\$# ) {
					$str .= '//$$CDS-header$$'."\n\n"	;
					$bTabified = 1	;
				}
			}
			while ( $s =~ /\t/ ) {
				#print "pos=", $+[0], "\n"	;
				$bTabified = 1	;
				my $pos = $+[0]	;
				if ( $pos ) {
					$pos -= 1	;
					if ( $pos % 4 == 0 ) {
						$s =~ s/\t/    /	;
					}
					elsif ( $pos % 4 == 1 ) {
						$s =~ s/\t/   /	;
					}
					elsif ( $pos % 4 == 2 ) {
						$s =~ s/\t/  /	;
					}
					elsif ( $pos % 4 == 3 ) {
						$s =~ s/\t/ /	;
					}
				}
			}
			$bTabified = 1 if $s =~ s/[\t ]+([\n\r]+)$/$1/ ;
			$str .= $s ;
		}
		close $fh	;
		
		if ( $bTabified ) {
			print $file, "\n"	;
			if ( open( my $fh, ">$file" )) {
				binmode $fh 	;
				print $fh $str 	;
				close $fh		;
			}
		}
	}
}

chdir ('..') ;
processDir('cds')	;
processDir('src')	;
processDir('tests/unit')	;

exit;