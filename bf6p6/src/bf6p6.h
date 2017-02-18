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
#ifndef PYGPGME_H
#define PYGPGME_H

#include <Python.h>
#include <gpgme.h>

/* Provide fallback definitions for older GPGME versions.  */
#if GPGME_VERSION_NUMBER < 0x010400
typedef enum
  {
    GPGME_PINENTRY_MODE_DEFAULT  = 0,
    GPGME_PINENTRY_MODE_ASK      = 1,
    GPGME_PINENTRY_MODE_CANCEL   = 2,
    GPGME_PINENTRY_MODE_ERROR    = 3,
    GPGME_PINENTRY_MODE_LOOPBACK = 4
  }
gpgme_pinentry_mode_t;
#endif /* GPGME < 1.4.0 */


#include "pycompat.h"

#define HIDDEN __attribute__((visibility("hidden")))

typedef struct {
    PyObject_HEAD
    gpgme_ctx_t ctx;
} BF6P6Context;

typedef struct {
    PyObject_HEAD
    gpgme_key_t key;
} BF6P6Key;

typedef struct {
    PyObject_HEAD
    gpgme_subkey_t subkey;
    PyObject *parent;
} BF6P6Subkey;

typedef struct {
    PyObject_HEAD
    gpgme_user_id_t user_id;
    PyObject *parent;
} BF6P6UserId;

typedef struct {
    PyObject_HEAD
    gpgme_key_sig_t key_sig;
    PyObject *parent;
} BF6P6KeySig;

typedef struct {
    PyObject_HEAD
    PyObject *type;
    PyObject *pubkey_algo;
    PyObject *hash_algo;
    PyObject *timestamp;
    PyObject *fpr;
    PyObject *sig_class;
} BF6P6NewSignature;

typedef struct {
    PyObject_HEAD
    PyObject *summary;
    PyObject *fpr;
    PyObject *status;
    PyObject *notations;
    PyObject *timestamp;
    PyObject *exp_timestamp;
    PyObject *wrong_key_usage;
    PyObject *validity;
    PyObject *validity_reason;
} BF6P6Signature;

typedef struct {
    PyObject_HEAD
    PyObject *considered;
    PyObject *no_user_id;
    PyObject *imported;
    PyObject *imported_rsa;
    PyObject *unchanged;
    PyObject *new_user_ids;
    PyObject *new_sub_keys;
    PyObject *new_signatures;
    PyObject *new_revocations;
    PyObject *secret_read;
    PyObject *secret_imported;
    PyObject *secret_unchanged;
    PyObject *skipped_new_keys;
    PyObject *not_imported;
    PyObject *imports;
} BF6P6ImportResult;

typedef struct {
    PyObject_HEAD
    PyObject *primary;
    PyObject *sub;
    PyObject *fpr;
} BF6P6GenkeyResult;

typedef struct {
    PyObject_HEAD
    BF6P6Context *ctx;
} BF6P6KeyIter;

extern HIDDEN PyObject *bf6p6_error;
extern HIDDEN PyTypeObject BF6P6Context_Type;
extern HIDDEN PyTypeObject BF6P6Key_Type;
extern HIDDEN PyTypeObject BF6P6Subkey_Type;
extern HIDDEN PyTypeObject BF6P6UserId_Type;
extern HIDDEN PyTypeObject BF6P6KeySig_Type;
extern HIDDEN PyTypeObject BF6P6NewSignature_Type;
extern HIDDEN PyTypeObject BF6P6Signature_Type;
extern HIDDEN PyTypeObject BF6P6ImportResult_Type;
extern HIDDEN PyTypeObject BF6P6GenkeyResult_Type;
extern HIDDEN PyTypeObject BF6P6KeyIter_Type;

HIDDEN int           bf6p6_check_error    (gpgme_error_t err);
HIDDEN PyObject     *bf6p6_error_object   (gpgme_error_t err);
HIDDEN gpgme_error_t bf6p6_check_pyerror  (void);
HIDDEN int           bf6p6_no_constructor (PyObject *self, PyObject *args,
                                             PyObject *kwargs);

HIDDEN int           bf6p6_data_new       (gpgme_data_t *dh, PyObject *fp);
HIDDEN PyObject     *bf6p6_key_new        (gpgme_key_t key);
HIDDEN PyObject     *bf6p6_newsiglist_new (gpgme_new_signature_t siglist);
HIDDEN PyObject     *bf6p6_siglist_new    (gpgme_signature_t siglist);
HIDDEN PyObject     *bf6p6_import_result  (gpgme_ctx_t ctx);
HIDDEN PyObject     *bf6p6_genkey_result  (gpgme_ctx_t ctx);

HIDDEN PyObject     *bf6p6_make_constants (PyObject *self, PyObject *args);

#endif
