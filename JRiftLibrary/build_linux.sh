SCRIPTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
echo $SCRIPTDIR
CURDIR=.
echo $CURDIR

if [ "$1" == "-m64" ]; then
   rm -rf "$SCRIPTDIR/natives/linux/libJRiftLibrary64.so"
   rm -rf "$SCRIPTDIR/linux64"
   mkdir "$SCRIPTDIR/linux64"
   cd "$SCRIPTDIR/linux64"
   cmake -DCMAKE_C_FLAGS="-m64" -DCMAKE_CXX_FLAGS="-m64" -DCMAKE_SHARED_LINKER_FLAGS="-m64" "$SCRIPTDIR"
   make
   cp "$SCRIPTDIR/linux64/libJRiftLibrary64.so" "$SCRIPTDIR/natives/linux/"
   cd "$CURDIR"
elif [ "$1" == "-m32" ]; then
   rm -rf "$SCRIPTDIR/natives/linux/libJRiftLibrary.so"
   rm -rf "$SCRIPTDIR/linux32"
   mkdir "$SCRIPTDIR/linux32"
   cd "$SCRIPTDIR/linux32"
   cmake -DCMAKE_C_FLAGS="-m32" -DCMAKE_CXX_FLAGS="-m32" -DCMAKE_SHARED_LINKER_FLAGS="-m32" "$SCRIPTDIR"
   make
   cp "$SCRIPTDIR/linux32/libJRiftLibrary.so" "$SCRIPTDIR/natives/linux/"
   cd "$CURDIR"
else
   echo "Missing argument '-m32' (for 32bit build) or '-m64' (for 64bit build)"
   exit 1
fi

