cd "$(dirname $0)"
java -jar redshell/asc.jar -AS3 -import redshell/builtin.abc -import redshell/toplevel.abc Test.as
redshell/redshell Test.abc