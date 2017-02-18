/* -*- mode: C; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
    bf6p6 - a Python wrapper for the gpgme library
    Copyright (C) 2006  James Henstridge

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <Python.h>
#include "bf6p6.h"

static PyMethodDef bf6p6_functions[] = {
    { "make_constants", (PyCFunction)bf6p6_make_constants, METH_VARARGS },
    { NULL, NULL, 0 }
};

#if PY_VERSION_HEX >= 0x03000000
static PyModuleDef bf6p6_module = {
    PyModuleDef_HEAD_INIT,
    "bf6p6._bf6p6",
    .m_size = -1,
    .m_methods = bf6p6_functions
};
#endif

static PyObject *
create_module(void)
{
    const char *gpgme_version;
    PyObject *mod;

    bf6p6_error = PyErr_NewException("bf6p6.BF6P6Error",
                                       PyExc_RuntimeError, NULL);

#define INIT_TYPE(type)                      \
    if (!Py_TYPE(&type))                      \
        Py_TYPE(&type) = &PyType_Type;        \
    if (!type.tp_alloc)                      \
        type.tp_alloc = PyType_GenericAlloc; \
    if (!type.tp_new)                        \
        type.tp_new = PyType_GenericNew;     \
    if (PyType_Ready(&type) < 0)             \
        return NULL

#define ADD_TYPE(type)                \
    Py_INCREF(&BF6P6 ## type ## _Type); \
    PyModule_AddObject(mod, #type, (PyObject *)&BF6P6 ## type ## _Type)

    INIT_TYPE(BF6P6Context_Type);
    INIT_TYPE(BF6P6Key_Type);
    INIT_TYPE(BF6P6Subkey_Type);
    INIT_TYPE(BF6P6UserId_Type);
    INIT_TYPE(BF6P6KeySig_Type);
    INIT_TYPE(BF6P6NewSignature_Type);
    INIT_TYPE(BF6P6Signature_Type);
    INIT_TYPE(BF6P6ImportResult_Type);
    INIT_TYPE(BF6P6GenkeyResult_Type);
    INIT_TYPE(BF6P6KeyIter_Type);

#if PY_VERSION_HEX >= 0x03000000
    mod = PyModule_Create(&bf6p6_module);
#else
    mod = Py_InitModule("bf6p6._bf6p6", bf6p6_functions);
#endif

    ADD_TYPE(Context);
    ADD_TYPE(Key);
    ADD_TYPE(Subkey);
    ADD_TYPE(UserId);
    ADD_TYPE(KeySig);
    ADD_TYPE(NewSignature);
    ADD_TYPE(Signature);
    ADD_TYPE(ImportResult);
    ADD_TYPE(GenkeyResult);
    ADD_TYPE(KeyIter);

    Py_INCREF(bf6p6_error);
    PyModule_AddObject(mod, "BF6P6Error", bf6p6_error);

    gpgme_version = gpgme_check_version(NULL);
    if (gpgme_version == NULL) {
        PyErr_SetString(PyExc_ImportError, "Unable to initialize bf6p6.");
        Py_DECREF(mod);
        return NULL;
    }
    PyModule_AddObject(mod, "bf6p6_version",
                       PyUnicode_DecodeASCII(gpgme_version,
                                             strlen(gpgme_version), "replace"));

    return mod;
}

#if PY_VERSION_HEX >= 0x03000000
PyMODINIT_FUNC
PyInit__bf6p6(void)
{
    return create_module();
}
#else
PyMODINIT_FUNC
init_bf6p6(void)
{
    create_module();
}
#endif
