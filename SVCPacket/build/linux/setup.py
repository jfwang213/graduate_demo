#!/usr/bin/env python
from distutils.core import setup, Extension
import os
src_file_header = '../../src/read_video'
src_files = os.listdir(src_file_header)
c_files = []
for i in range(len(src_files)):
	if src_files[i][-2:] == '.c':
		c_files.append(src_file_header + '/'+src_files[i])
read_video_module = Extension('read_video',
		sources = c_files)
setup (name = 'read_video',
		version = '1.0',
		description = 'This is a package for read H.264 SVC video file',
		ext_modules = [read_video_module])
