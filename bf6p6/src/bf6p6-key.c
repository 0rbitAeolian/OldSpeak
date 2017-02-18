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

static void
bf6p6_subkey_dealloc(BF6P6Subkey *self)
{
    self->subkey = NULL;
    Py_XDECREF(self->parent);
    self->parent = NULL;
    PyObject_Del(self);
}

static PyObject *
bf6p6_subkey_get_revoked(BF6P6Subkey *self)
{
    return PyBool_FromLong(self->subkey->revoked);
}

static PyObject *
bf6p6_subkey_get_expired(BF6P6Subkey *self)
{
    return PyBool_FromLong(self->subkey->expired);
}

static PyObject *
bf6p6_subkey_get_disabled(BF6P6Subkey *self)
{
    return PyBool_FromLong(self->subkey->disabled);
}

static PyObject *
bf6p6_subkey_get_invalid(BF6P6Subkey *self)
{
    return PyBool_FromLong(self->subkey->invalid);
}

static PyObject *
bf6p6_subkey_get_can_encrypt(BF6P6Subkey *self)
{
    return PyBool_FromLong(self->subkey->can_encrypt);
}

static PyObject *
bf6p6_subkey_get_can_sign(BF6P6Subkey *self)
{
    return PyBool_FromLong(self->subkey->can_sign);
}

static PyObject *
bf6p6_subkey_get_can_certify(BF6P6Subkey *self)
{
    return PyBool_FromLong(self->subkey->can_certify);
}

static PyObject *
bf6p6_subkey_get_secret(BF6P6Subkey *self)
{
    return PyBool_FromLong(self->subkey->secret);
}

static PyObject *
bf6p6_subkey_get_can_authenticate(BF6P6Subkey *self)
{
    return PyBool_FromLong(self->subkey->can_authenticate);
}

static PyObject *
bf6p6_subkey_get_pubkey_algo(BF6P6Subkey *self)
{
    return PyInt_FromLong(self->subkey->pubkey_algo);
}

static PyObject *
bf6p6_subkey_get_length(BF6P6Subkey *self)
{
    return PyInt_FromLong(self->subkey->length);
}

static PyObject *
bf6p6_subkey_get_keyid(BF6P6Subkey *self)
{
    if (self->subkey->keyid)
        return PyUnicode_DecodeASCII(self->subkey->keyid,
                                     strlen(self->subkey->keyid), "replace");
    else
        Py_RETURN_NONE;
}

static PyObject *
bf6p6_subkey_get_fpr(BF6P6Subkey *self)
{
    if (self->subkey->fpr)
        return PyUnicode_DecodeASCII(self->subkey->fpr,
                                     strlen(self->subkey->fpr), "replace");
    else
        Py_RETURN_NONE;
}

static PyObject *
bf6p6_subkey_get_timestamp(BF6P6Subkey *self)
{
    return PyInt_FromLong(self->subkey->timestamp);
}

static PyObject *
bf6p6_subkey_get_expires(BF6P6Subkey *self)
{
    return PyInt_FromLong(self->subkey->expires);
}

static PyGetSetDef bf6p6_subkey_getsets[] = {
    { "revoked", (getter)bf6p6_subkey_get_revoked },
    { "expired", (getter)bf6p6_subkey_get_expired },
    { "disabled", (getter)bf6p6_subkey_get_disabled },
    { "invalid", (getter)bf6p6_subkey_get_invalid },
    { "can_encrypt", (getter)bf6p6_subkey_get_can_encrypt },
    { "can_sign", (getter)bf6p6_subkey_get_can_sign },
    { "can_certify", (getter)bf6p6_subkey_get_can_certify },
    { "secret", (getter)bf6p6_subkey_get_secret },
    { "can_authenticate", (getter)bf6p6_subkey_get_can_authenticate },
    { "pubkey_algo", (getter)bf6p6_subkey_get_pubkey_algo },
    { "length", (getter)bf6p6_subkey_get_length },
    { "keyid", (getter)bf6p6_subkey_get_keyid },
    { "fpr", (getter)bf6p6_subkey_get_fpr },
    { "timestamp", (getter)bf6p6_subkey_get_timestamp },
    { "expires", (getter)bf6p6_subkey_get_expires },
    { NULL, (getter)0, (setter)0 }
};

PyTypeObject BF6P6Subkey_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "bf6p6.Subkey",
    sizeof(BF6P6Subkey),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_init = bf6p6_no_constructor,
    .tp_dealloc = (destructor)bf6p6_subkey_dealloc,
    .tp_getset = bf6p6_subkey_getsets,
};

static void
bf6p6_key_sig_dealloc(BF6P6KeySig *self)
{
    self->key_sig = NULL;
    Py_XDECREF(self->parent);
    self->parent = NULL;
    PyObject_Del(self);
}

