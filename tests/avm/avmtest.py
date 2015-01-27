import glob, os, argparse, shutil, subprocess, time

ASC      	 = os.path.abspath( 'redshell/asc.jar' ).replace( '\\', '/' )
REDSHELL 	 = os.path.abspath( 'redshell/redshell' ).replace( '\\', '/' )
AVMSHELL	 = os.path.abspath( 'avmshell/avmshell' ).replace( '\\', '/' )

# run
def run( cmd ):
	if os.name == 'nt':
		cmd = cmd.split( ' ' )

	try:
		output = subprocess.check_output( cmd, shell=True, stderr=subprocess.STDOUT )
	except Exception, e:
		output = str( e.output )

	return output

# check
def check( cmd ):
	if os.name == 'nt':
		cmd = cmd.split( ' ' )

	try:
		output = subprocess.check_output( cmd, shell=True, stderr=subprocess.STDOUT )
	except Exception, e:
		output = str( e.output )

	return output

# compile
def compile( folder ):
	output = os.path.abspath( os.path.basename( folder ) + '.abc' )

	# Remove previous ABC
	if os.path.exists( output ):
		shutil.rmtree( output )

	# Copy source files
	shutil.copytree( folder, output )

	# Compile files
	os.chdir( output )

	print 'Compiling source files...'
	for sourceFile in glob.glob( '*.as' ):
		print 'Compiling ' + sourceFile
		run( 'java -jar {0} -AS3 -import ../redshell/builtin.abc -import ../redshell/toplevel.abc {1}'.format( ASC, sourceFile ) )

	# Clean
	for root, dirs, files in os.walk( output ):
		for currentDir in dirs:
			shutil.rmtree( os.path.join( output, currentDir ) )
		for currentFile in files:
			if currentFile.lower().endswith( '.as' ):
				os.remove( os.path.join( root, currentFile ) )

# test
def test( folder ):
	failed = []
	tests  = glob.glob( os.path.join( folder, '*.abc' ) )

	for abcFile in tests:
		fileName = os.path.basename( abcFile )

		timeA = time.time()
		a 	  = check( '{0} {1}'.format( REDSHELL, abcFile ) )
		timeA = time.time() - timeA

		timeB = time.time()
		b 	  = check( '{0} -abc {1}'.format( AVMSHELL, abcFile ) )
		timeB = time.time() - timeB

		if a == b:
			factor = timeA / timeB
			speed  = '[{:10.5f}]'.format( factor ).replace( ' ', '' )
			print '\033[92mPassed ' + fileName + ' ' + speed
		else:
			print '\033[91mFailed ' + fileName
			failed.append( { 'name': os.path.splitext( fileName )[0], 'expected': a, 'result': b } )

	print '\033[90m[{0}/{1}] tests passed\033[99m'.format( len( tests ) - len( failed ), len( tests ) )

	if len( failed ):
		output = os.path.join( folder, 'failed' )
		if os.path.exists( output ):
			shutil.rmtree( output )
		os.mkdir( output )

		for fail in failed:
			with open( os.path.join( output, fail['name'] + '.txt' ), 'wt' ) as fh:
				fh.write( 'Expected: {0}\n\nResult: {1}'.format( fail['expected'], fail['result'] ) )

# Entry point
if __name__ == "__main__":
	# Parse arguments
	parser = argparse.ArgumentParser( description = 'Dreemchest SWF tag reader generator' )

	parser.add_argument( 'action', type = str, help = 'AVM2 test suite action [compile, test]' )
	parser.add_argument( 'input',  type = str, help = 'Input folder' )

	args = parser.parse_args()

	if args.action == 'compile':
		compile( args.input )
	elif args.action == 'test':
		test( args.input )
