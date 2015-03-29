SCRIPTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
echo $SCRIPTDIR
CURDIR=.
echo $CURDIR

rm -rf "$SCRIPTDIR/natives/osx/libJRiftLibrary64.jnilib"
rm -rf "$SCRIPTDIR/natives/osx/libJRiftLibrary64.dylib"
rm -rf "$SCRIPTDIR/osx64"
mkdir "$SCRIPTDIR/osx64"
cd "$SCRIPTDIR/osx64"
cmake -DCMAKE_OSX_ARCHITECTURES=x86_64 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.9 "$SCRIPTDIR" -Wno-dev
make
cp ./libJRiftLibrary64.dylib "$SCRIPTDIR/natives/osx/libJRiftLibrary64.jnilib"
cd "$CURDIR"

rm -rf "$SCRIPTDIR/natives/osx/libJRiftLibrary.jnilib"
rm -rf "$SCRIPTDIR/natives/osx/libJRiftLibrary.dylib"
rm -rf "$SCRIPTDIR/osx32"
mkdir "$SCRIPTDIR/osx32"
cd "$SCRIPTDIR/osx32"
cmake -DCMAKE_OSX_ARCHITECTURES=i386 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.9 "$SCRIPTDIR" -Wno-dev
make
cp ./libJRiftLibrary.dylib "$SCRIPTDIR/natives/osx/libJRiftLibrary.jnilib"
cd "$CURDIR"



