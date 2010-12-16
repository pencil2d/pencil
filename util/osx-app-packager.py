#!/usr/bin/env python

import os
import os.path
import shutil
import sys

if len(sys.argv) < 2:
	sys.stderr.write('No binary to package specified.');
	sys.stderr.flush()

#print '#!/bin/sh\n'
binary = sys.argv[1]
if binary[0] != '/':
	binary = os.path.join(os.getcwd(), binary)
frameworkDir = os.path.normpath(os.path.join(os.path.dirname(binary), '../Frameworks'))
if not os.path.exists(frameworkDir):
	os.mkdir(frameworkDir)

print '\n# update binary', binary
output = os.popen('otool -L "%s"' % binary).readlines()
for line in output:
	line = line.split('(', 1)[0].strip()
	if line.startswith('/System') or line.startswith('/usr/lib') or line.startswith('/usr/X11') or line.endswith(':'):
		pass
	elif os.path.splitext(line)[1] == '.dylib':
		if line.startswith('@'):
			if os.path.exists(os.path.join(frameworkDir, line.rsplit('/', 1)[1].strip())): continue
			else: line = os.path.join('/usr/local/lib', line.rsplit('/', 1)[1].strip())
		realName = os.path.realpath(line)
		shutil.copy(realName, frameworkDir)
		frameworkName = os.path.basename(realName)
		os.system('install_name_tool -change %s @executable_path/../Frameworks/%s %s' % (line, frameworkName, binary))
	elif line.split('/', 1)[0].strip().endswith('framework'):
		frameworkRootName = line.split('/', 1)[0].strip()
		frameworkName = os.path.basename(line)
		frameworkRoot = os.path.join(frameworkDir, frameworkRootName)
		if os.path.exists(frameworkRoot): shutil.rmtree(frameworkRoot)
		os.mkdir(frameworkRoot)
		frameworkResources = os.path.join(
			'/Library/Frameworks',
			os.path.dirname(line),
			'Resources')
		frameworkRootResources = os.path.join(frameworkRoot, 'Resources')
		if os.path.exists(frameworkResources):
			#if os.path.exists(frameworkRootResources): shutil.rmtree(frameworkRootResources)
			shutil.copytree(frameworkResources, frameworkRootResources)
		shutil.copy(os.path.join('/Library/Frameworks', line), frameworkRoot)
		os.system('install_name_tool -change %s @executable_path/../Frameworks/%s/%s %s' % (line, frameworkRootName, frameworkName, binary))

frameworks = os.listdir(frameworkDir)
for framework in frameworks:
	print '# update framework', framework
	binary = None
	if framework.endswith('.framework'):
		binary = framework.split('.', 1)[0].strip()
	target = os.path.join(frameworkDir, framework, binary) if binary else os.path.join(frameworkDir, framework)
	output = os.popen('otool -L "%s"' % target).readlines()
	for line in output:
		line = line.split('(', 1)[0].strip()
		if line.startswith('/System') or line.startswith('/usr/lib') or line.startswith('/usr/X11') or line.endswith(':'):
			pass
		else:
			name = os.path.basename(line)
			if name in frameworks:
				pass
			elif name + '.framework' in frameworks:
				name = name + '.framework/' + name
			else:
				print 'Could not resolve the framework or dynamic library reference', line, 'on framework or dynamic library', target
				continue
			os.system('install_name_tool -change %s @executable_path/../Frameworks/%s %s' % (line, name, target))
	name = framework + '/' + framework.split('.', 1)[0].strip() if binary else framework
	os.system('install_name_tool -id @executable_path/../Frameworks/%s %s' % (name, target))
