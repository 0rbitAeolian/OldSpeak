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
#include "bf6p6.h"
#include <structmember.h>

static void
bf6p6_genkey_result_dealloc(BF6P6GenkeyResult *self)
{
    Py_XDECREF(self->primary);
    Py_XDECREF(self->sub);
    Py_XDECREF(self->fpr);
    PyObject_Del(self);
}

static PyMemberDef bf6p6_genkey_result_members[] = {
    { "primary", T_OBJECT, offsetof(BF6P6GenkeyResult, primary), READONLY},
    { "sub", T_OBJECT, offsetof(BF6P6GenkeyResult, sub), READONLY},
    { "fpr", T_OBJECT, offsetof(BF6P6GenkeyResult, fpr), READONLY},
    { NULL, 0, 0, 0}
};

PyTypeObject BF6P6GenkeyResult_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "bf6p6.GenkeyResult",
    sizeof(BF6P6GenkeyResult),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_init = bf6p6_no_constructor,
    .tp_dealloc = (destructor)bf6p6_genkey_result_dealloc,
    .tp_members = bf6p6_genkey_result_members,
};

PyObject *
bf6p6_genkey_result(gpgme_ctx_t ctx)
{
    gpgme_genkey_result_t result;
    BF6P6GenkeyResult *self;

    result = gpgme_op_genkey_result(ctx);

    if (result == NULL)
        Py_RETURN_NONE;

    self = PyObject_New(BF6P6GenkeyResult, &BF6P6GenkeyResult_Type);
    if (!self)
        return NULL;

    self->primary = PyBool_FromLong(result->primary);
    self->sub = PyBool_FromLong(result->sub);
    if (result->fpr)
        self->fpr = PyUnicode_DecodeASCII(result->fpr, strlen(result->fpr),
                                          "replace");
    else {
        Py_INCREF(Py_None);
        self->fpr = Py_None;
    }

    return (PyObject *) self;
}
