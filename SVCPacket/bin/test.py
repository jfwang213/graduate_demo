import read_video

read_video.file_name = "F:\\code\\rtp4svc\\trunk\\bin\\test_4layer.264"
print read_video.init_file(read_video.file_name)
auId = 0
while True:
	access_unit = read_video.read_one_access_unit()
	if access_unit == None:
		break
	print auId
	auId = auId + 1
read_video.close_file()