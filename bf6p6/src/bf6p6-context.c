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

static gpgme_error_t
bf6p6_passphrase_cb(void *hook, const char *uid_hint,
                      const char *passphrase_info, int prev_was_bad,
                      int fd)
{
    PyObject *callback, *ret;
    PyGILState_STATE state;
    gpgme_error_t err;

    state = PyGILState_Ensure();
    callback = (PyObject *)hook;
    ret = PyObject_CallFunction(callback, "zzii", uid_hint, passphrase_info,
                                prev_was_bad, fd);
    err = bf6p6_check_pyerror();
    Py_XDECREF(ret);
    PyGILState_Release(state);
    return err;
}

static void
bf6p6_progress_cb(void *hook, const char *what, int type,
                    int current, int total)
{
    PyObject *callback, *ret;
    PyGILState_STATE state;

    state = PyGILState_Ensure();
    callback = (PyObject *)hook;
    ret = PyObject_CallFunction(callback, "ziii", what, type, current, total);
    PyErr_Clear();
    Py_XDECREF(ret);
    PyGILState_Release(state);
}

static void
bf6p6_context_dealloc(BF6P6Context *self)
{
    gpgme_passphrase_cb_t passphrase_cb;
    gpgme_progress_cb_t progress_cb;
    PyObject *callback;

    if (self->ctx) {
        /* free the passphrase callback */
        gpgme_get_passphrase_cb(self->ctx, &passphrase_cb, (void **)&callback);
        if (passphrase_cb == bf6p6_passphrase_cb) {
            Py_DECREF(callback);
        }

        /* free the progress callback */
        gpgme_get_progress_cb(self->ctx, &progress_cb, (void **)&callback);
        if (progress_cb == bf6p6_progress_cb) {
            Py_DECREF(callback);
        }

        gpgme_release(self->ctx);
    }
    self->ctx = NULL;
    PyObject_Del(self);
}

static int
bf6p6_context_init(BF6P6Context *self, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = { NULL };

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "", kwlist))
        return -1;

    if (self->ctx != NULL) {
        PyErr_SetString(PyExc_ValueError, "context already initialised");
        return -1;
    }

    if (bf6p6_check_error(gpgme_new(&self->ctx)))
        return -1;

    gpgme_set_pinentry_mode(self->ctx, GPGME_PINENTRY_MODE_CANCEL);
    return 0;
}

static const char bf6p6_context_protocol_doc[] =
    "The encryption system for this context (one of the PROTOCOL_* constants).";

static PyObject *
bf6p6_context_get_protocol(BF6P6Context *self)
{
    return PyInt_FromLong(gpgme_get_protocol(self->ctx));
}

static int
bf6p6_context_set_protocol(BF6P6Context *self, PyObject *value)
{
    gpgme_protocol_t protocol;

    if (value == NULL) {
        PyErr_SetString(PyExc_AttributeError, "Can not delete attribute");
        return -1;
    }

    protocol = PyInt_AsLong(value);
    if (PyErr_Occurred())
        return -1;

    if (bf6p6_check_error(gpgme_set_protocol(self->ctx, protocol)))
        return -1;

    return 0;
}

static const char bf6p6_context_armor_doc[] =
    "Whether encrypted data should be ASCII-armored.";

static PyObject *
bf6p6_context_get_armor(BF6P6Context *self)
{
    return PyBool_FromLong(gpgme_get_armor(self->ctx));
}

static int
bf6p6_context_set_armor(BF6P6Context *self, PyObject *value)
{
    int armor;

    if (value == NULL) {
        PyErr_SetString(PyExc_AttributeError, "Can not delete attribute");
        return -1;
    }

    armor = PyInt_AsLong(value) != 0;
    if (PyErr_Occurred())
        return -1;

    gpgme_set_armor(self->ctx, armor);
    return 0;
}

static const char bf6p6_context_textmode_doc[] =
    "Whether text mode is enabled.";

static PyObject *
bf6p6_context_get_textmode(BF6P6Context *self)
{
    return PyBool_FromLong(gpgme_get_textmode(self->ctx));
}

static int
bf6p6_context_set_textmode(BF6P6Context *self, PyObject *value)
{
    int textmode;

    if (value == NULL) {
        PyErr_SetString(PyExc_AttributeError, "Can not delete attribute");
        return -1;
    }

    textmode = PyInt_AsLong(value) != 0;
    if (PyErr_Occurred())
        return -1;

    gpgme_set_textmode(self->ctx, textmode);
    return 0;
}

static const char bf6p6_context_include_certs_doc[] =
    "How many certificates will be included in an S/MIME signed message.\n\n"
    "See GPGME docs for details.";

static PyObject *
bf6p6_context_get_include_certs(BF6P6Context *self)
{
    return PyInt_FromLong(gpgme_get_include_certs(self->ctx));
}

static int
bf6p6_context_set_include_certs(BF6P6Context *self, PyObject *value)
{
    int nr_of_certs;

    if (value == NULL) {
        PyErr_SetString(PyExc_AttributeError, "Can not delete attribute");
        return -1;
    }

    nr_of_certs = PyInt_AsLong(value);
    if (PyErr_Occurred())
        return -1;

    gpgme_set_include_certs(self->ctx, nr_of_certs);
    return 0;
}

static const char bf6p6_context_keylist_mode_doc[] =
    "Set to KEYLIST_MODE_* constants added together.\n\nSee GPGME docs "
    "for details.";

static PyObject *
bf6p6_context_get_keylist_mode(BF6P6Context *self)
{
    return PyInt_FromLong(gpgme_get_keylist_mode(self->ctx));
}

