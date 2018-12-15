import re
import subprocess
import filecmp
import os
import sys

class COLORS:
    HEADER = '\033[95m'
    OK_BLUE = '\033[94m'
    OK_GREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    END = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

# list all files in . directory
onlyDirectories = [f for f in os.listdir(".") if os.path.isdir(os.path.join(".", f))]

# filter only .input files
regex = re.compile(r".py")

testPassed = 0
testCounter = 0

for directory in onlyDirectories:
	path = directory
	files = [f for f in os.listdir(path) if os.path.isfile(os.path.join(path, f))]
	files = [i for i in files if regex.search(i)]
	os.chdir(path)
	for file in files:
		cmd = ['python', file]
		process = subprocess.Popen(cmd, stdout=subprocess.PIPE)
		sync_output, sync_error = process.communicate()
		process.wait()
		decoded = sync_output
		decoded = decoded.replace(b'(', b'')
		decoded = decoded.replace(b')', b'')
		decoded = decoded.replace(b'\',', b'')
		decoded = decoded.replace(b' \'', b'')
		decoded = decoded.replace(b'\'', b'')
		decoded = decoded.replace(b'\\x', b'\\x')
		decoded = decoded.replace(b'\\x', b'\\x')
		sys.stdout.write(decoded.decode('unicode_escape'))

	os.chdir("..")

