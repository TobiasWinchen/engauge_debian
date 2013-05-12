#!/usr/bin/perl

# this script builds Engauge Digitizer for solaris x386 platforms
# usage: release_solaris.pl version_number [build_directory]
# author: mark mitchell

my($versionPrefix);
my($versionSuffix);
# release directory $releaseDir must in current directory so the release
# directory ends up in the user's current directory when he/she untars. 
# this means no '..' or slashes!
my($releaseDir) = "engauge";
my($tarball);

# parse optional command line argument to override directory defaults
my($numArgs) = $#ARGV + 1;
my($okLine) = ((1 <= $numArgs) && ($numArgs <= 3));
if ($okLine) {
    @fields = split(/\./, $ARGV[0]);
    $versionPrefix = $fields [0];
    $versionSuffix = $fields [1];
    $tarball = "../digit-exe-solaris-x386-$versionPrefix\_$versionSuffix.tar";

    $okLine = ((isNumber($versionPrefix) == 1) && 
	       (isNumber($versionSuffix) == 1));

    if ($numArgs > 1) {
	$releaseDir = $ARGV[1];
    }
}
if ((index($releaseDir, '..') >=0) || (index($releaseDir, '/') >= 0)) {
    print "Release directory must be in the local directory!\n";
    exit;
}
if (! $okLine) {
    print "Usage: release_solaris.pl version_number [build_directory]\n";
    exit;
}
print "\nVersion number: $versionPrefix.$versionSuffix\n";
print "Building release in directory: $releaseDir\n";
print "Writing to compressed file: $tarball.bz2\n\n";

# clean up release directory
print "Deleting existing release directory...\n";
if (-d $releaseDir) {
    use File::Find;
    finddepth (sub {
	if (-f $_) {
	    unlink $_;
	} else {
	    rmdir $_;
	}
    }, $releaseDir);
}

# make directory tree
print "Recreating release directory...\n";
mkdir "$releaseDir";
mkdir "$releaseDir/samples";
mkdir "$releaseDir/usermanual";

# copy files. note that all wildcard copies are performed with file suffixes
# so the CVS subdirectory is not copied, which fails and causes the script to die
print "Copying files...\n";
system("cp -v bin/engauge $releaseDir") == 0 or
    die "Failed to copy: $?";
system("cp -v src/img/lo32-app-digitizer.png $releaseDir") == 0 or
    die "Failed to copy: $?";
system("cp -v LICENSE $releaseDir") == 0 or
    die "Failed to copy: $?";
system("cp -v samples/*.bmp $releaseDir/samples") == 0 or 
    die "Failed to copy: $?";
system("cp -v samples/*.gif $releaseDir/samples") == 0 or 
    die "Failed to copy: $?";
system("cp -v samples/*.jpg $releaseDir/samples") == 0 or 
    die "Failed to copy: $?";
system("cp -v samples/*.png $releaseDir/samples") == 0 or 
    die "Failed to copy: $?";
system("cp -v usermanual/*.gif $releaseDir/usermanual") == 0 or 
    die "Failed to copy: $?";
system("cp -v usermanual/*.html $releaseDir/usermanual") == 0 or 
    die "Failed to copy: $?";
system("cp -v usermanual/*.jpg $releaseDir/usermanual") == 0 or 
    die "Failed to copy: $?";
system("cp -v usermanual/*.png $releaseDir/usermanual") == 0 or 
    die "Failed to copy: $?";

# we made it this far, so finish up by tarring up the release
print "Writing tarball...\n";
system("tar cvf $tarball $releaseDir") == 0 or
    die "Failed to create tarball: $?";
system("bzip2 -f $tarball") == 0 or
    die "Failed to perform bzip2 compression: $?";

sub isNumber {
    # return 1 if input is a number, else return 0
    my $n = shift;
    local $noError = 1;
    local $^W = 1;
    local $SIG{__WARN__} = sub { $noError = undef };
    eval { my $x = $n + 0 };
    return ($noError);
} 
