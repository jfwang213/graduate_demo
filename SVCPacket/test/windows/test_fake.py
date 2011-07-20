import sys
sys.path.append('../../src/')

from App import FakeRtpApp
file_name = "F:\\code\\rtp4svc\\trunk\\bin\\test_4layer.264"
app = FakeRtpApp.FakeRtpApp(file_name)
app.work()