static PyObject *
bf6p6_key_sig_get_revoked(BF6P6KeySig *self)
{
    return PyBool_FromLong(self->key_sig->revoked);
}

static PyObject *
bf6p6_key_sig_get_expired(BF6P6KeySig *self)
{
    return PyBool_FromLong(self->key_sig->expired);
}

static PyObject *
bf6p6_key_sig_get_invalid(BF6P6KeySig *self)
{
    return PyBool_FromLong(self->key_sig->invalid);
}

static PyObject *
bf6p6_key_sig_get_exportable(BF6P6KeySig *self)
{
    return PyBool_FromLong(self->key_sig->exportable);
}

static PyObject *
bf6p6_key_sig_get_pubkey_algo(BF6P6KeySig *self)
{
    return PyInt_FromLong(self->key_sig->pubkey_algo);
}

static PyObject *
bf6p6_key_sig_get_keyid(BF6P6KeySig *self)
{
    if (self->key_sig->keyid)
        return PyUnicode_DecodeASCII(self->key_sig->keyid,
                                     strlen(self->key_sig->keyid), "replace");
    else
        Py_RETURN_NONE;
}

static PyObject *
bf6p6_key_sig_get_timestamp(BF6P6KeySig *self)
{
    return PyInt_FromLong(self->key_sig->timestamp);
}

static PyObject *
bf6p6_key_sig_get_expires(BF6P6KeySig *self)
{
    return PyInt_FromLong(self->key_sig->expires);
}

static PyObject *
bf6p6_key_sig_get_status(BF6P6KeySig *self)
{
    return bf6p6_error_object(self->key_sig->status);
}

static PyObject *
bf6p6_key_sig_get_uid(BF6P6KeySig *self)
{
    if (self->key_sig->uid)
        return PyUnicode_DecodeUTF8(self->key_sig->uid,
                                    strlen(self->key_sig->uid), "replace");
    else
        Py_RETURN_NONE;
}

static PyObject *
bf6p6_key_sig_get_name(BF6P6KeySig *self)
{
    if (self->key_sig->name)
        return PyUnicode_DecodeUTF8(self->key_sig->name,
                                    strlen(self->key_sig->name), "replace");
    else
        Py_RETURN_NONE;
}

static PyObject *
bf6p6_key_sig_get_email(BF6P6KeySig *self)
{
    if (self->key_sig->email)
        return PyUnicode_DecodeUTF8(self->key_sig->email,
                                    strlen(self->key_sig->email), "replace");
    else
        Py_RETURN_NONE;
}

static PyObject *
bf6p6_key_sig_get_comment(BF6P6KeySig *self)
{
    if (self->key_sig->comment)
        return PyUnicode_DecodeUTF8(self->key_sig->comment,
                                    strlen(self->key_sig->comment), "replace");
    else
        Py_RETURN_NONE;
}

static PyObject *
bf6p6_key_sig_get_sig_class(BF6P6KeySig *self)
{
    return PyInt_FromLong(self->key_sig->sig_class);
}

static PyGetSetDef bf6p6_key_sig_getsets[] = {
    { "revoked", (getter)bf6p6_key_sig_get_revoked },
    { "expired", (getter)bf6p6_key_sig_get_expired },
    { "invalid", (getter)bf6p6_key_sig_get_invalid },
    { "exportable", (getter)bf6p6_key_sig_get_exportable },
    { "pubkey_algo", (getter)bf6p6_key_sig_get_pubkey_algo },
    { "keyid", (getter)bf6p6_key_sig_get_keyid },
    { "timestamp", (getter)bf6p6_key_sig_get_timestamp },
    { "expires", (getter)bf6p6_key_sig_get_expires },
    { "status", (getter)bf6p6_key_sig_get_status },
    { "uid", (getter)bf6p6_key_sig_get_uid },
    { "name", (getter)bf6p6_key_sig_get_name },
    { "email", (getter)bf6p6_key_sig_get_email },
    { "comment", (getter)bf6p6_key_sig_get_comment },
    { "sig_class", (getter)bf6p6_key_sig_get_sig_class },
    { NULL, (getter)0, (setter)0 }
};

PyTypeObject BF6P6KeySig_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "bf6p6.KeySig",
    sizeof(BF6P6KeySig),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_init = bf6p6_no_constructor,
    .tp_dealloc = (destructor)bf6p6_key_sig_dealloc,
    .tp_getset = bf6p6_key_sig_getsets,
};

static void
bf6p6_user_id_dealloc(BF6P6UserId *self)
{
    self->user_id = NULL;
    Py_XDECREF(self->parent);
    self->parent = NULL;
    PyObject_Del(self);
}

static PyObject *
bf6p6_user_id_get_revoked(BF6P6UserId *self)
{
    return PyBool_FromLong(self->user_id->revoked);
}

