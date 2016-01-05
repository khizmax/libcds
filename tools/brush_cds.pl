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
	processDir( "../tests/test-hdr" ) ;
	processDir( "../tests/unit" ) ;
	processDir( "../tests/cppunit" ) ;
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
		binmode $fh	;
		my $str = '';
		while (<$fh>) {
            if ( /^\/\/\$\$CDS-header\$\$/ ) {
                $str .= 
"/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev\@gmail.com) 2006-$year

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.     
*/\n" ;
            }
            else {
                $nTabsFound += $_ =~ s/\t/    /g;
                $_ =~ s/\s+$//  ;
                $_ =~ s/\s+;$/;/;
                $str .= $_      ;
                $str .= "\n"    ;
            }
		}
		close $fh;
		
		if ( open( my $fh, ">$file" )) {
			binmode $fh;
			print $fh $str;
			close $fh;
		}
	}
}



