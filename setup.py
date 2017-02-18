# -*- coding: utf-8 -*-

import sys

import ast
import os
from setuptools import setup
from setuptools import Extension

reload(sys)
sys.setdefaultencoding('utf-8')

local_path = lambda *f: os.path.join(os.path.dirname(__file__), *f)
local_file = lambda *f: open(local_path(*f), 'rb').read()


bf6p6 = Extension(
    'bf6p6._bf6p6',
    [
        'bf6p6/src/bf6p6.c',
        'bf6p6/src/bf6p6-error.c',
        'bf6p6/src/bf6p6-data.c',
        'bf6p6/src/bf6p6-context.c',
        'bf6p6/src/bf6p6-key.c',
        'bf6p6/src/bf6p6-signature.c',
        'bf6p6/src/bf6p6-import.c',
        'bf6p6/src/bf6p6-keyiter.c',
        'bf6p6/src/bf6p6-constants.c',
        'bf6p6/src/bf6p6-genkey.c',
    ],
    include_dirs=[local_path('bf6p6')],
    libraries=['gpgme']
)


dependencies = filter(bool, map(bytes.strip, local_file('requirements.txt').splitlines()))

# https://setuptools.readthedocs.io/en/latest/setuptools.html#adding-setup-arguments
setup(
    name='oldspeak',
    version='0.1.0',
    description="\n".join([
        'I reckon a bellyfeel: while writing in the best language for it'
    ]),
    entry_points={
        'console_scripts': [
            'oldspeak = oldspeak.console.main:entrypoint',
        ],
    },
    author=u"Ð4√¡η¢Ч",
    author_email='d4v1ncy@protonmail.ch',
    url=u'https://github.com/0rbitAeolian/OldSpeak',
    packages=[
        'oldspeak',
        'bf6p6',
    ],
    install_requires=dependencies,
    include_package_data=True,
    package_data={
        'oldspeak': 'COPYING *.rst *.txt docs/source/* docs/*'.split(),
    },
    ext_modules=[bf6p6],
    zip_safe=False,
)
