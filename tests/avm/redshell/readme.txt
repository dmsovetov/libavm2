--------------------------------------------------------------------------------
redtamarin
==========

Simply put, redtamarin is based on the Tamarin project
http://www.mozilla.org/projects/tamarin/ (eg. the Flash Player Virtual Machine)
and allow to run AS3 code on the command-line.

But because Tamarin only support the AS3 builtins (Object, Array, etc.),
redtamarin add numerous native functionalities, so you can test if your program
run under Windows or Linux, or read/write files, or send data with sockets, etc.

Put another way, if Adobe AIR allow you to build desktop executable with a GUI,
redtamarin allow you to build desktop executable with a CLI.


Mission Statement
-----------------

To support the use of the AS3 language for cross-platform command-line executable,
as single exe either for the desktop or the server, as scripts for automation,
as tools for the Flash Platform community.


Usage
-----

A. running AS3 scripts

hello.as
----
var sys:Class = getClassByName( "avmplus.System" );

trace( "hello world" );
trace( "what is your name ?" );
var name:String = readLine();
trace( "hello " + name );
trace( "I'm Avmplus v" + sys.getAvmplusVersion() );
----

run it with
----
$ ./redshell hello.as
----

note:
eval() is not fully functional in tamarin
but even if you can not use 'import', you
can have a workaround using 'getClassByName'


B. running ABC files

sysinfo.as
----
import avmplus.OperatingSystem;

trace( "             OperatingSystem.name = " + OperatingSystem.name );
trace( "         OperatingSystem.username = " + OperatingSystem.username );
trace( "         OperatingSystem.nodename = " + OperatingSystem.nodename );
trace( "         OperatingSystem.hostname = " + OperatingSystem.hostname );
trace( "          OperatingSystem.release = " + OperatingSystem.release );
trace( "          OperatingSystem.version = " + OperatingSystem.version );
trace( "          OperatingSystem.machine = " + OperatingSystem.machine );
trace( "           OperatingSystem.vendor = " + OperatingSystem.vendor );
trace( "       OperatingSystem.vendorName = " + OperatingSystem.vendorName );
trace( "    OperatingSystem.vendorVersion = " + OperatingSystem.vendorVersion );
trace( "OperatingSystem.vendorDescription = " + OperatingSystem.vendorDescription );
trace( "         OperatingSystem.codename = " + OperatingSystem.codename );
----

compile the *.abc
----
$ java -jar asc.jar -AS3 -import builtin.abc -import toplevel.abc sysinfo.as
sysinfo.abc, 914 bytes written
----

run it with
----
$ ./redshell sysinfo.abc
----

C. making EXE files

----
$ java -jar asc.jar -AS3 -import builtin.abc -import toplevel.abc -exe redshell sysinfo.as
sysinfo.abc, 914 bytes written
sysinfo.exe, 1718002 bytes written
$ mv sysinfo.exe sysinfo
$ chmod +x sysinfo
----

run it with
----
$ ./sysinfo
----



Documentation
-------------

If you want to compile redtamarin from sources follow those articles
http://code.google.com/p/redtamarin/wiki/Coders
http://code.google.com/p/redtamarin/wiki/Code101
http://code.google.com/p/redtamarin/wiki/Build

API reference
http://code.google.com/p/redtamarin/wiki/API

C libraries documentation
http://code.google.com/p/redtamarin/wiki/StandardC

AVMPlus API documentation
http://code.google.com/p/redtamarin/wiki/AVMPlus


more documentations to come in
http://code.google.com/p/redtamarin/wiki/Examples


Components
----------

You can find SWC components that will provide syntax completion for
  * Flex Builder 3.0+
  * Flash BUilder 4.0+
  * Flash Develop 3.0+

builtin.swc map exactly with builtin.abc
clib.swc map with the C.* package (depends on builtin.swc)

those two SWC are here for people who want to build their own libraries
on top of redtamarin (eg. if you implement your own toplevel.abc)
and want to generate their own SWC.

For everyone else use:
redtamarin.swc as it contains everything

--------------------------------------------------------------------------------

