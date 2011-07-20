#include "Python.h"
#include <structmember.h>
#include "py_access_unit.h"
#include "AccessUnit.h"

static PyMemberDef py_access_unit_members[] = {
	{"nals", T_OBJECT_EX, offsetof(py_access_unit_object, list_nal), 0,
	"nal list"},
	{NULL}
};

static PyObject *
py_access_unit_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	py_access_unit_object *self;
	self = (py_access_unit_object*)type->tp_alloc(type,0);
	if (NULL != self)
	{
		self->list_nal = PyList_New(0);
		if (NULL == self->list_nal)
		{
			Py_DecRef((PyObject*)self);
			return NULL;
		}
	}
	return (PyObject*)self;
}

PyObject* c_build_py_access_unit(access_unit* c_au)
{
	py_access_unit_object* res = (py_access_unit_object*)py_access_unit_new(&py_access_unit_type,NULL,NULL);
	if(NULL != res)
	{
		NAL* iter = NULL;
		while (iter = get_next_nal(c_au,iter))
		{
			py_nal_object* py_nal = c_build_nal(iter);
			if (NULL != py_nal)
			{
				PyList_Append(res->list_nal,(PyObject*)py_nal);
				//dec ref?????
			}
		}
	}
	return (PyObject*)res;
}

PyTypeObject py_access_unit_type = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"read_video.access_unit",             /*tp_name*/
	sizeof(py_access_unit_object), /*tp_basicsize*/
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
	"py_access_unit type",           /* tp_doc */
	0,		               /* tp_traverse */
	0,		               /* tp_clear */
	0,		               /* tp_richcompare */
	0,		               /* tp_weaklistoffset */
	0,		               /* tp_iter */
	0,		               /* tp_iternext */
	0,             /* tp_methods */
	py_access_unit_members,             /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	0,      /* tp_init */
	0,                         /* tp_alloc */
	py_access_unit_new,                 /* tp_new */
};