static PyObject *
bf6p6_user_id_get_invalid(BF6P6UserId *self)
{
    return PyBool_FromLong(self->user_id->invalid);
}

static PyObject *
bf6p6_user_id_get_validity(BF6P6UserId *self)
{
    return PyInt_FromLong(self->user_id->validity);
}

static PyObject *
bf6p6_user_id_get_uid(BF6P6UserId *self)
{
    if (self->user_id->uid)
        return PyUnicode_DecodeUTF8(self->user_id->uid,
                                    strlen(self->user_id->uid), "replace");
    else
        Py_RETURN_NONE;
}

static PyObject *
bf6p6_user_id_get_name(BF6P6UserId *self)
{
    if (self->user_id->name)
        return PyUnicode_DecodeUTF8(self->user_id->name,
                                    strlen(self->user_id->name), "replace");
    else
        Py_RETURN_NONE;
}

static PyObject *
bf6p6_user_id_get_email(BF6P6UserId *self)
{
    if (self->user_id->email)
        return PyUnicode_DecodeUTF8(self->user_id->email,
                                    strlen(self->user_id->email), "replace");
    else
        Py_RETURN_NONE;
}

static PyObject *
bf6p6_user_id_get_comment(BF6P6UserId *self)
{
    if (self->user_id->comment)
        return PyUnicode_DecodeUTF8(self->user_id->comment,
                                    strlen(self->user_id->comment), "replace");
    else
        Py_RETURN_NONE;
}

static PyObject *
bf6p6_user_id_get_signatures(BF6P6UserId *self)
{
    PyObject *ret;
    gpgme_key_sig_t sig;

    ret = PyList_New(0);
    if (ret == NULL)
        return NULL;
    for (sig = self->user_id->signatures; sig != NULL; sig = sig->next) {
        BF6P6KeySig *item;

        item = PyObject_New(BF6P6KeySig, &BF6P6KeySig_Type);
        if (item == NULL) {
            Py_DECREF(ret);
            return NULL;
        }
        item->key_sig = sig;
        Py_INCREF(self);
        item->parent = (PyObject *)self;
        PyList_Append(ret, (PyObject *)item);
        Py_DECREF(item);
    }
    return ret;
}

static PyGetSetDef bf6p6_user_id_getsets[] = {
    { "revoked", (getter)bf6p6_user_id_get_revoked },
    { "invalid", (getter)bf6p6_user_id_get_invalid },
    { "validity", (getter)bf6p6_user_id_get_validity },
    { "uid", (getter)bf6p6_user_id_get_uid },
    { "name", (getter)bf6p6_user_id_get_name },
    { "email", (getter)bf6p6_user_id_get_email },
    { "comment", (getter)bf6p6_user_id_get_comment },
    { "signatures", (getter)bf6p6_user_id_get_signatures },
    { NULL, (getter)0, (setter)0 }
};

PyTypeObject BF6P6UserId_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "bf6p6.UserId",
    sizeof(BF6P6UserId),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_init = bf6p6_no_constructor,
    .tp_dealloc = (destructor)bf6p6_user_id_dealloc,
    .tp_getset = bf6p6_user_id_getsets,
};

static void
bf6p6_key_dealloc(BF6P6Key *self)
{
    gpgme_key_unref(self->key);
    self->key = NULL;
    PyObject_Del(self);
}

static PyObject *
bf6p6_key_get_revoked(BF6P6Key *self)
{
    return PyBool_FromLong(self->key->revoked);
}

static PyObject *
bf6p6_key_get_expired(BF6P6Key *self)
{
    return PyBool_FromLong(self->key->expired);
}

static PyObject *
bf6p6_key_get_disabled(BF6P6Key *self)
{
    return PyBool_FromLong(self->key->disabled);
}

static PyObject *
bf6p6_key_get_invalid(BF6P6Key *self)
{
    return PyBool_FromLong(self->key->invalid);
}

static PyObject *
bf6p6_key_get_can_encrypt(BF6P6Key *self)
{
    return PyBool_FromLong(self->key->can_encrypt);
}

static PyObject *
bf6p6_key_get_can_sign(BF6P6Key *self)
{
    return PyBool_FromLong(self->key->can_sign);
}

static PyObject *
bf6p6_key_get_can_certify(BF6P6Key *self)
{
    return PyBool_FromLong(self->key->can_certify);
}

static PyObject *
bf6p6_key_get_secret(BF6P6Key *self)
{
    return PyBool_FromLong(self->key->secret);
}

static PyObject *
bf6p6_key_get_can_authenticate(BF6P6Key *self)
{
    return PyBool_FromLong(self->key->can_authenticate);
}

static PyObject *
bf6p6_key_get_protocol(BF6P6Key *self)
{
    return PyInt_FromLong(self->key->protocol);
}

