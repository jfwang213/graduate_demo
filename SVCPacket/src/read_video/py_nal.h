#ifndef _PY_NAL_H_
#define _PY_NAL_H_
#include "type.h"

typedef struct {
	PyObject_HEAD
	PyObject*   byte_array_nal;
    int         nal_dqID; 
}py_nal_object;
extern PyTypeObject py_nal_type;

void c_init_nal(py_nal_object* nal,char *bytes,int size);
py_nal_object* c_build_nal(NAL* nal);


#endif
