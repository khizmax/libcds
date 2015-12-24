#!/usr/bin/perl

my $curDir = `cd`;

my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
$year += 1900;


# Get libcds version
my $Version = get_version();
print "Make libcds-$Version distributive\n";

my $DistrDir = get_distrib_dir();
print "Distrib dir: $DistrDir\n";

# Git clone
my $GitBranch = get_git_branch();
my $GitRepo = get_git_repo();
print "Clone git: repo=$GitRepo, branch=$GitBranch\n";
`git clone -b $GitBranch $GitRepo $DistrDir`; # or die "Error cloning branch $GitBranch to $DistrDir\n";
print "Remove $DistrDir/.git directory\n";
`rm -fr $DistrDir/.git`;
`rm -f $DistrDir/.gitignore $DistrDir/tools/brush_cds.pl $DistrDir/tools/make_distrib.pl $DistrDir/tools/make_distrib.bat $DistrDir/doxygen/images.odp`;

print "make copyright...\n" ;
makeCopyright($DistrDir);

print "patch files...\n";
patch_file("$DistrDir/build/Makefile", 'VERSION=\d+\.\d+\.\d+', "VERSION=$Version" );
patch_file("$DistrDir/CMakeList.txt", 'PROJECT_VERSION \d+\.\d+\.\d+', "PROJECT_VERSION $Version" );
patch_file("$DistrDir/doxygen/cds.doxy", 'PROJECT_NUMBER\s*=\s*\d+\.\d+\.\d+', "PROJECT_NUMBER = $Version" ) ;
patch_file("$DistrDir/projects/android/jni/Android.mk", 'CDS_LIBRARY_VERSION\s*:=\s*\d+\.\d+\.\d+', 
   "CDS_LIBRARY_VERSION := $Version" );

print "Make docs\n";
`cd $DistrDir/tools && make_docs.bat && rm doxygen.log && cd $curDir`;
  
print "make zip...\n" ;
`rm -f $DistrDir/../cds-$Version.zip` ;
`cd $DistrDir/.. && 7za a cds-$Version.zip cds-$Version` ;

print "Done\n" ;
exit ;

  
sub get_version()
{
	my $version;
	open( my $fh, 'cds/version.h' ) or die "ERROR: Cannot find ../cds/version.h file";
	binmode $fh ;
	
	while (<$fh>) {
		if ( /CDS_VERSION_STRING.+(\d+\.\d+\.\d+)/ ) {
			$version = $1 ;
			last	;
		}
	}
	close $fh ;
	die "ERROR: cannot find version in ../cds/version.h" unless $version ;
}

sub get_distrib_dir()
{
	my $dir = "../cds-distrib/cds-$Version";
	`rm -fr $dir` if -d $dir;
	mkdir $dir;
	return $dir;
}

sub get_git_repo()
{
	return 'git@github.com:khizmax/libcds.git';
}

sub get_git_branch()
{
	my $branchList = `git branch`;
	#print "$branchList\n";
	
	# Search "v$Version-rc" branch
	my ($branch) = $branchList =~ /(v$Version-rc\d*)/g;
	return $branch || 'master';
}

sub makeCopyright($) 
{
	my $distrDir = shift;
	processDir( "$distrDir/cds" ) ;
	processDir( "$distrDir/src" ) ;
	processDir( "$distrDir/tests/test-hdr" ) ;
	processDir( "$distrDir/tests/unit" ) ;
	processDir( "$distrDir/tests/cppunit" ) ;
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
    Version: $Version

    (C) Copyright Maxim Khizhinsky (libcds.dev\@gmail.com) 2006-$year
    Distributed under the BSD license (see accompanying file LICENSE)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/\n" ;
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

sub patch_file(@) {
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
