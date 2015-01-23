JRift
=====

Java Wrapper (JNI) for the Oculus Rift HMD and Sensors.

Uses Jherico's Oculus SDK port as a submodule, https://github.com/jherico/OculusSDK
Many thanks to Jherico for making it easier to port to new releases of the Oculus SDK! 


Building
========

- Ensure the JAVA_HOME environment variable is set-up and points to a valid Java installation directory. I currently build with JDK 1.6.0.45 64bit.
- Install CMake >= V2.8
- Install Maven if you want to be able to easily package and deploy JRift versions.

JRift - the java part
---------------------

- Change directory to <JRift root>/JRift/
- Run 'mvn package'. This will create the appropriately named <JRift-version>.jar in <JRift root>/JRift/target/

JRiftLibrary - the C++ JNI part
-------------------------------

Windows
-------

- Ensure Visual Studio 12 or higher is installed. 
- Change directory to <JRift root>/JRiftLibrary/
- Run 'build_windows.bat'. This will build the Windows JRiftLibrary 32 and 64 bit native dlls in <JRift root>/JRiftLibrary/natives/windows
- Run 'mvn package'. This will create the appropriately named <JRiftLibrary-version>.jar and <JRiftLibrary-version-windows-natives>.jar in <JRift root>/JRiftLibrary/target/
- Any client program using JRift and JRiftLibrary will have a dependency on the MS VC++ 12 re-distributable packages (32 or 64bit, dependent on the version of Java being used to run your client). Install these before running your program on Windows.

Mac
---

Thanks to krisds, MacOSX is supported as well; need to build native library yourself via Makefile, though.

Linux
-----

You need to install "build-essentials" and "g++-multilib"

run "make"