static int
bf6p6_context_set_keylist_mode(BF6P6Context *self, PyObject *value)
{
    gpgme_keylist_mode_t keylist_mode;

    if (value == NULL) {
        PyErr_SetString(PyExc_AttributeError, "Can not delete attribute");
        return -1;
    }

    keylist_mode = PyInt_AsLong(value);
    if (PyErr_Occurred())
        return -1;

    if (bf6p6_check_error(gpgme_set_keylist_mode(self->ctx, keylist_mode)))
        return -1;

    return 0;
}

static const char bf6p6_context_pinentry_mode_doc[] =
    "Set to PINENTRY_MODE_* constant.\n\nSee GPGME docs for details.";

static PyObject *
bf6p6_context_get_pinentry_mode(BF6P6Context *self)
{
#if GPGME_VERSION_NUMBER < 0x010400
    return PyInt_FromLong(GPGME_PINENTRY_MODE_LOOPBACK);
#else  /* gpgme >= 1.4.0 */
    return PyInt_FromLong(gpgme_get_pinentry_mode(self->ctx));
#endif /* gpgme >= 1.4.0 */
}

static int
bf6p6_context_set_pinentry_mode(BF6P6Context *self, PyObject *value)
{
#if GPGME_VERSION_NUMBER < 0x010400
    PyErr_SetString(PyExc_AttributeError,
                    "Not supported by this version of GPGME");
    return -1;
#else /* gpgme >= 1.4.0 */
    gpgme_pinentry_mode_t pinentry_mode;

    if (value == NULL) {
        PyErr_SetString(PyExc_AttributeError, "Can not delete attribute");
        return -1;
    }

    pinentry_mode = PyInt_AsLong(value);
    if (PyErr_Occurred())
        return -1;

    if (bf6p6_check_error(gpgme_set_pinentry_mode(self->ctx, pinentry_mode)))
        return -1;

    return 0;
#endif /* gpgme >= 1.4.0 */
}

static const char bf6p6_context_passphrase_cb_doc[] =
    "A callback that will get a passphrase from the user.\n\n"
    "The callable must have the following signature:\n\n"
    "    callback(uidHint, passphraseInfo, prevWasBad, fd)\n\n"
      "uidHint: a string describing the key whose passphrase is needed, or\n"
    "  None.\n\n"
    "passphraseInfo: a string containing more information about the\n"
    "  required passphrase, or None.\n\n"
    "prevWasBad: If the user gave a bad passphrase and we're asking again,\n"
    "  this will be 1, otherwise 0.\n\n"
    "fd: A numeric file-descriptor, as returned by os.open().\n\n"
    "The callback is required to prompt the user for a passphrase, then\n"
    "write the passphrase followed by a '\\n' to the file-descriptor fd\n"
    "using os.write(). If the user indicates they wish to cancel the\n"
    "operation, you should raise a gpgme.BF6P6Error whose .code attribute\n"
    "is set to ERR_CANCELED.";

static PyObject *
bf6p6_context_get_passphrase_cb(BF6P6Context *self)
{
    gpgme_passphrase_cb_t passphrase_cb;
    PyObject *callback;

    /* free the passphrase callback */
    gpgme_get_passphrase_cb(self->ctx, &passphrase_cb, (void **)&callback);
    if (passphrase_cb == bf6p6_passphrase_cb) {
        Py_INCREF(callback);
        return callback;
    } else {
        Py_RETURN_NONE;
    }
}

static int
bf6p6_context_set_passphrase_cb(BF6P6Context *self, PyObject *value)
{
    gpgme_passphrase_cb_t passphrase_cb;
    PyObject *callback;

    /* free the passphrase callback */
    gpgme_get_passphrase_cb(self->ctx, &passphrase_cb, (void **)&callback);
    if (passphrase_cb == bf6p6_passphrase_cb) {
        Py_DECREF(callback);
    }

    /* callback of None == unset */
    if (value == Py_None)
        value = NULL;

    if (value != NULL) {
        Py_INCREF(value);
        gpgme_set_passphrase_cb(self->ctx, bf6p6_passphrase_cb, value);
    } else {
        gpgme_set_passphrase_cb(self->ctx, NULL, NULL);
    }
    return 0;
}

static PyObject *
bf6p6_context_get_progress_cb(BF6P6Context *self)
{
    gpgme_progress_cb_t progress_cb;
    PyObject *callback;

    /* free the progress callback */
    gpgme_get_progress_cb(self->ctx, &progress_cb, (void **)&callback);
    if (progress_cb == bf6p6_progress_cb) {
        Py_INCREF(callback);
        return callback;
    } else {
        Py_RETURN_NONE;
    }
}

static int
bf6p6_context_set_progress_cb(BF6P6Context *self, PyObject *value)
{
    gpgme_progress_cb_t progress_cb;
    PyObject *callback;

    /* free the progress callback */
    gpgme_get_progress_cb(self->ctx, &progress_cb, (void **)&callback);
    if (progress_cb == bf6p6_progress_cb) {
        Py_DECREF(callback);
    }

    /* callback of None == unset */
    if (value == Py_None)
        value = NULL;

    if (value != NULL) {
        Py_INCREF(value);
        gpgme_set_progress_cb(self->ctx, bf6p6_progress_cb, value);
    } else {
        gpgme_set_progress_cb(self->ctx, NULL, NULL);
    }
    return 0;
}

static const char bf6p6_context_signers_doc[] =
    "A list of Key objects, used to sign data with the .sign() method.";

static PyObject *
bf6p6_context_get_signers(BF6P6Context *self)
{
    PyObject *list, *tuple;
    gpgme_key_t key;
    int i;

    list = PyList_New(0);
    for (i = 0, key = gpgme_signers_enum(self->ctx, 0);
         key != NULL; key = gpgme_signers_enum(self->ctx, ++i)) {
        PyObject *item;

        item = bf6p6_key_new(key);
        gpgme_key_unref(key);
        if (item == NULL) {
            Py_DECREF(list);
            return NULL;
        }
        PyList_Append(list, item);
        Py_DECREF(item);
    }
    tuple = PySequence_Tuple(list);
    Py_DECREF(list);
    return tuple;
}

