
def log_end():
	global log_files
	for i in range(len(log_files)):
		log_files[i].close()

def log_str(str,log_id):
	if log_id == -1:
		return
	global log_files
	log_files[log_id].write(str+"\n")
	
def log_start(log_file_num = 2):
	global log_files
	log_files = []
	for i in range(log_file_num):
		log_file = file('log'+str(i)+'.txt','w')
		log_files.append(log_file)
