#include "Python.h"
#include "py_access_unit.h"
#include "read_video.h"
#include "py_access_unit.h"

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif


decoder_context pdeocder_context[1];


PyObject *
print_file_name(PyObject *self, PyObject *args)
{
	const char* pcfile_name;
	PyArg_ParseTuple(args,"s",&pcfile_name);
	printf("%s\n",pcfile_name);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *
init_file(PyObject *self, PyObject *args)
{
	const char* pcfile_name;
	PyArg_ParseTuple(args,"s",&pcfile_name);
	printf("%s\n",pcfile_name);
	if (read_video_init(pcfile_name))
	{
		Py_INCREF(Py_True);
		return Py_True;
	}
	else
	{
		Py_INCREF(Py_False);
		return Py_False;
	}
}

PyObject *
close_file(PyObject *self, PyObject *args)
{
	close_read_video();
	Py_INCREF(Py_True);
	return Py_True;
}

PyObject* 
read_one_access_unit(PyObject *self, PyObject *args)
{
	PyObject* py_au;
	access_unit* au = get_one_access_unit(pdeocder_context->all_sps,pdeocder_context->all_pps,pdeocder_context);
	if(NULL == au)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
	py_au = c_build_py_access_unit(au);
	free_access_unit(au);
	return py_au;
}

static PyMethodDef read_video_methods[] = {
	{"print_file_name", print_file_name, METH_VARARGS, "print file name."},
	{"init_file", init_file, METH_VARARGS, "init video file."},
	{"close_file", close_file, METH_VARARGS, "close video file."},
	{"read_one_access_unit", read_one_access_unit, METH_VARARGS, "read one access unit from file."},
	{NULL, NULL}
};

PyMODINIT_FUNC
initread_video(void) 
{
	PyObject* py_module;

	if (PyType_Ready(&py_access_unit_type) < 0)
	{
		printf("access unit type error!\n");
		return;
	}
	if (PyType_Ready(&py_nal_type) < 0)
	{
		printf("nal type error!\n");
		return;
	}

	py_module = Py_InitModule3("read_video", read_video_methods,
		"Example module that creates an extension type.");
	{
		PyObject* py_file_name;
		py_file_name = PyString_FromString("");
		PyModule_AddObject(py_module, "file_name", py_file_name);

	}
	
}
