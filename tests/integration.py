# -*- coding: utf-8 -*-
import os
import shutil
from datetime import datetime

from oldspeak import settings
from oldspeak.persistence.vfs import Bucket
from oldspeak.crypt0.gpg import GPGKeyChain

DATADIR = os.path.join(os.path.abspath(os.getcwd()), 'integration-data')

if os.path.isdir(DATADIR):
    shutil.rmtree(DATADIR)

os.makedirs(DATADIR)


def random_file_contents():
    return datetime.utcnow().isoformat()

keychain = GPGKeyChain(DATADIR)
key = keychain.generate_key(
    'Mary Doe',
    'mary@doe.com',
    length=4096
)

print key.public()
print key.private()

test = Bucket('test', author_name='root', author_email='root@localhost')
test.write_file('example-number-one-1.file', random_file_contents())
# test.write_file('shallow-example-number-two-2.file', random_file_contents())
# test.write_file('moredata/single-depth-three-3.file', random_file_contents())
# test.write_file('moredata/double-depth-four/four-4.file', random_file_contents())
# test.save()

os.chdir('data/test')
os.system('/usr/local/bin/tree')
os.system('/usr/local/bin/git branch')
os.system('/usr/local/bin/git whatchanged')
os.system('/usr/local/bin/git status')
