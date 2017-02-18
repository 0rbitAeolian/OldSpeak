# -*- coding: utf-8 -*-
import os
import io
import bf6p6
import logging
from datetime import datetime
from oldspeak.lib.buffers import AutoBuffer
# import ipdb;ipdb.set_trace()


def generate_temp_token(bits=128):
    if bits % 8:
        raise ValueError(
            'generate_temp_token() only works with multiples of 8')

    return os.urandom(int(bits / 8)).encode('hex')


class GPGKeyChain(object):

    def __init__(self, data_dir, armor=True, textmode=True):
        self.armor = armor
        self.textmode = textmode
        self.data_dir = data_dir
        if isinstance(data_dir, basestring) and not os.path.isdir(data_dir):
            os.makedirs(data_dir)

        self.context = bf6p6.Context()
        self.context.armor = armor
        self.context.progress_cb = self.progress_callback
        self.context.set_engine_info(
            bf6p6.PROTOCOL_OpenPGP,
            None,
            data_dir
        )

    def progress_callback(self, action, action_type, current, total):
        logging.info(
            '{action} ({action_type}): {current}/{total}'.format(**locals()))

    def key(self, fpr):
        return GPGKey(self.context.get_key(fpr), self.context)

    def import_key(self, data):

        return self.context.import_(io.BytesIO(bytes(data)))

    def list(self, pattern=None):
        return map(
            lambda key: GPGKey(
                key,
                self.context),
            self.context.keylist(pattern))

    def generate_key(
            self,
            name,
            email,
            key_type='RSA',
            length=4096,
            expire_date=0,
            comment=None,
            passphrase=None):
        comment = comment or 'created at {}'.format(
            datetime.utcnow().isoformat())
        tmpl = '\n'.join(filter(bool, [
            '<GnupgKeyParms format="internal">',
            'Key-Type: RSA',
            'Key-Length: {length}',
            'Subkey-Type: RSA',
            'Subkey-Length: {length}',
            'Name-Real: {name}',
            comment and 'Name-Comment: {comment}',
            'Name-Email: {email}',
            'Expire-Date: {expire_date}',
            passphrase is not None and 'Passphrase: {passphrase}',
            '</GnupgKeyParms>',
        ]))
        result = self.context.genkey(tmpl.format(**locals()), None, None)
        key = self.context.get_key(result.fpr)
        # import ipdb
        # ipdb.set_trace()
        return self.key(result.fpr)


class GPGKey(object):

    def __init__(self, key, context):
        for name in dir(key):
            if not name.startswith('_'):
                setattr(self, name, getattr(key, name))

        self.key = key
        self.context = context

        self.subkeys = key.subkeys

        if len(self.subkeys) == 2:
            self.private_key, self.public_key = self.subkeys
        elif len(self.subkeys) == 1:
            self.public_key = self.subkeys[0]
            self.private_key = None
        else:
            self.public_key = self.private_key = None

        self.name = key.uids[0].name
        self.email = key.uids[0].email
        self.comment = key.uids[0].comment

    def export_public(self, filelike):
        if self.public_key:
            self.context.export([self.public_key.fpr], filelike, 0)

    def export_private(self, filelike):
        if self.private_key:
            self.context.export(
                [self.private_key.fpr],
                filelike, bf6p6.EXPORT_MODE_SECRET)

    def public(self):
        return AutoBuffer(self.export_public).getvalue()

    def private(self):
        return AutoBuffer(self.export_private).getvalue()
