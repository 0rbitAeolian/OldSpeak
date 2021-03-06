BF6P6 Cookbook
################

The following recipes illustrate typical use cases of BF6P6. For a detailed
documentation of the individual classes and methods please refer to the
:doc:`API documentation <api>`.

Listing All Keys
================

Use :py:meth:`Context.keylist` without arguments to get all keys::

    from __future__ import print_function
    import bf6p6

    c = bf6p6.Context()
    for key in c.keylist():
        user = key.uids[0]
        print("Keys for %s (%s):" % (user.name, user.email))
        for subkey in key.subkeys:
            features = []
            if subkey.can_authenticate:
                features.append('auth')
            if subkey.can_certify:
                features.append('cert')
            if subkey.can_encrypt:
                features.append('encrypt')
            if subkey.can_sign:
                features.append('sign')
            print('  %s %s' %(subkey.fpr, ','.join(features)))


Searching for a Specific Key
============================

To search for a key using parts of the key owner's name or e-mail address, pass
a query to :py:meth:`bf6p6.Context.keylist`::

    from __future__ import print_function
    import bf6p6

    c = bf6p6.Context()
    for key in c.keylist('john'):
        print(key.subkeys[0].fpr)

To get a key via its fingerprint, use :py:meth:`bf6p6.Context.get_key` instead
(note that you must pass the full fingerprint)::

    from __future__ import print_function
    import bf6p6

    c = bf6p6.Context()
    fingerprint = 'key fingerprint to search for'
    try:
        key = c.get_key(fingerprint)
        print('%s (%s)' % (key.uids[0].name, key.uids[0].email))
    except bf6p6.BF6P6Error:
        print("No key for fingerprint '%s'." % fingerprint)


Encrypting and Decrypting Files
===============================

By default, :py:meth:`bf6p6.Context.encrypt` returns the encrypted data in binary
form, so make sure to open the ciphertext files in binary mode::

    import bf6p6

    c = bf6p6.Context()
    recipient = c.get_key("fingerprint of recipient's key")

    # Encrypt
    with open('foo.txt', 'r') as input_file:
        with open('foo.txt.gpg', 'wb') as output_file:
            c.encrypt([recipient], 0, input_file, output_file)

    # Decrypt
    with open('foo.txt.gpg', 'rb') as input_file:
        with open('foo2.txt', 'w') as output_file:
            c.decrypt(input_file, output_file)

If you set :py:attr:`bf6p6.Context.armor` to ``True`` then the ciphertext is
encoded in a so-called ASCII-armor string. In that case, the ciphertext file
should be opened in text mode.

The example above uses `asymmetric encryption`_, i.e. the data is encrypted
using a public key and can only be decrypted using the corresponding private
key. If you want to use `symmetric encryption`_ instead (where encryption
and decryption use the same passphrase) then pass ``None`` as the first
argument to :py:meth:`bf6p6.Context.encrypt`. In that case you will be prompted
for the passphrase.

.. _`asymmetric encryption`: https://en.wikipedia.org/wiki/Public-key_cryptography
.. _`symmetric encryption`: https://en.wikipedia.org/wiki/Symmetric-key_algorithm


Encrypting and Decrypting Bytes and Strings
===========================================

:py:meth:`bf6p6.Context.encrypt` and :py:meth:`bf6p6.Context.decrypt` operate
on streams of data (i.e. file-like objects). If you want to encrypt or decrypt
data from bytes variables instead then you need to wrap them in a
suitable buffer (e.g. :py:class:`io.BytesIO`)::

    import io
    import bf6p6

    c = bf6p6.Context()
    recipient = c.get_key("fingerprint of recipient's key")

    plaintext_bytes = io.BytesIO(b'plain binary data')
    encrypted_bytes = io.BytesIO()
    c.encrypt([recipient], 0, plaintext_bytes, encrypted_bytes)

    encrypted_bytes.seek(0)  # Return file pointer to beginning of file

    decrypted_bytes = io.BytesIO()
    c.decrypt(encrypted_bytes, decrypted_bytes)

    assert decrypted_bytes.getvalue() == plaintext_bytes.getvalue()

