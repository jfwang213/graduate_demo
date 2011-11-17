#!/usr/bin/env python

class Log(object):
    def __init__(self, fileName):
        self.logFile = file(fileName, 'w')


    def LogStr(self, logStr):
        self.logFile.write(logStr + '\n')

    def __del__(self):
        self.logFile.close()
