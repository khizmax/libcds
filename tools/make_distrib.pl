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
my $GitBranch = 'master';
my $GitRepo = get_git_repo();
print "Clone git: repo=$GitRepo, branch=$GitBranch\n";
`git clone -b $GitBranch $GitRepo $DistrDir`; # or die "Error cloning branch $GitBranch to $DistrDir\n";
print "Remove $DistrDir/.git directory\n";
`rm -fr $DistrDir/.git`;
`rm -f $DistrDir/.gitignore $DistrDir/tools/brush_cds.pl $DistrDir/tools/make_distrib.pl $DistrDir/tools/make_distrib.bat $DistrDir/doxygen/images.odp`;

print "patch files...\n";
patch_file("$DistrDir/CMakeList.txt", 'PROJECT_VERSION \d+\.\d+\.\d+', "PROJECT_VERSION $Version" );
patch_file("$DistrDir/doxygen/cds.doxy", 'PROJECT_NUMBER\s*=\s*\d+\.\d+\.\d+', "PROJECT_NUMBER = $Version" ) ;

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
	return 'https://github.com/khizmax/libcds.git';
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