static int
bf6p6_context_set_signers(BF6P6Context *self, PyObject *value)
{
    PyObject *signers = NULL;
    int i, length, ret = 0;

    if (value == NULL) {
        PyErr_SetString(PyExc_AttributeError, "Can not delete attribute");
        return -1;
    }

    signers = PySequence_Fast(value, "signers must be a sequence of keys");
    if (!signers) {
        ret = -1;
        goto end;
    }

    gpgme_signers_clear(self->ctx);
    length = PySequence_Fast_GET_SIZE(signers);
    for (i = 0; i < length; i++) {
        PyObject *item = PySequence_Fast_GET_ITEM(signers, i);

        if (!PyObject_TypeCheck(item, &BF6P6Key_Type)) {
            PyErr_SetString(PyExc_TypeError,
                            "signers must be a sequence of keys");
            ret = -1;
            goto end;
        }
        gpgme_signers_add(self->ctx, ((BF6P6Key *)item)->key);
    }

 end:
    Py_XDECREF(signers);
    return ret;
}

static PyGetSetDef bf6p6_context_getsets[] = {
    { "protocol", (getter)bf6p6_context_get_protocol,
      (setter)bf6p6_context_set_protocol,
      (char *)bf6p6_context_protocol_doc },
    { "armor", (getter)bf6p6_context_get_armor,
      (setter)bf6p6_context_set_armor,
      (char *)bf6p6_context_armor_doc },
    { "textmode", (getter)bf6p6_context_get_textmode,
      (setter)bf6p6_context_set_textmode,
      (char *)bf6p6_context_textmode_doc },
    { "include_certs", (getter)bf6p6_context_get_include_certs,
      (setter)bf6p6_context_set_include_certs,
      (char *)bf6p6_context_include_certs_doc },
    { "keylist_mode", (getter)bf6p6_context_get_keylist_mode,
      (setter)bf6p6_context_set_keylist_mode,
      (char *)bf6p6_context_keylist_mode_doc },
    { "pinentry_mode", (getter)bf6p6_context_get_pinentry_mode,
      (setter)bf6p6_context_set_pinentry_mode,
      (char *)bf6p6_context_pinentry_mode_doc },
    { "passphrase_cb", (getter)bf6p6_context_get_passphrase_cb,
      (setter)bf6p6_context_set_passphrase_cb,
      (char *)bf6p6_context_passphrase_cb_doc },
    { "progress_cb", (getter)bf6p6_context_get_progress_cb,
      (setter)bf6p6_context_set_progress_cb },
    { "signers", (getter)bf6p6_context_get_signers,
      (setter)bf6p6_context_set_signers,
      (char *)bf6p6_context_signers_doc },
    { NULL, (getter)0, (setter)0 }
};

static PyObject *
bf6p6_context_set_engine_info(BF6P6Context *self, PyObject *args)
{
    int protocol;
    const char *file_name, *home_dir;

    if (!PyArg_ParseTuple(args, "izz", &protocol, &file_name, &home_dir))
        return NULL;

    if (bf6p6_check_error(gpgme_ctx_set_engine_info(self->ctx, protocol,
                                                      file_name, home_dir)))
        return NULL;

    Py_RETURN_NONE;
}

static PyObject *
bf6p6_context_set_locale(BF6P6Context *self, PyObject *args)
{
    int category;
    const char *value;

    if (!PyArg_ParseTuple(args, "iz", &category, &value))
        return NULL;

    if (bf6p6_check_error(gpgme_set_locale(self->ctx, category, value)))
        return NULL;

    Py_RETURN_NONE;
}

static const char bf6p6_context_get_key_doc[] =
    "get_key(fingerprint[, secret]) -> Key instance\n\n"
    "Finds a key with the given fingerprint (a string of hex digits) in\n"
    "the user's keyring. If secret is 1, only private keys will be\n"
    "returned.\n\nIf no key can be found, raises BF6P6Error.";

static PyObject *
bf6p6_context_get_key(BF6P6Context *self, PyObject *args)
{
    const char *fpr;
    int secret = 0;
    gpgme_error_t err;
    gpgme_key_t key;
    PyObject *ret;

    if (!PyArg_ParseTuple(args, "s|i", &fpr, &secret))
        return NULL;

    Py_BEGIN_ALLOW_THREADS;
    err = gpgme_get_key(self->ctx, fpr, &key, secret);
    Py_END_ALLOW_THREADS;

    if (bf6p6_check_error(err))
        return NULL;

    ret = bf6p6_key_new(key);
    gpgme_key_unref(key);
    return ret;
}

/* XXX: cancel -- not needed unless we wrap the async calls */

/* annotate exception with encrypt_result data */
static void
decode_encrypt_result(BF6P6Context *self)
{
    PyObject *err_type, *err_value, *err_traceback;
    gpgme_encrypt_result_t res;
    gpgme_invalid_key_t key;
    PyObject *list;

    PyErr_Fetch(&err_type, &err_value, &err_traceback);
    PyErr_NormalizeException(&err_type, &err_value, &err_traceback);

    if (!PyErr_GivenExceptionMatches(err_type, bf6p6_error))
        goto end;

    res = gpgme_op_encrypt_result(self->ctx);
    if (res == NULL)
        goto end;

    list = PyList_New(0);
    for (key = res->invalid_recipients; key != NULL; key = key->next) {
        PyObject *item, *py_fpr, *err;

        if (key->fpr)
            py_fpr = PyUnicode_DecodeASCII(key->fpr, strlen(key->fpr),
                                           "replace");
        else {
            py_fpr = Py_None;
            Py_INCREF(py_fpr);
        }
        err = bf6p6_error_object(key->reason);
        item = Py_BuildValue("(NN)", py_fpr, err);
        PyList_Append(list, item);
        Py_DECREF(item);
    }

    PyObject_SetAttrString(err_value, "invalid_recipients", list);
    Py_DECREF(list);

 end:
    PyErr_Restore(err_type, err_value, err_traceback);
}

