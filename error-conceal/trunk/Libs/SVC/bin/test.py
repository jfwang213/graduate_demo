import subprocess
import os
import time

folder = "F:\\code\\ParallelSvc2\\trunk\\Libs\\SVC\\bin\\"
for i in range(100):
	output_file = file(folder+'output'+str(i)+'.txt','w')
	decoder = subprocess.Popen([folder+'SVC_decoder_v9.exe',"-h264","test_4layer.264","-layer","17","-silent"],stdout=output_file)
	time.sleep(2)
	print decoder.pid
	decoder.wait()
	if os.path.exists(folder+'loss_log'+str(i)+'.txt'):
		os.remove(folder+'loss_log'+str(i)+'.txt')
	os.rename(folder+'loss_log.txt',folder+'loss_log'+str(i)+'.txt')
	output_file.close()