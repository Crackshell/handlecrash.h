#!/usr/bin/python
#
# handlecrash.h
# https://github.com/angelog/handlecrash.h

import re
import sys
import zlib
import base64
import subprocess

def addr2line(name, ptr, intext = False):
	args = ['addr2line', '-Cfpe', name, ptr]
	if intext:
		args.append('-j')
		args.append('.text')

	p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	out, err = p.communicate()
	if out.find('??') != -1:
		return False
	return out.strip()

if len(sys.argv) == 1:
	print 'Usage: ./decode.py crash_1475679908.log'
	sys.exit(1)

with open(sys.argv[1]) as f:
	readingBacktrace = False
	backtrace = []

	readingStackMemory = False
	stackMemory = ''

	for line in f:
		line = line.strip()
		if readingBacktrace:
			if line[4:] == '':
				readingBacktrace = False
				for frame in backtrace:
					parse = re.match('([^\\(]+)\\((\\+0x[0-9a-f]+|)\\)', frame[2])
					addr = False

					if parse != None:
						addr = addr2line(parse.group(1), frame[1])
						if addr == False and parse.group(2) != '':
							addr = addr2line(parse.group(1), parse.group(2), True)

					if addr == False:
						print '***   ' + frame[0] + '\t' + frame[1] + '\t' + frame[2]
					else:
						parse = addr.split(' at ')
						print '***   ' + frame[0] + '\t' + parse[0] + '\t' + parse[1]
				print line
				continue
			backtrace.append(line[6:].split('\t'))
		elif readingStackMemory:
			stackMemory += line[4:]
		else:
			if line[4:13] == 'Backtrace':
				readingBacktrace = True
			elif line[4:17] == 'Stack memory:':
				readingStackMemory = True
				continue
			print line

	with open(sys.argv[1] + '.bin', 'wb') as fbin:
		compressed = base64.b64decode(stackMemory)
		fbin.write(zlib.decompress(compressed))
