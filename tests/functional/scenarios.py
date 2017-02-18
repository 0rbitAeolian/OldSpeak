#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import shutil
import logging
import coloredlogs
# from glob import glob
from sure import scenario
# from datetime import datetime
from oldspeak import settings
from oldspeak.persistence import connectors
from oldspeak.persistence.sql.mapper import metadata, orm
from oldspeak.http.server import Application
from oldspeak.lib.clients import OldSpeakClient
from oldspeak.lib.networking import get_free_tcp_port

coloredlogs.install(logging.DEBUG)


def prepare_server(context):
    host = '127.0.0.1'
    port = get_free_tcp_port()
    app = Application()
    server = app.wsgi(port=port, host=host)
    base_url = 'http://{host}:{port}'.format(**locals())
    context.app = app
    context.server = server
    context.server.start()
    context.client = OldSpeakClient(server.get_url())


def cleanup_server(context):
    context.server.stop()


def prepare_admin_scenario(context):
    context.user = orm.User.using(context.db.alias).create(
        name='Mary Poppins',
        email='mary@oldspeak.io',
    )


def prepare_subscriber_scenario(context):
    context.user = orm.User.using(context.db.alias).create(
        name='John Doe',
        email='johndoe@gmail.com'
    )


class db_connection(object):

    def __init__(self, alias):
        self.alias = alias
        self.engine = connectors.sql.get_pool(alias)
        self.connection = connectors.sql.get_connection(alias)


def prepare_sql(context):
    context.db = db_connection('test')
    metadata.drop_all(bind=context.db.engine)
    metadata.create_all(bind=context.db.engine)


def cleanup_sql(context):
    metadata.drop_all(bind=context.db.engine)


def prepare_storage(context):
    context.datadir_path = settings.OLDSPEAK_DATADIR
    target = context.datadir_path
    if os.path.isdir(target):
        shutil.rmtree(target)

    os.makedirs(target)
    # for target in glob('{0}/*'.format(settings.OLDSPEAK_DATADIR)):
    #     if os.path.isdir(target):
    #         shutil.rmtree(target)
    #     elif os.path.isfile(target):
    #         os.unlink(target)
    #
    # # os.makedirs(settings.OLDSPEAK_DATADIR)


def cleanup_storage(context):
    target = context.datadir_path
    if os.path.isdir(target):
        shutil.rmtree(target)
    # utcnow = datetime.utcnow()
    # shutil.copytree(settings.OLDSPEAK_DATADIR, '_'.join((settings.OLDSPEAK_DATADIR, utcnow.isoformat())))
    pass


storage_scenario = scenario([prepare_storage], [cleanup_storage])
web_scenario = scenario([prepare_storage, prepare_sql, prepare_server], [
                        cleanup_server, cleanup_sql, cleanup_storage])
sql_scenario = scenario(prepare_sql, cleanup_sql)
api_admin_scenario = scenario(
    [prepare_sql, prepare_server, prepare_admin_scenario],
    [cleanup_server, cleanup_sql])
api_user_scenario = scenario(
    [prepare_sql, prepare_server, prepare_subscriber_scenario],
    [cleanup_server, cleanup_sql])


def cookies(response):
    return "".join(response.headers.getlist('Set-Cookie'))