static const char bf6p6_context_encrypt_doc[] =
    "encrypt(recipients, flags, plaintext, ciphertext)\n\n"
    "Encrypts plaintext so it can only be read by the given recipients.\n\n"
    "recipients: A list of Key objects. Only people in posession of the\n"
    "  corresponding private key (for public key encryption) or passphrase\n"
    "  (for symmetric encryption) will be able to decrypt the result.\n\n"
    "flags: ENCRYPT_* constants added together. See GPGME docs for\n"
    "  details.\n\n"
    "plaintext: A file-like object opened for reading, containing the data\n"
    "  to be encrypted.\n\n"
    "ciphertext: A file-like object opened for writing, where the\n"
    "  encrypted data will be written. If the Context's .armor property is\n"
    "  False, this file should be opened in binary mode.";

static PyObject *
bf6p6_context_encrypt(BF6P6Context *self, PyObject *args)
{
    PyObject *py_recp, *py_plain, *py_cipher, *recp_seq = NULL, *result = NULL;
    int flags, i, length;
    gpgme_key_t *recp = NULL;
    gpgme_data_t plain = NULL, cipher = NULL;
    gpgme_error_t err;

    if (!PyArg_ParseTuple(args, "OiOO", &py_recp, &flags,
                          &py_plain, &py_cipher))
        goto end;

    if (py_recp != Py_None) {
        recp_seq = PySequence_Fast(py_recp, "first argument must be a "
                                   "sequence or None");
        if (recp_seq == NULL)
            goto end;

        length = PySequence_Fast_GET_SIZE(recp_seq);
        recp = malloc((length + 1) * sizeof (gpgme_key_t));
        for (i = 0; i < length; i++) {
            PyObject *item = PySequence_Fast_GET_ITEM(recp_seq, i);

            if (!PyObject_TypeCheck(item, &BF6P6Key_Type)) {
                PyErr_SetString(PyExc_TypeError, "items in first argument "
                                "must be gpgme.Key objects");
                goto end;
            }
            recp[i] = ((BF6P6Key *)item)->key;
        }
        recp[i] = NULL;
    }

    if (bf6p6_data_new(&plain, py_plain))
        goto end;
    if (bf6p6_data_new(&cipher, py_cipher))
        goto end;

    Py_BEGIN_ALLOW_THREADS;    err = gpgme_op_encrypt(self->ctx, recp, flags, plain, cipher);
    Py_END_ALLOW_THREADS;

    if (bf6p6_check_error(err)) {
        decode_encrypt_result(self);
        goto end;
    }

    Py_INCREF(Py_None);
    result = Py_None;

 end:
    if (recp != NULL)
        free(recp);
    Py_XDECREF(recp_seq);
    if (plain != NULL)
        gpgme_data_release(plain);
    if (cipher != NULL)
        gpgme_data_release(cipher);

    return result;
}

static PyObject *
bf6p6_context_encrypt_sign(BF6P6Context *self, PyObject *args)
{
    PyObject *py_recp, *py_plain, *py_cipher, *recp_seq = NULL, *result = NULL;
    int flags, i, length;
    gpgme_key_t *recp = NULL;
    gpgme_data_t plain = NULL, cipher = NULL;
    gpgme_error_t err;
    gpgme_sign_result_t sign_result;

    if (!PyArg_ParseTuple(args, "OiOO", &py_recp, &flags,
                          &py_plain, &py_cipher))
        goto end;

    recp_seq = PySequence_Fast(py_recp, "first argument must be a sequence");
    if (recp_seq == NULL)
        goto end;

    length = PySequence_Fast_GET_SIZE(recp_seq);
    recp = malloc((length + 1) * sizeof (gpgme_key_t));
    for (i = 0; i < length; i++) {
        PyObject *item = PySequence_Fast_GET_ITEM(recp_seq, i);

        if (!PyObject_TypeCheck(item, &BF6P6Key_Type)) {
            PyErr_SetString(PyExc_TypeError, "items in first argument "
                            "must be gpgme.Key objects");
            goto end;
        }
        recp[i] = ((BF6P6Key *)item)->key;
    }
    recp[i] = NULL;

    if (bf6p6_data_new(&plain, py_plain))
        goto end;
    if (bf6p6_data_new(&cipher, py_cipher))
        goto end;

    Py_BEGIN_ALLOW_THREADS;
    err = gpgme_op_encrypt_sign(self->ctx, recp, flags, plain, cipher);
    Py_END_ALLOW_THREADS;

    sign_result = gpgme_op_sign_result(self->ctx);

    /* annotate exception */
    if (bf6p6_check_error(err)) {
        PyObject *err_type, *err_value, *err_traceback;
        PyObject *list;
        gpgme_invalid_key_t key;

        decode_encrypt_result(self);

        PyErr_Fetch(&err_type, &err_value, &err_traceback);
        PyErr_NormalizeException(&err_type, &err_value, &err_traceback);

        if (sign_result == NULL)
            goto error_end;

        if (!PyErr_GivenExceptionMatches(err_type, bf6p6_error))
            goto error_end;

        list = PyList_New(0);
        for (key = sign_result->invalid_signers; key != NULL; key = key->next) {
            PyObject *item, *py_fpr, *err;

            if (key->fpr)
                py_fpr = PyUnicode_DecodeASCII(key->fpr, strlen(key->fpr),
                                               "replace");
            else {
                py_fpr = Py_None;
                Py_INCREF(py_fpr);
            }
            err = bf6p6_error_object(key->reason);
            item = Py_BuildValue("(NN)", py_fpr, err);
            PyList_Append(list, item);
            Py_DECREF(item);
        }
        PyObject_SetAttrString(err_value, "invalid_signers", list);
        Py_DECREF(list);

        list = bf6p6_newsiglist_new(sign_result->signatures);
        PyObject_SetAttrString(err_value, "signatures", list);
        Py_DECREF(list);
    error_end:
        PyErr_Restore(err_type, err_value, err_traceback);
        goto end;
    }

    if (sign_result)
        result = bf6p6_newsiglist_new(sign_result->signatures);
    else
        result = PyList_New(0);

 end:
    if (recp != NULL)
        free(recp);
    Py_XDECREF(recp_seq);
    if (plain != NULL)
        gpgme_data_release(plain);
    if (cipher != NULL)
        gpgme_data_release(cipher);

    return result;
}

