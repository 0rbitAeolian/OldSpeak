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
bf6p6_newsig_dealloc(BF6P6NewSignature *self)
{
    Py_XDECREF(self->type);
    Py_XDECREF(self->pubkey_algo);
    Py_XDECREF(self->hash_algo);
    Py_XDECREF(self->timestamp);
    Py_XDECREF(self->fpr);
    Py_XDECREF(self->sig_class);
    PyObject_Del(self);
}

static PyMemberDef bf6p6_newsig_members[] = {
    { "type", T_OBJECT, offsetof(BF6P6NewSignature, type), READONLY},
    { "pubkey_algo", T_OBJECT, offsetof(BF6P6NewSignature, pubkey_algo), READONLY},
    { "hash_algo", T_OBJECT, offsetof(BF6P6NewSignature, hash_algo), READONLY},
    { "timestamp", T_OBJECT, offsetof(BF6P6NewSignature, timestamp), READONLY},
    { "fpr", T_OBJECT, offsetof(BF6P6NewSignature, fpr), READONLY},
    { "sig_class", T_OBJECT, offsetof(BF6P6NewSignature, sig_class), READONLY},
    { NULL, 0, 0, 0}
};

PyTypeObject BF6P6NewSignature_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "bf6p6.NewSignature",
    sizeof(BF6P6NewSignature),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_init = bf6p6_no_constructor,
    .tp_dealloc = (destructor)bf6p6_newsig_dealloc,
    .tp_members = bf6p6_newsig_members,
};

PyObject *
bf6p6_newsiglist_new(gpgme_new_signature_t siglist)
{
    PyObject *list;
    gpgme_new_signature_t sig;

    list = PyList_New(0);
    for (sig = siglist; sig != NULL; sig = sig->next) {
        BF6P6NewSignature *item = PyObject_New(BF6P6NewSignature,
                                                 &BF6P6NewSignature_Type);
        if (item == NULL) {
            Py_DECREF(list);
            return NULL;
        }
        item->type = PyInt_FromLong(sig->type);
        item->pubkey_algo = PyInt_FromLong(sig->pubkey_algo);
        item->hash_algo = PyInt_FromLong(sig->hash_algo);
        item->timestamp = PyInt_FromLong(sig->timestamp);
        if (sig->fpr) {
            item->fpr = PyUnicode_DecodeASCII(sig->fpr, strlen(sig->fpr),
                                              "replace");
        } else {
            Py_INCREF(Py_None);
            item->fpr = Py_None;
        }
        item->sig_class = PyInt_FromLong(sig->sig_class);
        if (PyErr_Occurred()) {
            Py_DECREF(item);
            Py_DECREF(list);
            return NULL;
        }
        PyList_Append(list, (PyObject *)item);
        Py_DECREF(item);
    }
    return list;
}

static void
bf6p6_sig_dealloc(BF6P6Signature *self)
{
    Py_XDECREF(self->summary);
    Py_XDECREF(self->fpr);
    Py_XDECREF(self->status);
    Py_XDECREF(self->notations);
    Py_XDECREF(self->timestamp);
    Py_XDECREF(self->exp_timestamp);
    Py_XDECREF(self->wrong_key_usage);
    Py_XDECREF(self->validity);
    Py_XDECREF(self->validity_reason);
    PyObject_Del(self);
}

static PyMemberDef bf6p6_sig_members[] = {
    { "summary", T_OBJECT, offsetof(BF6P6Signature, summary), READONLY},
    { "fpr", T_OBJECT, offsetof(BF6P6Signature, fpr), READONLY},
    { "status", T_OBJECT, offsetof(BF6P6Signature, status), READONLY},
    { "notations", T_OBJECT, offsetof(BF6P6Signature, notations), READONLY},
    { "timestamp", T_OBJECT, offsetof(BF6P6Signature, timestamp), READONLY},
    { "exp_timestamp", T_OBJECT,
      offsetof(BF6P6Signature, exp_timestamp), READONLY},
    { "wrong_key_usage", T_OBJECT,
      offsetof(BF6P6Signature, wrong_key_usage), READONLY},
    { "validity", T_OBJECT, offsetof(BF6P6Signature, validity), READONLY},
    { "validity_reason", T_OBJECT,
      offsetof(BF6P6Signature, validity_reason), READONLY},
    { NULL, 0, 0, 0}
};

PyTypeObject BF6P6Signature_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "bf6p6.Signature",
    sizeof(BF6P6Signature),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_init = bf6p6_no_constructor,
    .tp_dealloc = (destructor)bf6p6_sig_dealloc,
    .tp_members = bf6p6_sig_members,
};

PyObject *
bf6p6_siglist_new(gpgme_signature_t siglist)
{
    PyObject *list;
    gpgme_signature_t sig;
    gpgme_sig_notation_t not;

    list = PyList_New(0);
    for (sig = siglist; sig != NULL; sig = sig->next) {
        BF6P6Signature *item = PyObject_New(BF6P6Signature,
                                              &BF6P6Signature_Type);
        if (item == NULL) {
            Py_DECREF(list);
            return NULL;
        }
        item->summary = PyInt_FromLong(sig->summary);
        if (sig->fpr) {
            item->fpr = PyUnicode_DecodeASCII(sig->fpr, strlen(sig->fpr),
                                              "replace");
        } else {
            Py_INCREF(Py_None);
            item->fpr = Py_None;
        }
        item->status = bf6p6_error_object(sig->status);
        item->notations = PyList_New(0);
        for (not = sig->notations; not != NULL; not = not->next) {
            PyObject *py_name, *py_value, *py_not;

            py_name = PyUnicode_DecodeUTF8(not->name, not->name_len,
                                           "replace");
            py_value = PyBytes_FromStringAndSize(not->value, not->value_len);
            py_not = Py_BuildValue("(NN)", py_name, py_value);

            if (!py_not)
                break;
            PyList_Append(item->notations, py_not);
            Py_DECREF(py_not);
        }
        item->timestamp = PyInt_FromLong(sig->timestamp);
        item->exp_timestamp = PyInt_FromLong(sig->exp_timestamp);
        item->wrong_key_usage = PyBool_FromLong(sig->wrong_key_usage);
        item->validity = PyInt_FromLong(sig->validity);
        item->validity_reason = bf6p6_error_object(sig->validity_reason);
        if (PyErr_Occurred()) {
            Py_DECREF(item);
            Py_DECREF(list);
            return NULL;
        }
        PyList_Append(list, (PyObject *)item);
        Py_DECREF(item);
    }
    return list;
}
