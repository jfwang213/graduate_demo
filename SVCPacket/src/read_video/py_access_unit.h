#ifndef _PY_ACCESS_UNIT_H_
#define _PY_ACCESS_UNIT_H_

#include "Python.h"
#include "py_nal.h"
#include "AccessUnit.h"


typedef struct {
	PyObject_HEAD
	PyObject* list_nal;
		/* Type-specific fields go here. */
} py_access_unit_object;


extern PyTypeObject py_access_unit_type;

PyObject* c_build_py_access_unit(access_unit* c_au);

#endif
