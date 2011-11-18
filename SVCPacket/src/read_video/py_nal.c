#include "Python.h"
#include "py_nal.h"
#include <structmember.h>



static PyMemberDef py_nal_members[] = {
	{"video_byte", T_OBJECT_EX, offsetof(py_nal_object, byte_array_nal), 0,
	"video content"},
    {"nal_dqID", T_INT, offsetof(py_nal_object, nal_dqID), 0,
	"video content"},
	{NULL}  /* Sentinel */
};

static PyObject *
py_nal_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	py_nal_object *self;
	self = (py_nal_object*)type->tp_alloc(type,0);
	if (NULL != self)
	{
        self->nal_dqID = 0;
		self->byte_array_nal=PyString_FromStringAndSize("",0);
		if (NULL == self->byte_array_nal)
		{
			Py_DecRef((PyObject*)self);
			return NULL;
		}
	}
	return (PyObject*)self;
}

py_nal_object* c_build_nal(NAL* nal)
{
	py_nal_object* res = (py_nal_object*)py_nal_new(&py_nal_type,NULL,NULL);
	if (NULL != res)
	{
        res->nal_dqID = nal->DqId;
		c_init_nal(res,nal->prbsp_memory,nal->nal_byte_size);
	}
	return res;
}

void c_init_nal(py_nal_object* nal,char *bytes,int size)
{
	Py_XDECREF(nal->byte_array_nal);
	nal->byte_array_nal = PyString_FromStringAndSize(bytes,size);
}

static void py_nal_dealloc(py_nal_object* nal)
{
	Py_DecRef(nal->byte_array_nal);
	nal->ob_type->tp_free((PyObject*)nal);
}
PyTypeObject py_nal_type = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"read_video.nal",             /*tp_name*/
	sizeof(py_nal_object),		/*tp_basicsize*/
	0,                         /*tp_itemsize*/
	0,                         /*tp_dealloc*/
	0,                         /*tp_print*/
	0,                         /*tp_getattr*/
	0,                         /*tp_setattr*/
	0,                         /*tp_compare*/
	0,                         /*tp_repr*/
	0,                         /*tp_as_number*/
	0,                         /*tp_as_sequence*/
	0,                         /*tp_as_mapping*/
	0,                         /*tp_hash */
	0,                         /*tp_call*/
	0,                         /*tp_str*/
	0,                         /*tp_getattro*/
	0,                         /*tp_setattro*/
	0,                         /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT,        /*tp_flags*/
	"py_nal type",           /* tp_doc */
	0,		               /* tp_traverse */
	0,		               /* tp_clear */
	0,		               /* tp_richcompare */
	0,		               /* tp_weaklistoffset */
	0,		               /* tp_iter */
	0,		               /* tp_iternext */
	0,             /* tp_methods */
	py_nal_members,             /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	0,      /* tp_init */
	0,                         /* tp_alloc */
	py_nal_new,                 /* tp_new */
};


