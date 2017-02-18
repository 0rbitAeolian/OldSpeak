# -*- coding: utf-8 -*-
from __future__ import unicode_literals
# from tests.functional.fixtures import JohnDoe
import os
from oldspeak.crypt0.core import GPGKeyChain
from oldspeak import settings
from tests.functional.scenarios import storage_scenario


@storage_scenario
def test_generate_key(context):
    keyring = GPGKeyChain(os.path.join(settings.OLDSPEAK_DATADIR, 'generate'))
    key = keyring.generate_key(
        u'Full User Name',
        u'some@email.com',
        length=1024,
        passphrase='s3cr37'
    )
    # import ipdb;ipdb.set_trace()
    public = key.public()
    public.should.be.a(basestring)
    public.should.contain('PGP PUBLIC KEY BLOCK')

    private = key.private()
    private.should.be.a(basestring)
    private.should.contain('PGP PRIVATE KEY BLOCK')
