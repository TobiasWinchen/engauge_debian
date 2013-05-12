#!/usr/bin/perl

# this script builds Engauge Digitizer source release for linux platforms
# usage: release_linux_src.pl version_number [build_directory]
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
    $tarball = "../digit-src-$versionPrefix\_$versionSuffix.tar.gz";

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
    print "Usage: release_linux_src.pl version_number [build_directory]\n";
    exit;
}
print "\nVersion number: $versionPrefix.$versionSuffix\n";
print "Building binary release in directory: $releaseDir\n";
print "Writing to tarball: $tarball\n\n";

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
print "Recreating source release directory...\n";
mkdir "$releaseDir";

# copy files. do not add die to the copy since a warning is produced when recursive copy is 
# attempted
print "Copying files...\n";
system("cp -r * $releaseDir 2>/dev/null");

# we made it this far, so finish up by tarring up the source release
print "Writing source tarball...\n";
system("tar cvfz $tarball --exclude='*CVS*' --exclude='*cvs*' --exclude='*debian*' $releaseDir") == 0 or
    die "Failed to create tarball: $?";

sub isNumber {
    # return 1 if input is a number, else return 0
    my $n = shift;
    local $noError = 1;
    local $^W = 1;
    local $SIG{__WARN__} = sub { $noError = undef };
    eval { my $x = $n + 0 };
    return ($noError);
} 