static void
decode_decrypt_result(BF6P6Context *self)
{
    PyObject *err_type, *err_value, *err_traceback;
    PyObject *value;
    gpgme_decrypt_result_t res;

    PyErr_Fetch(&err_type, &err_value, &err_traceback);
    PyErr_NormalizeException(&err_type, &err_value, &err_traceback);

    if (!PyErr_GivenExceptionMatches(err_type, bf6p6_error))
        goto end;

    res = gpgme_op_decrypt_result(self->ctx);
    if (res == NULL)
        goto end;

    if (res->unsupported_algorithm) {
        value = PyUnicode_DecodeUTF8(res->unsupported_algorithm,
                                     strlen(res->unsupported_algorithm),
                                     "replace");
    } else {
        Py_INCREF(Py_None);
        value = Py_None;
    }
    if (value) {
        PyObject_SetAttrString(err_value, "unsupported_algorithm", value);
        Py_DECREF(value);
    }

    value = PyBool_FromLong(res->wrong_key_usage);
    if (value) {
        PyObject_SetAttrString(err_value, "wrong_key_usage", value);
        Py_DECREF(value);
    }

 end:
    PyErr_Restore(err_type, err_value, err_traceback);
}

static const char bf6p6_context_decrypt_doc[] =
    "decrypt(ciphertext, plaintext)\n\n"
    "Decrypts the ciphertext and writes out the plaintext.\n\n"
    "ciphertext: A file-like object opened for reading, containing the\n"
    "  encrypted data.\n\n"
    "plaintext: A file-like object opened for writing, where the decrypted\n"
    "  data will be written.\n\n"
    "To decrypt data, you must have one of the recipients' private keys in\n"
    "your keyring (for public key encryption) or the passphrase (for \n"
    "symmetric encryption). If gpg finds the key but needs a passphrase to\n"
    "unlock it, the .passphrase_cb callback will be used to ask for it.";