Note that :py:meth:`bf6p6.Context.encrypt` only accepts binary buffers -- passing
text buffers like :py:class:`io.StringIO` raises :py:class:`bf6p6.BF6P6Error`.
To encrypt string data, you therefore need to encode it to binary first::

    import io
    import bf6p6

    c = bf6p6.Context()
    recipient = c.get_key("fingerprint of recipient's key")

    plaintext_string = u'plain text data'
    plaintext_bytes = io.BytesIO(plaintext_string.encode('utf8'))
    encrypted_bytes = io.BytesIO()
    c.encrypt([recipient], 0, plaintext_bytes, encrypted_bytes)

    encrypted_bytes.seek(0)  # Return file pointer to beginning of file

    decrypted_bytes = io.BytesIO()
    c.decrypt(encrypted_bytes, decrypted_bytes)
    decrypted_string = decrypted_bytes.getvalue().decode('utf8')

    assert decrypted_string == plaintext_string

Even if :py:attr:`bf6p6.Context.armor` is true and the encrypted output is text
you still need to use binary buffers. That is not a problem, however, since the
armor uses plain ASCII::

    from __future__ import print_function

    import io
    import bf6p6

    c = bf6p6.Context()
    recipient = c.get_key("fingerprint of recipient's key")
    c.armor = True  # Use ASCII-armor output

    plaintext_string = u'plain text data'
    plaintext_bytes = io.BytesIO(plaintext_string.encode('utf8'))
    encrypted_bytes = io.BytesIO()
    c.encrypt([recipient], 0, plaintext_bytes, encrypted_bytes)
    encrypted_string = encrypted_bytes.getvalue().decode('ascii')
    print(encrypted_string)  # Display ASCII armored ciphertext

    # Re-initialize encrypted bytes data from ASCII armor
    encrypted_bytes = io.BytesIO(encrypted_string.encode('ascii'))

    decrypted_bytes = io.BytesIO()
    c.decrypt(encrypted_bytes, decrypted_bytes)
    decrypted_string = decrypted_bytes.getvalue().decode('utf8')

    assert decrypted_string == plaintext_string


Signing
=======

FIXME


Verifying a Signature
=====================

FIXME


Generating Keys
===============

.. code:: python

    import io
    import os
    import bf6p6

    keyring_path = os.path.expanduser('~/gpg-keyring')

    if not os.path.exists(keyring_path):
        os.makedirs(keyring_path)

    c = bf6p6.Context()
    c.armor = True
    c.textmode = True
    c.set_engine_info(
        bf6p6.PROTOCOL_OpenPGP,
        None,
        keyring_path
    )

    key_type = 'RSA'
    comment = ''
    length = 4096
    name = 'Destiny Cyfer'
    email = 'destiny@bf6p6.mail'
    passphrase = None

    # craft the "parms" string
    parameters = '\n'.join(filter(bool, [
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
    ])).format(**locals())

    # generate a key pair
    result = c.genkey(parameters, None, None)
    private, public = result.subkeys

    # exporting to string
    buf = BytesIO()

    c.export(private.fpr, bf6p6.EXPORT_MODE_SECRET, buf)
    private_armored = buf.getvalue()

    c.export(public.fpr, bf6p6.EXPORT_MODE_EXTERN, buf)
    public_armored = buf.getvalue()


Using a Passphrase Callback
===========================

FIXME


Using a Different GPG Base Directory
====================================

.. code:: python

    import io
    import os
    import shutil

    import bf6p6

    keyring_path = os.path.expanduser('~/gpg-keyring')

    if os.path.isdir(keyring_path):
        shutil.rmtree(keyring_path)

    os.makedirs(keyring_path)

    c = bf6p6.Context()
    c.armor = True
    c.textmode = True

    c.set_engine_info(
        bf6p6.PROTOCOL_OpenPGP,  # PGP
        None,                    # use the default executable
        keyring_path             # path to the keyring
    )
