# -*- coding: utf-8 -*-
import os

import json
import traceback

from flask import Flask, Response, request
from flask_oauthlib.provider import OAuth2Provider
from gevent.wsgi import WSGIServer

from plant import Node

from oldspeak import settings
from oldspeak.core import get_logger
from oldspeak.lib.logs import configure_logging
from oldspeak.lib.networking import get_free_tcp_port
from oldspeak.http.core import html
from oldspeak.http.endpoints import get_server_components

COLORED_LOGS_LEVEL = os.environ.get('COLORED_LOGS_LEVEL')

if COLORED_LOGS_LEVEL:
    configure_logging(COLORED_LOGS_LEVEL)

logger = get_logger()


class Application(Flask):

    def __init__(
            self,
            secret_key=settings.SECRET_KEY,
            host='127.0.0.1',
            port=None,
            ssl=None):
        app_node = Node(__file__).dir
        super(Application, self).__init__(
            __name__,
            static_folder=settings.STATIC_FOLDER_PATH,
            template_folder=settings.HTML_TEMPLATE_PATH,
            static_url_path=settings.STATIC_URL_PREFIX,
        )
        self.scheme = ssl is None and 'http' or 'https'
        self.host = host
        self.port = int(port or get_free_tcp_port())
        self.ssl = ssl
        self.config.from_object('oldspeak.settings')
        self.app_node = app_node
        self.secret_key = os.environ.get('SECRET_KEY')
        self.oauth = OAuth2Provider(self)
        self.secret_key = secret_key
        components = get_server_components()
        if not components:
            raise RuntimeError('no components found')

        for bp in components:
            logger.info('registering component {}'.format(bp))
            self.register_blueprint(bp)

    def run(self, **kw):
        kw['host'] = kw.pop('host', self.host)
        kw['port'] = kw.pop('port', self.port)
        kw['debug'] = kw.pop('debug', False)
        kw['use_evalex'] = kw.pop('use_evalex', False)
        kw['use_reloader'] = kw.pop('use_reloader', True)
        return super(Application, self).run(**kw)

    def json_handle_weird(self, obj):
        fallback = repr(obj)
        logger.warning("failed to serialize %s", fallback)
        return fallback

    def json_response(self, data, code=200, headers={}):
        headers = headers.copy()
        headers['Content-Type'] = 'application/json'
        payload = json.dumps(data, indent=2, default=self.json_handle_weird)
        r = Response(payload, status=code, headers=headers)
        return r

    def get_json_request(self):
        try:
            data = json.loads(request.data)
        except ValueError:
            logger.exception(
                "Trying to parse json body in the %s to %s",
                request.method, request.url,
            )
            data = {}

        return data

    def handle_exception(self, e):
        tb = traceback.format_exc(e)
        logger.error(tb)

        if settings.LOCAL:
            return self.json_response(
                {'error': 'bad-request', 'traceback': tb},
                code=500)
        else:
            return html('500.html')

    def stop(self):
        logger.warning('stopping server')

    def get_url(self):
        return '{scheme}://{host}:{port}'.format(**self.__dict__)

    def wsgi(
            self,
            port=None,
            host=None,
            max_accept=settings.GEVENT_MAX_CONNECTIONS,
            min_delay=0.1,
            max_delay=1,
            stop_timeout=1):
        if isinstance(port, int):
            self.port = port

        if isinstance(host, basestring):
            self.host = host

        wsgi_params = dict(
            max_accept=max_accept,
            min_delay=min_delay,
            max_delay=max_delay,
            stop_timeout=stop_timeout,
            get_url=lambda s: self.get_url()
        )
        StandaloneServer = type(
            'StandaloneServer', (WSGIServer, ), wsgi_params)
        return StandaloneServer((self.host, self.port), self)