static PyObject *
bf6p6_context_decrypt(BF6P6Context *self, PyObject *args)
{
    PyObject *py_cipher, *py_plain;
    gpgme_data_t cipher, plain;
    gpgme_error_t err;

    if (!PyArg_ParseTuple(args, "OO", &py_cipher, &py_plain))
        return NULL;

    if (bf6p6_data_new(&cipher, py_cipher)) {
        return NULL;
    }

    if (bf6p6_data_new(&plain, py_plain)) {
        gpgme_data_release(cipher);
        return NULL;
    }

    Py_BEGIN_ALLOW_THREADS;
    err = gpgme_op_decrypt(self->ctx, cipher, plain);
    Py_END_ALLOW_THREADS;

    gpgme_data_release(cipher);
    gpgme_data_release(plain);

    if (bf6p6_check_error(err)) {
        decode_decrypt_result(self);
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *
bf6p6_context_decrypt_verify(BF6P6Context *self, PyObject *args)
{
    PyObject *py_cipher, *py_plain;
    gpgme_data_t cipher, plain;
    gpgme_error_t err;
    gpgme_verify_result_t result;

    if (!PyArg_ParseTuple(args, "OO", &py_cipher, &py_plain))
        return NULL;

    if (bf6p6_data_new(&cipher, py_cipher)) {
        return NULL;
    }

    if (bf6p6_data_new(&plain, py_plain)) {
        gpgme_data_release(cipher);
        return NULL;
    }

    Py_BEGIN_ALLOW_THREADS;
    err = gpgme_op_decrypt_verify(self->ctx, cipher, plain);
    Py_END_ALLOW_THREADS;

    gpgme_data_release(cipher);
    gpgme_data_release(plain);

    if (bf6p6_check_error(err)) {
        decode_decrypt_result(self);
        return NULL;
    }

    result = gpgme_op_verify_result(self->ctx);

    /* annotate exception */
    if (bf6p6_check_error(err)) {
        PyObject *err_type, *err_value, *err_traceback;
        PyObject *list;

        PyErr_Fetch(&err_type, &err_value, &err_traceback);
        PyErr_NormalizeException(&err_type, &err_value, &err_traceback);

        if (result == NULL)
            goto end;

        if (!PyErr_GivenExceptionMatches(err_type, bf6p6_error))
            goto end;

        list = bf6p6_siglist_new(result->signatures);
        PyObject_SetAttrString(err_value, "signatures", list);
        Py_DECREF(list);
    end:
        PyErr_Restore(err_type, err_value, err_traceback);
        return NULL;
    }

    if (result)
        return bf6p6_siglist_new(result->signatures);
    else
        return PyList_New(0);
}

static PyObject *
bf6p6_context_sign(BF6P6Context *self, PyObject *args)
{
    PyObject *py_plain, *py_sig;
    gpgme_data_t plain, sig;
    int sig_mode = GPGME_SIG_MODE_NORMAL;
    gpgme_error_t err;
    gpgme_sign_result_t result;

    if (!PyArg_ParseTuple(args, "OO|i", &py_plain, &py_sig, &sig_mode))
        return NULL;

    if (bf6p6_data_new(&plain, py_plain))
        return NULL;

    if (bf6p6_data_new(&sig, py_sig)) {
        gpgme_data_release(plain);
        return NULL;
    }

    Py_BEGIN_ALLOW_THREADS;
    err = gpgme_op_sign(self->ctx, plain, sig, sig_mode);
    Py_END_ALLOW_THREADS;

    gpgme_data_release(plain);
    gpgme_data_release(sig);

    result = gpgme_op_sign_result(self->ctx);

    /* annotate exception */
    if (bf6p6_check_error(err)) {
        PyObject *err_type, *err_value, *err_traceback;
        PyObject *list;
        gpgme_invalid_key_t key;

        PyErr_Fetch(&err_type, &err_value, &err_traceback);
        PyErr_NormalizeException(&err_type, &err_value, &err_traceback);

        if (result == NULL)
            goto end;

        if (!PyErr_GivenExceptionMatches(err_type, bf6p6_error))
            goto end;

        list = PyList_New(0);
        for (key = result->invalid_signers; key != NULL; key = key->next) {
            PyObject *item, *py_fpr, *err;

            if (key->fpr)
                py_fpr = PyUnicode_DecodeASCII(key->fpr, strlen(key->fpr),
                                               "replace");
            else {
                py_fpr = Py_None;
                Py_INCREF(py_fpr);
            }
            err = bf6p6_error_object(key->reason);
            item = Py_BuildValue("(NN)", py_fpr, err);
            PyList_Append(list, item);
            Py_DECREF(item);
        }
        PyObject_SetAttrString(err_value, "invalid_signers", list);
        Py_DECREF(list);

        list = bf6p6_newsiglist_new(result->signatures);
        PyObject_SetAttrString(err_value, "signatures", list);
        Py_DECREF(list);
    end:
        PyErr_Restore(err_type, err_value, err_traceback);
        return NULL;
    }

    if (result)
        return bf6p6_newsiglist_new(result->signatures);
    else
        return PyList_New(0);
}

static PyObject *
bf6p6_context_verify(BF6P6Context *self, PyObject *args)
{
    PyObject *py_sig, *py_signed_text, *py_plaintext;
    gpgme_data_t sig, signed_text, plaintext;
    gpgme_error_t err;
    gpgme_verify_result_t result;

    if (!PyArg_ParseTuple(args, "OOO", &py_sig, &py_signed_text,
                          &py_plaintext))
        return NULL;

    if (bf6p6_data_new(&sig, py_sig)) {
        return NULL;
    }
    if (bf6p6_data_new(&signed_text, py_signed_text)) {
        gpgme_data_release(sig);
        return NULL;
    }
    if (bf6p6_data_new(&plaintext, py_plaintext)) {
        gpgme_data_release(sig);
        gpgme_data_release(signed_text);
        return NULL;
    }

    Py_BEGIN_ALLOW_THREADS;
    err = gpgme_op_verify(self->ctx, sig, signed_text, plaintext);
    Py_END_ALLOW_THREADS;

    gpgme_data_release(sig);
    gpgme_data_release(signed_text);
    gpgme_data_release(plaintext);

    result = gpgme_op_verify_result(self->ctx);

    /* annotate exception */
    if (bf6p6_check_error(err)) {
        PyObject *err_type, *err_value, *err_traceback;
        PyObject *list;

        PyErr_Fetch(&err_type, &err_value, &err_traceback);
        PyErr_NormalizeException(&err_type, &err_value, &err_traceback);

        if (result == NULL)
            goto end;

        if (!PyErr_GivenExceptionMatches(err_type, bf6p6_error))
            goto end;

        list = bf6p6_siglist_new(result->signatures);
        PyObject_SetAttrString(err_value, "signatures", list);
        Py_DECREF(list);
    end:
        PyErr_Restore(err_type, err_value, err_traceback);
        return NULL;
    }

    if (result)
        return bf6p6_siglist_new(result->signatures);
    else
        return PyList_New(0);
}

static PyObject *
bf6p6_context_import(BF6P6Context *self, PyObject *args)
{
    PyObject *py_keydata, *result;
    gpgme_data_t keydata;
    gpgme_error_t err;

    if (!PyArg_ParseTuple(args, "O", &py_keydata))
        return NULL;

    if (bf6p6_data_new(&keydata, py_keydata))
        return NULL;

    Py_BEGIN_ALLOW_THREADS;
    err = gpgme_op_import(self->ctx, keydata);
    Py_END_ALLOW_THREADS;

    gpgme_data_release(keydata);
    result = bf6p6_import_result(self->ctx);
    if (bf6p6_check_error(err)) {
        PyObject *err_type, *err_value, *err_traceback;

        PyErr_Fetch(&err_type, &err_value, &err_traceback);
        PyErr_NormalizeException(&err_type, &err_value, &err_traceback);

        if (!PyErr_GivenExceptionMatches(err_type, bf6p6_error))
            goto end;

        if (result != NULL) {
            PyObject_SetAttrString(err_value, "result", result);
            Py_DECREF(result);
        }
    end:
        PyErr_Restore(err_type, err_value, err_traceback);
        return NULL;
    }
    return result;
}

static void
free_key_patterns(char **patterns) {
    int i;

    for (i = 0; patterns[i] != NULL; i++) {
        free(patterns[i]);
    }
    free(patterns);
}

/* This function should probably be changed to not accept bytes() when
 * Python 2.x support is dropped. */
static int
parse_key_patterns(PyObject *py_pattern, char ***patterns)
{
    int result = -1, length, i;
    PyObject *list = NULL;

    *patterns = NULL;
    if (py_pattern == Py_None) {
        result = 0;
    } else if (PyUnicode_Check(py_pattern) || PyBytes_Check(py_pattern)) {
        PyObject *bytes;

        if (PyUnicode_Check(py_pattern)) {
            bytes = PyUnicode_AsUTF8String(py_pattern);
            if (bytes == NULL)
                goto end;
        } else {
            bytes = py_pattern;
            Py_INCREF(bytes);
        }
        *patterns = calloc(2, sizeof (char *));
        if (*patterns == NULL) {
            PyErr_NoMemory();
            Py_DECREF(bytes);
            goto end;
        }
        (*patterns)[0] = strdup(PyBytes_AsString(bytes));
        if ((*patterns)[0] == NULL) {
            PyErr_NoMemory();
            Py_DECREF(bytes);
            goto end;
        }
        result = 0;
    } else {
        /* We must have a sequence of strings. */
        list = PySequence_Fast(py_pattern,
            "first argument must be a string or sequence of strings");
        if (list == NULL)
            goto end;

        length = PySequence_Fast_GET_SIZE(list);
        *patterns = calloc((length + 1), sizeof(char *));
        if (*patterns == NULL) {
            PyErr_NoMemory();
            goto end;
        }
        for (i = 0; i < length; i++) {
            PyObject *item = PySequence_Fast_GET_ITEM(list, i);
            PyObject *bytes;

            if (PyBytes_Check(item)) {
                bytes = item;
                Py_INCREF(bytes);
            } else if (PyUnicode_Check(item)) {
                bytes = PyUnicode_AsUTF8String(item);
                if (bytes == NULL) {
                    goto end;
                }
            } else {
                PyErr_SetString(PyExc_TypeError,
                    "first argument must be a string or sequence of strings");
                goto end;
            }
            (*patterns)[i] = strdup(PyBytes_AsString(bytes));
            if ((*patterns)[i] == NULL) {
                PyErr_NoMemory();
                Py_DECREF(bytes);
                goto end;
            }
        }
        result = 0;
    }
 end:
    Py_XDECREF(list);
    /* cleanup the partial pattern list if there was an error*/
    if (result < 0 && *patterns != NULL) {
        free_key_patterns(*patterns);
        *patterns = NULL;
    }
    return result;
}
static const char bf6p6_context_export_doc[] =
    "export(patterns, fd, mode)";


static PyObject *
bf6p6_context_export(BF6P6Context *self, PyObject *args)
{
    PyObject *py_pattern, *py_keydata;
    char **patterns = NULL;
    gpgme_data_t keydata;
    gpgme_error_t err;
    int mode = GPGME_EXPORT_MODE_EXTERN;

    if (!PyArg_ParseTuple(args, "OO|i", &py_pattern, &py_keydata, &mode))
        return NULL;

    if (parse_key_patterns(py_pattern, &patterns) < 0)
        return NULL;

    if (bf6p6_data_new(&keydata, py_keydata)) {
        if (patterns)
            free_key_patterns(patterns);
        return NULL;
    }

    Py_BEGIN_ALLOW_THREADS;
    err = gpgme_op_export_ext(self->ctx, (const char **)patterns, mode, keydata);
    Py_END_ALLOW_THREADS;

    if (patterns)
        free_key_patterns(patterns);
    gpgme_data_release(keydata);
    if (bf6p6_check_error(err))
        return NULL;
    Py_RETURN_NONE;
}
static const char bf6p6_context_genkey_doc[] =
    "genkey(GnupgKeyParms, None, None)";

static PyObject *
bf6p6_context_genkey(BF6P6Context *self, PyObject *args)
{
    PyObject *py_pubkey = Py_None, *py_seckey = Py_None;
    const char *parms;
    gpgme_data_t pubkey = NULL, seckey = NULL;
    PyObject *result;
    gpgme_error_t err;

    if (!PyArg_ParseTuple(args, "z|OO", &parms, &py_pubkey, &py_seckey))
        return NULL;

    if (bf6p6_data_new(&pubkey, py_pubkey))
        return NULL;

    if (bf6p6_data_new(&seckey, py_seckey)) {
        gpgme_data_release(pubkey);
        return NULL;
    }

    Py_BEGIN_ALLOW_THREADS;
    err = gpgme_op_genkey(self->ctx, parms, pubkey, seckey);
    Py_END_ALLOW_THREADS;

    gpgme_data_release(seckey);
    gpgme_data_release(pubkey);
    result = bf6p6_genkey_result(self->ctx);

    if (bf6p6_check_error(err)) {
        PyObject *err_type, *err_value, *err_traceback;

        PyErr_Fetch(&err_type, &err_value, &err_traceback);
        PyErr_NormalizeException(&err_type, &err_value, &err_traceback);

        if (!PyErr_GivenExceptionMatches(err_type, bf6p6_error))
            goto end;

        if (result != NULL) {
            PyObject_SetAttrString(err_value, "result", result);
            Py_DECREF(result);
        }
    end:
        PyErr_Restore(err_type, err_value, err_traceback);
        return NULL;
    }

    return (PyObject *) result;
}


static PyObject *
bf6p6_context_delete(BF6P6Context *self, PyObject *args)
{
    BF6P6Key *key;
    int allow_secret = 0;
    gpgme_error_t err;

    if (!PyArg_ParseTuple(args, "O!|i", &BF6P6Key_Type, &key, &allow_secret))
        return NULL;

    Py_BEGIN_ALLOW_THREADS;
    err = gpgme_op_delete(self->ctx, key->key, allow_secret);
    Py_END_ALLOW_THREADS;

    if (bf6p6_check_error(err))
        return NULL;
    Py_RETURN_NONE;
}

static gpgme_error_t
bf6p6_edit_cb(void *user_data, gpgme_status_code_t status,
                const char *args, int fd)
{
    PyObject *callback, *ret;
    PyGILState_STATE state;
    gpgme_error_t err;

    state = PyGILState_Ensure();
    callback = (PyObject *)user_data;
    ret = PyObject_CallFunction(callback, "lzi", (long)status, args, fd);
    err = bf6p6_check_pyerror();
    Py_XDECREF(ret);
    PyGILState_Release(state);
    return err;
}

static PyObject *
bf6p6_context_edit(BF6P6Context *self, PyObject *args)
{
    BF6P6Key *key;
    PyObject *callback, *py_out;
    gpgme_data_t out;
    gpgme_error_t err;

    if (!PyArg_ParseTuple(args, "O!OO", &BF6P6Key_Type, &key, &callback,
                          &py_out))
        return NULL;

    if (bf6p6_data_new(&out, py_out))
        return NULL;

    Py_BEGIN_ALLOW_THREADS;
    err = gpgme_op_edit(self->ctx, key->key,
                        bf6p6_edit_cb, (void *)callback, out);
    Py_END_ALLOW_THREADS;

    gpgme_data_release(out);

    if (bf6p6_check_error(err))
        return NULL;
    Py_RETURN_NONE;
}

static PyObject *
bf6p6_context_card_edit(BF6P6Context *self, PyObject *args)
{
    BF6P6Key *key;
    PyObject *callback, *py_out;
    gpgme_data_t out;
    gpgme_error_t err;

    if (!PyArg_ParseTuple(args, "O!OO", &BF6P6Key_Type, &key, &callback,
                          &py_out))
        return NULL;

    if (bf6p6_data_new(&out, py_out))
        return NULL;

    Py_BEGIN_ALLOW_THREADS;
    err = gpgme_op_card_edit(self->ctx, key->key,
                             bf6p6_edit_cb, (void *)callback, out);
    Py_END_ALLOW_THREADS;

    gpgme_data_release(out);

    if (bf6p6_check_error(err))
        return NULL;
    Py_RETURN_NONE;
}

static const char bf6p6_context_keylist_doc[] =
    "keylist([strOrSeq[, secret]]) -> KeyIter instance\n\n"
    "Searches for keys matching the given pattern(s).\n\n"
    "strOrSeq: If None or not supplied, the KeyIter fetches all available\n"
    "  keys. If a string, it fetches keys matching the given pattern (such\n"
    "  as a name or email address). If a sequence of strings, it fetches\n"
    "  keys matching at least one of the given patterns.\n\n"
    "secret: If True, only secret keys will be returned (like 'gpg -K').";

static PyObject *
bf6p6_context_keylist(BF6P6Context *self, PyObject *args)
{
    PyObject *py_pattern = Py_None;
    char **patterns = NULL;
    int secret_only = 0;
    gpgme_error_t err;
    BF6P6KeyIter *ret;

    if (!PyArg_ParseTuple(args, "|Oi", &py_pattern, &secret_only))
        return NULL;

    if (parse_key_patterns(py_pattern, &patterns) < 0)
        return NULL;

    Py_BEGIN_ALLOW_THREADS;
    err = gpgme_op_keylist_ext_start(self->ctx, (const char **)patterns,
                                     secret_only, 0);
    Py_END_ALLOW_THREADS;

    if (patterns)
        free_key_patterns(patterns);

    if (bf6p6_check_error(err))
        return NULL;

    /* return a KeyIter object */
    ret = PyObject_New(BF6P6KeyIter, &BF6P6KeyIter_Type);
    if (!ret)
        return NULL;
    Py_INCREF(self);
    ret->ctx = self;
    return (PyObject *)ret;
}

// bf6p6_context_trustlist

static PyMethodDef bf6p6_context_methods[] = {
    { "set_engine_info", (PyCFunction)bf6p6_context_set_engine_info, METH_VARARGS , "set_engine_info(protocol, executable_path, keyring_path)"},
    { "set_locale", (PyCFunction)bf6p6_context_set_locale, METH_VARARGS },
    { "get_key", (PyCFunction)bf6p6_context_get_key, METH_VARARGS,
      bf6p6_context_get_key_doc },
    { "encrypt", (PyCFunction)bf6p6_context_encrypt, METH_VARARGS,
      bf6p6_context_encrypt_doc },
    { "encrypt_sign", (PyCFunction)bf6p6_context_encrypt_sign, METH_VARARGS },
    { "decrypt", (PyCFunction)bf6p6_context_decrypt, METH_VARARGS,
      bf6p6_context_decrypt_doc },
    { "decrypt_verify", (PyCFunction)bf6p6_context_decrypt_verify, METH_VARARGS },
    { "sign", (PyCFunction)bf6p6_context_sign, METH_VARARGS },
    { "verify", (PyCFunction)bf6p6_context_verify, METH_VARARGS },
    { "import_", (PyCFunction)bf6p6_context_import, METH_VARARGS},
    { "export", (PyCFunction)bf6p6_context_export, METH_VARARGS,
      bf6p6_context_export_doc},
    { "genkey", (PyCFunction)bf6p6_context_genkey, METH_VARARGS , bf6p6_context_genkey_doc},
    { "delete", (PyCFunction)bf6p6_context_delete, METH_VARARGS },
    { "edit", (PyCFunction)bf6p6_context_edit, METH_VARARGS },
    { "card_edit", (PyCFunction)bf6p6_context_card_edit, METH_VARARGS },
    { "keylist", (PyCFunction)bf6p6_context_keylist, METH_VARARGS,
      bf6p6_context_keylist_doc },
    // trustlist
    { NULL, 0, 0 }
};

static const char bf6p6_context_doc[] =
    "The settings and methods for interacting with GPG.\n\n"
    "Context() -> Context instance";

PyTypeObject BF6P6Context_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "bf6p6.Context",
    sizeof(BF6P6Context),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_dealloc = (destructor)bf6p6_context_dealloc,
    .tp_init = (initproc)bf6p6_context_init,
    .tp_getset = bf6p6_context_getsets,
    .tp_methods = bf6p6_context_methods,
    .tp_doc = bf6p6_context_doc,
};
