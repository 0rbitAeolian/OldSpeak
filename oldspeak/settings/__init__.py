# -*- coding: utf-8 -*-

import sys
import multiprocessing

from os.path import join, abspath, dirname
from envelop import Environment

# enforce utf-8
ENCODING = 'utf-8'
reload(sys)
sys.setdefaultencoding(ENCODING)

local_file = lambda *path: join(abspath(dirname(__file__)), *path)
module_file = lambda *path: abspath(local_file('..', *path))
project_file = lambda *path: abspath(module_file('..', *path))


env = Environment()

SELF = sys.modules[__name__]

CONNECTION_POOL_SIZE = multiprocessing.cpu_count()

LOCAL_PORT = 19842
PORT = env.get_int('PORT', LOCAL_PORT)

TEST_MODE = env.get('TEST_MODE', 'false')
OLDSPEAK_WORKDIR = env.get('OLDSPEAK_WORKDIR', '/srv/oldspeak/sandbox')
OLDSPEAK_DATADIR = env.get('OLDSPEAK_DATADIR', '/srv/oldspeak/private-data')
OLDSPEAK_PUBLICDIR = env.get('OLDSPEAK_PUBLICDIR', '/srv/oldspeak/public-data')
STATIC_FOLDER_PATH = env.get(
    'OLDSPEAK_STATIC_FOLDER_PATH',
    project_file(
        'static',
        'dist'))
HTML_TEMPLATE_PATH = env.get(
    'OLDSPEAK_HTML_TEMPLATE_PATH',
    project_file(
        'static',
        'templates'))
STATIC_URL_PREFIX = '/s'

HTML_TEMPLATE_PATH = project_file('static', 'templates')

BOOTSTRAP_USE_MINIFIED = True
BOOTSTRAP_SERVE_LOCAL = True
BOOTSTRAP_CDN_FORCE_SSL = True
BOOTSTRAP_QUERYSTRING_REVVING = True

# Identifying environment
LOCAL = str(PORT) == str(LOCAL_PORT)

# used for indicating where static files live in
ENVIRONMENT = {
    'production': 'pro',
    'development': 'dev',
}[env.get('OLDSPEAK_ENVIRONMENT', 'development')]

# Detecting environment
DEBUG = env.get_bool('DEBUG')

# HTTP
HOST = env.get("HOST") or 'oldspeak'
DOMAIN = env.get("DOMAIN") or '{HOST}:{PORT}'.format(**locals())
SCHEME = env.get('SCHEME') or (
    'oldspeak' not in DOMAIN and 'https://' or 'http://')
LOG_LEVEL_NAME = (env.get('LOG_LEVEL') or 'INFO').upper()

# Database-related

SQLALCHEMY_DATABASE_URI = env.get(
    'SQLALCHEMY_DATABASE_URI',
    'postgresql+psycopg2://01d5pk:kp5d10@localhost/01d5pk')
DATA_DIR = env.get('OLDSPEAK_DATADIR', "~/.oldspeak/data")
REDIS_URI = env.get_uri("REDIS_URI") or 'redis://localhost:6379'

# Filesystem
LOCAL_FILE = lambda *path: abspath(join(dirname(__file__), '..', '..', *path))

# Security
SECRET_KEY = env.get("SECRET_KEY")

# Gevent/Concurrency

VFS_PERSISTENCE_USER = 'oldspeak service'
VFS_PERSISTENCE_EMAIL = 'service@oldspeak'

GEVENT_MAX_CONNECTIONS = 1024 * 32

API_TOKEN_EXPIRATION_TIME = 60 * 60 * 12  # 12 hours in seconds

UPLOAD_PATH = env.get('OLDSPEAK_UPLOAD_PATH') or 'uploads'
UPLOADED_FILE = lambda *path: join(UPLOAD_PATH, *path)

absurl = lambda *path: "{0}{1}/{2}".format(
    SCHEME, DOMAIN, "/".join(path).lstrip('/'))

APP_URL = lambda *path: absurl('app', *path)

DEFAULT_CONNECTOR_ALIAS = 'default'

REDIS_URI_DEFAULT = env.get('REDIS_URI_DEFAULT')

SQLALCHEMY_DATABASES = {
    'default': SQLALCHEMY_DATABASE_URI,
    'oldspeak.api': SQLALCHEMY_DATABASE_URI,
}

REDIS_SERVERS = {
    DEFAULT_CONNECTOR_ALIAS: REDIS_URI_DEFAULT,
    'oldspeak.api': REDIS_URI_DEFAULT,
}


def get_sqlalchemy_url(alias=None):
    alias = alias or DEFAULT_CONNECTOR_ALIAS
    return SQLALCHEMY_DATABASES[alias]


def get_redis_url(alias=None):
    alias = alias or DEFAULT_CONNECTOR_ALIAS
    return REDIS_SERVERS[alias]


if LOCAL:
    from .local import setup_localhost
    setup_localhost(SELF)
else:
    sys.stderr.write('running in production\n')
