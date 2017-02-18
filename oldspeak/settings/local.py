#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# This file sets up the default environment variables for working in
# local environment.

# The reason for this is that in production the app MUST work
# seamlessly after certain required environment variables are set.

# This file is an example of ALL those essential variables.

import os


def setup_localhost(settings):
    # Relational Database

    # REDIS
    # ~~~~~

    # Example of REDIS_URI for localhost:
    #   redis://localhost:6379
    #
    # Example of REDIS_URI for *PRODUCTION*:
    #   redis://redis-server-hostname:6379/verylongpasswordhash

    settings.SESSION_SECRET_KEY = os.urandom(8).encode('hex')
    settings.SQLALCHEMY_DATABASES[
        'test'] = 'sqlite:///{}'.format(settings.project_file('k43p5d10.sqlite'))
    settings.REDIS_SERVERS['test'] = 'redis://localhost:6379'
    settings.SECRET_KEY = 'deadbeefdeadbeefdeadbeefdeadbeefdeadbeefdeadbeef'
    settings.OLDSPEAK_WORKDIR = settings.project_file('sandbox')
    settings.OLDSPEAK_DATADIR = settings.project_file('data')
