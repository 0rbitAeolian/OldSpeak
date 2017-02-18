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

static void
bf6p6_keyiter_dealloc(BF6P6KeyIter *self)
{
    if (self->ctx) {
        gpgme_error_t err = gpgme_op_keylist_end(self->ctx->ctx);
        PyObject *exc = bf6p6_error_object(err);

        if (exc != NULL && exc != Py_None) {
            PyErr_WriteUnraisable(exc);
        }
        Py_XDECREF(exc);
        Py_DECREF(self->ctx);
        self->ctx = NULL;
    }
    PyObject_Del(self);
}

static PyObject *
bf6p6_keyiter_iter(BF6P6KeyIter *self)
{
    Py_INCREF(self);
    return (PyObject *)self;
}

static PyObject *
bf6p6_keyiter_next(BF6P6KeyIter *self)
{
    gpgme_key_t key = NULL;
    gpgme_error_t err;
    PyObject *ret;

    Py_BEGIN_ALLOW_THREADS;
    err = gpgme_op_keylist_next(self->ctx->ctx, &key);
    Py_END_ALLOW_THREADS;

    /* end iteration */
    if (gpgme_err_source(err) == GPG_ERR_SOURCE_GPGME &&
        gpgme_err_code(err) == GPG_ERR_EOF) {
        PyErr_SetNone(PyExc_StopIteration);
        return NULL;
    }

    if (bf6p6_check_error(err))
        return NULL;

    if (key == NULL)
        Py_RETURN_NONE;

    ret = bf6p6_key_new(key);
    gpgme_key_unref(key);
    return ret;
}

PyTypeObject BF6P6KeyIter_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "bf6p6.KeyIter",
    sizeof(BF6P6KeyIter),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_init = bf6p6_no_constructor,
    .tp_dealloc = (destructor)bf6p6_keyiter_dealloc,
    .tp_iter = (getiterfunc)bf6p6_keyiter_iter,
    .tp_iternext = (iternextfunc)bf6p6_keyiter_next,
};