static PyObject *
bf6p6_key_get_issuer_serial(BF6P6Key *self)
{
    if (self->key->issuer_serial)
        /* Haven't tested this, so perhaps it should be UTF8 */
        return PyUnicode_DecodeASCII(self->key->issuer_serial,
                                     strlen(self->key->issuer_serial),
                                     "replace");
    else
        Py_RETURN_NONE;
}

static PyObject *
bf6p6_key_get_issuer_name(BF6P6Key *self)
{
    if (self->key->issuer_name)
        return PyUnicode_DecodeUTF8(self->key->issuer_name,
                                    strlen(self->key->issuer_name),
                                    "replace");
    else
        Py_RETURN_NONE;
}

static PyObject *
bf6p6_key_get_chain_id(BF6P6Key *self)
{
    if (self->key->chain_id)
        /* Haven't tested this, so perhaps it should be UTF8 */
        return PyUnicode_DecodeASCII(self->key->chain_id,
                                     strlen(self->key->chain_id), "replace");
    else
        Py_RETURN_NONE;
}

static PyObject *
bf6p6_key_get_owner_trust(BF6P6Key *self)
{
    return PyInt_FromLong(self->key->owner_trust);
}

static PyObject *
bf6p6_key_get_subkeys(BF6P6Key *self)
{
    PyObject *ret;
    gpgme_subkey_t subkey;

    ret = PyList_New(0);
    if (ret == NULL)
        return NULL;
    for (subkey = self->key->subkeys; subkey != NULL; subkey = subkey->next) {
        BF6P6Subkey *item;

        item = PyObject_New(BF6P6Subkey, &BF6P6Subkey_Type);
        if (item == NULL) {
            Py_DECREF(ret);
            return NULL;
        }
        item->subkey = subkey;
        Py_INCREF(self);
        item->parent = (PyObject *)self;
        PyList_Append(ret, (PyObject *)item);
        Py_DECREF(item);
    }
    return ret;
}

static PyObject *
bf6p6_key_get_uids(BF6P6Key *self)
{
    PyObject *ret;
    gpgme_user_id_t uid;

    ret = PyList_New(0);
    if (ret == NULL)
        return NULL;
    for (uid = self->key->uids; uid != NULL; uid = uid->next) {
        BF6P6UserId *item;

        item = PyObject_New(BF6P6UserId, &BF6P6UserId_Type);
        if (item == NULL) {
            Py_DECREF(ret);
            return NULL;
        }
        item->user_id = uid;
        Py_INCREF(self);
        item->parent = (PyObject *)self;
        PyList_Append(ret, (PyObject *)item);
        Py_DECREF(item);
    }
    return ret;
}

static PyObject *
bf6p6_key_get_keylist_mode(BF6P6Key *self)
{
    return PyInt_FromLong(self->key->keylist_mode);
}

static PyGetSetDef bf6p6_key_getsets[] = {
    { "revoked", (getter)bf6p6_key_get_revoked },
    { "expired", (getter)bf6p6_key_get_expired },
    { "disabled", (getter)bf6p6_key_get_disabled },
    { "invalid", (getter)bf6p6_key_get_invalid },
    { "can_encrypt", (getter)bf6p6_key_get_can_encrypt },
    { "can_sign", (getter)bf6p6_key_get_can_sign },
    { "can_certify", (getter)bf6p6_key_get_can_certify },
    { "secret", (getter)bf6p6_key_get_secret },
    { "can_authenticate", (getter)bf6p6_key_get_can_authenticate },
    { "protocol", (getter)bf6p6_key_get_protocol },
    { "issuer_serial", (getter)bf6p6_key_get_issuer_serial },
    { "issuer_name", (getter)bf6p6_key_get_issuer_name },
    { "chain_id", (getter)bf6p6_key_get_chain_id },
    { "owner_trust", (getter)bf6p6_key_get_owner_trust },
    { "subkeys", (getter)bf6p6_key_get_subkeys },
    { "uids", (getter)bf6p6_key_get_uids },
    { "keylist_mode", (getter)bf6p6_key_get_keylist_mode },
    { NULL, (getter)0, (setter)0 }
};

PyTypeObject BF6P6Key_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "bf6p6.Key",
    sizeof(BF6P6Key),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_init = bf6p6_no_constructor,
    .tp_dealloc = (destructor)bf6p6_key_dealloc,
    .tp_getset = bf6p6_key_getsets,
};

PyObject *
bf6p6_key_new(gpgme_key_t key)
{
    BF6P6Key *self;

    self = PyObject_New(BF6P6Key, &BF6P6Key_Type);
    if (self == NULL)
        return NULL;

    gpgme_key_ref(key);
    self->key = key;
    return (PyObject *)self;
}
