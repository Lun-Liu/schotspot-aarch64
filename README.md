# VBD-HotSpot

This project modified the aarch64 port of HotSpot JVM in OpenJDK8u to provide Volatile-By-Default semantics for HotSpot JVM on ARM64 architecture. ```master``` branch has speculative compilation enabled by default. To use a version based on VBDA-HotSpot without speculative compilation, switch to ```new_membar``` branch, which can also supports VBD-Opt fence insretion optimization by using the flag ```-XX:+VBDOpt```.

This repo is a part of the repo cloned from the Mercurial repository aarch64-port/jdk8u (last changeset: 1893:4ac8e58e26e2). Therefore, to build this JVM from source, get the repository from openjdk Mercurial repository and switch to the revision.
```
hg clone http://hg.openjdk.java.net/aarch64-port/jdk8u jdk8u
cd jdk8u/
bash ./get_source.sh
hg update -r 1893:4ac8e58e26e2
```
The original README file can also be found in this repo.

Update ```hotspot/src``` to this repo. 
```
git init
git remote add origin git@github.com:Lun-Liu/schotspot-aarch64.git
git fetch origin
git checkout -b master --track origin/master  --force
```

## Build

0. before attempting to build, setup the sytem according to [System Setup] section of file 
./Readme-builds.html

1. if you don't have a jdk7u7 or newer jdk, download and install it from
http://java.sun.com/javase/downloads/index.jsp
add the /bin directory of this installation to your path environment
variable.

2. configure the build, install necessary dependencies:
	```
	bash ./configure
	```
	you may also want to run the configuration with additional flags like ```--with-freetype-include=/usr/include/freetype2/ --with-freetype-lib=/usr/lib/aarch64-linux-gnu``` if you run into problems of locating freetype. Or ```--with-extra-cflags=-Wno-error=deprecated-declarations --with-extra-cxxflags=-Wno-error=deprecated-declarations``` if there is a GCC compatability issue.

3. build the openjdk:
	```
	make all
	```

the resulting jdk image should be found in build/*. 

OpenJDK8u has some compatability issue with gcc 6.0. You can consider downgrade your gcc if you run into this issue.

## Run Java programs
```master``` branch will build to S-VBD and ```new_membar``` branch will build to VBDA-HotSpot.

There are several JVM flags we may use:
* -XX:-TieredCompilation: turn off c1 compiler (only the c2 compiler is modified now).
* -XX:+SC: enable VBD semantics
* -XX:-OptimizeStringConcat: disables String intrinsics optimizations
* -XX:+AggresiveMemBar: allows more optimizations for non-escaping objects
* -XX:+SCDynamic: enable speculative compilation
* -XX:+VBDOpt: enable VBD-Opt fence insertion optimization
* -XX:+DynamicCheckOnly: Check-Only version for S-VBD

Note that VBDA-HotSpot follows the design of previous work on x86 (https://github.com/SC-HotSpot/VBD-HotSpot) and thus inherits usage of flags: -XX:-TieredCompilation, -XX:-OptimizeStringConcat, and -XX:+AggresiveMemBar. Also note that when -XX:+SC is not on, VBDA-HotSpot should be the same as the baseline JVM. 

We can use different flags to run Java programs with different VMs. Below are some examples.

To run Java program using baseline JVM with the default one way fence, use the following command:

```
/PATH/TO/java -XX:-TieredCompilation SomeJavaProgram
```

To run some Java program using baseline JVM with two-way fence, use the following command:
```
/PATH/TO/java -XX:-TieredCompilation -XX:+UseBarriersForVolatile SomeJavaProgram
```

To run some Java program using VBDA-HotSpot with default one-way fence, use the following command:
```
/PATH/TO/java -XX:-TieredCompilation -XX:+SC -XX:-OptimizeStringConcat -XX:+AggresiveMemBar SomeJavaProgram
```

To run some Java program using VBDA-HotSpot with two-way fence, use the following command:
```
/PATH/TO/java -XX:-TieredCompilation -XX:+SC -XX:-OptimizeStringConcat -XX:+AggresiveMemBar -XX:+UseBarriersForVolatile SomeJavaProgram
```

To run some Java program with S-VBD, use the following command:
```
/PATH/TO/java -XX:-TieredCompilation -XX:+SC -XX:+SCDynamic -XX:-OptimizeStringConcat -XX:+AggresiveMemBar -XX:+UseBarriersForVolatile -XX:+VBDOpt SomeJavaProgram
```
