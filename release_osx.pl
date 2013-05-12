#!/usr/bin/perl

# this script builds Engauge Digitizer for osx platforms
# usage: release_osx.pl version_number [build_directory] [qt_library_directory]
# author: mark mitchell

my($versionPrefix);
my($versionSuffix);
# release directory $releaseDir must in current directory so the release
# directory ends up in the user's current directory when he/she untars. 
# this means no '..' or slashes!
my($releaseDir) = "Engauge.app";
my($qtLibDir) = "$ENV{QTDIR}/lib";
my($tarball);

# parse optional command line argument to override directory defaults
my($numArgs) = $#ARGV + 1;
my($okLine) = ((1 <= $numArgs) && ($numArgs <= 3));
if ($okLine) {
    @fields = split(/\./, $ARGV[0]);
    $versionPrefix = $fields [0];
    $versionSuffix = $fields [1];
    $tarball = "../digit-exe-osx-$versionPrefix\_$versionSuffix.tar.gz";

    $okLine = ((isNumber($versionPrefix) == 1) &&
               (isNumber($versionSuffix) == 1));

    if ($numArgs > 1) {
        $releaseDir = $ARGV[1];
        if ($numArgs > 2) {
            $qtLibDir = $ARGV[2];
        }
    }
}
if ((index($releaseDir, '..') >=0) || (index($releaseDir, '/') >= 0)) {
    print "Release directory must be in the local directory!\n";
    exit;
}
if (! $okLine) {
    print "Usage: release_osx.pl version_number [build_directory] [qt_library_directory]\n";
    exit;
}
print "\nVersion number: $versionPrefix.$versionSuffix\n";
print "Building release in directory: $releaseDir\n";
print "Using Qt library directory: $qtLibDir\n";
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
print "Recreating release directory...\n";
mkdir "$releaseDir";
mkdir "$releaseDir/Contents";
mkdir "$releaseDir/Contents/Frameworks";
mkdir "$releaseDir/Contents/MacOS";
mkdir "$releaseDir/Contents/Resources";
mkdir "$releaseDir/Contents/Resources/samples";
mkdir "$releaseDir/Contents/Resources/usermanual";

# copy files. note that all wildcard copies are performed with file suffixes
# so the CVS subdirectory is not copied, which fails and causes the script to die
print "Copying files...\n";
system("cp -v osx/Info.plist $releaseDir/Contents") == 0 or
    die "Failed to copy: $?";
system("cp -v osx/PkgInfo $releaseDir/Contents") == 0 or
    die "Failed to copy: $?";
system("cp -v $qtLibDir/libqt-mt.3.dylib $releaseDir/Contents/Frameworks") == 0 or
    die "Failed to copy: $?";
system("cp -v bin/engauge $releaseDir/Contents/MacOS") == 0 or
    die "Failed to copy: $?";
system("cp -v LICENSE $releaseDir/Contents/Resources") == 0 or
    die "Failed to copy: $?";
system("cp -v samples/*.bmp $releaseDir/Contents/Resources/samples") == 0 or
    die "Failed to copy: $?";
system("cp -v samples/*.gif $releaseDir/Contents/Resources/samples") == 0 or
    die "Failed to copy: $?";
system("cp -v samples/*.jpg $releaseDir/Contents/Resources/samples") == 0 or
    die "Failed to copy: $?";
system("cp -v samples/*.png $releaseDir/Contents/Resources/samples") == 0 or
    die "Failed to copy: $?";
system("cp -v usermanual/*.gif $releaseDir/Contents/Resources/usermanual") == 0 or
    die "Failed to copy: $?";
system("cp -v usermanual/*.html $releaseDir/Contents/Resources/usermanual") == 0 or
    die "Failed to copy: $?";
system("cp -v usermanual/*.jpg $releaseDir/Contents/Resources/usermanual") == 0 or
    die "Failed to copy: $?";
system("cp -v usermanual/*.png $releaseDir/Contents/Resources/usermanual") == 0 or
    die "Failed to copy: $?";

# osx requires that executable be told where library is found. to see
# where the executable believes the library is found use
#    "otool -L Engauge.app/Contents/MacOS/digitizer"
# as documented at http://doc.trolltech.com/qq/qq09-mac-deployment.html
print "Connecting bundled Qt libraries...\n";
system("install_name_tool -id \@executable_path/../Frameworks/libqt-mt.3.dylib Engauge.app/Contents/Frameworks/libqt-mt.3.dylib") == 0 or
    die "Failed to change identification name: $?";
system("install_name_tool -change libqt-mt.3.dylib \@executable_path/../Frameworks/libqt-mt.3.dylib Engauge.app/Contents/MacOS/engauge") == 0 or
    die "Failed to attach executable to library: $?";

# we made it this far, so finish up by tarring up the release
print "Writing tarball...\n";
system("tar cvfz $tarball $releaseDir") == 0 or
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
