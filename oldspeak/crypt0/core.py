# -*- coding: utf-8 -*-
import os
import bf6p6
import logging
from oldspeak import settings
from oldspeak.crypt0.gpg import GPGKeyChain


def generate_temp_token(bits=128):
    if bits % 8:
        raise ValueError(
            'generate_temp_token() only works with multiples of 8')

    return os.urandom(int(bits / 8)).encode('hex')


def bytes2int(b):
    return int(b.encode('hex'), 16)


def int2bytes(v):
    return format('x', v).decode('hex')


def xor(left, right):
    product = bytes2int(left) ^ bytes2int(right)
    return int2bytes(product)


class InvitationRoster(GPGKeyChain):

    def __init__(self, base_path=None):
        if base_path is None:
            base_path = settings.OLDSPEAK_DATADIR

        path = os.path.join(base_path, 'invitation-keyring')
        super(InvitationRoster, self).__init__(path)

    def get_public_key(self, fpr):
        found = self.list(fpr)
        if not found:
            return

        key = found[0]
        return key.public()

    def invite(self, armored_key):
        return self.import_key(armored_key)
