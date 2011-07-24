import os 

folder = "F:\\code\\ParallelSvc2\\trunk\\Libs\\SVC\\bin\\"
for i in range(100):
	result_file = file(folder+"output"+str(i)+'.txt')
	lastline = ''
	line = result_file.readline()
	while len(line)>0:
		lastline = line
		line = result_file.readline()
	print i,lastline[0:-1]
	result_file.close()