#!/usr/bin/perl

my $version ;

my $svnRelPath  ;
getRelPath()    ;
print "Relative path: $svnRelPath\n"    ;
my $upToRoot ;
if ( $svnRelPath =~ /trunk/ ) {
    $upToRoot = '..' ;
}
else {
    $upToRoot = '..\\..' ;
}
# my $upToRoot = $svnRelPath      ;
# $upToRoot =~ s/([^\/]+)/../g    ;
# $upToRoot = "../$upToRoot"      ;
print "upToRoot: $upToRoot\n"    ;

getVersion()	;
print "Version $version\n"	;

my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
$year += 1900	;

my $distrPath = "cds-distrib/cds-$version" ;
my $relDistrPath = "$upToRoot/$distrPath"   ;

print "export SVN...\n" ;
exportSVN()	;

my $nTabsFound = 0 ;

print "make copyright...\n" ;
makeCopyright()	;
patchFile("$relDistrPath/build/Makefile", 'VERSION=\d+\.\d+\.\d+', "VERSION=$version" ) ;
patchFile("$relDistrPath/doxygen/cds.doxy", 'PROJECT_NUMBER\s*=\s*\d+\.\d+\.\d+', "PROJECT_NUMBER = $version" ) ;
patchFile("$relDistrPath/projects/Win/vc2008/cds.2008.vcproj", 'CDS_USE_VLD', 'xCDS_USE_VLD' ) ;
patchFile("$relDistrPath/projects/Win/vc2008/unit.2008.vcproj", 'CDS_USE_VLD', 'xCDS_USE_VLD' ) ;
patchFile("$relDistrPath/projects/android/jni/Android.mk", 'CDS_LIBRARY_VERSION\s*:=\s*\d+\.\d+\.\d+', 
   "CDS_LIBRARY_VERSION := $version" );

print "Tabs found: $nTabsFound\n" ;

print "make docs...\n" ;
`cd $relDistrPath && make_docs.bat` ;
`rm -f $relDistrPath/make_distrib.pl`	;
`rm -fr $relDistrPath/scripts`	;
`rm -f $relDistrPath/doxygen/doxygen.log`	;

print "make zip...\n" ;
`rm -f $relDistrPath/../cds-$version.zip` ;
`cd $relDistrPath/.. && 7za a cds-$version.zip cds-$version` ;

print "Done\n" ;

exit ;

sub getVersion()
{
	open( my $fh, 'cds/version.h' ) or die "ERROR: Cannot find cds/version.h file";
	binmode $fh ;
	
	while (<$fh>) {
		if ( /CDS_VERSION_STRING.+(\d+\.\d+\.\d+)/ ) {
			$version = $1 ;
			last	;
		}
	}
	close $fh ;
	die "ERROR: cannot find version in cds/version.h" unless $version ;
}

sub getRelPath()
{
    my $svn = `svn info` ;
    ($svnRelPath) = $svn =~ /Working\sCopy\sRoot\sPath:\s+(.+)/ ;

    # my ($curDir) = $svn =~ /URL:\s+(.+)/ ;
    # my ($rootDir) = $svn =~ /Repository\s+Root:\s+(.+)/ ;
    # $rootDir =~ s/\+/\\\+/g ;
    
    # ($svnRelPath) = $curDir =~ /$rootDir\/(.+)/ ;
}

sub exportSVN()
{
	`cd $upToRoot && rm -fr $distrPath && svn export $svnRelPath $distrPath` ;
}

sub makeCopyright() 
{
	processDir( "$relDistrPath/cds" ) ;
	processDir( "$relDistrPath/src" ) ;
	processDir( "$relDistrPath/tests/test-hdr" ) ;
	processDir( "$relDistrPath/tests/unit" ) ;
	processDir( "$relDistrPath/tests/cppunit" ) ;
}

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
		while (<$fh>) {
			if ( /^\/\/\$\$CDS-header\$\$/ ) {
				$str .= 
"/*
    This file is a part of libcds - Concurrent Data Structures library
    See http://libcds.sourceforge.net/

    (C) Copyright Maxim Khiszinsky (libcds.sf\@gmail.com) 2006-$year
    Distributed under the BSD license (see accompanying file license.txt)

    Version $version
*/\n\n" ;
			}
			elsif ( /^\/\/\$\$CDS-\/\*\$\$/ ) {
				$str .= "/*\n" ;
			}
			elsif ( /^\/\/\$\$CDS-\*\/\$\$/ ) {
				$str .= "*/\n" ;
			}
			else {
                $nTabsFound += $_ =~ s/\t/    /g;
                $_ =~ s/\s+$//  ;
				$_ =~ s/\s+;$/;/;
				$str .= $_      ;
                $str .= "\n"    ;
			}
		}
		close $fh	;
		
		if ( open( my $fh, ">$file" )) {
			binmode $fh 	;
			print $fh $str 	;
			close $fh		;
		}
	}
}

sub patchFile(@) {
	my $file = shift	;
	my $seek = shift	;
	my $repl = shift	;
	
	if ( open( my $fh, $file )) {
		binmode $fh	;
		
		my $str = ''	;
		$str .= $_ while <$fh>	;
		close $fh ;
		
		$str =~ s/$seek/$repl/g ;
		
		if ( open( my $fh, ">$file" )) {
			binmode $fh	;
			print $fh $str 	;
			close $fh		;
		}
	}
}




