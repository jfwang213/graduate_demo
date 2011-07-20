rmdir /S /Q lib.win32-2.7
rmdir /S /Q temp.win32-2.7
python setup.py build --build-base="%cd%" install