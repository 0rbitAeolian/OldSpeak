# -*- coding: utf-8 -*-
import os

from functools import partial
from datetime import datetime
from collections import OrderedDict

from flask import Blueprint as BaseBlueprint
from flask import Response, request
from flask import render_template
from flask import redirect as flask_redirect

from oldspeak import settings
from oldspeak.core import get_logger
from oldspeak.lib.serializers import json


logger = get_logger()


__SERVER_COMPONENTS__ = OrderedDict()


def patch_response(response, headers=None, no_cache=True, cookies=None):
    headers = headers or {}
    cookies = cookies or {}
    if no_cache:
        headers['Cache-Control'] = 'no-cache, no-store, must-revalidate'
        headers['Pragma'] = 'no-cache'
        headers['Last-Modified'] = datetime.utcnow()
        headers[
            'Cache-Control'] = 'no-store, no-cache, must-revalidate, post-check=0, pre-check=0, max-age=0'
        headers['Pragma'] = 'no-cache'
        headers['Expires'] = '-1'

    for key, value in headers.items():
        response.headers[key] = value

    # set cookies
    for key, value in cookies.items():
        response.set_cookie(key, value=value)

    return response


def redirect(target, code=302, **kw):
    return patch_response(flask_redirect(target, code=code), **kw)


def generate_token():
    return os.urandom(32).encode('hex')


def generate_seed(path):
    return '&'.join(
        (os.urandom(8).encode('hex').encode('rot13'),
         path.encode('hex')))


def html(template, context=None, code=200, **kw):
    context = context or {}
    rendered = render_template(template, **context)
    return patch_response(Response(rendered, status=code), **kw)


class HTMLRoute(object):

    def __init__(
            self,
            route,
            template_name='index.html',
            oldspeak_token=None,
            seed=None):
        self.route = route
        self.template_name = template_name
        self.oldspeak_token = oldspeak_token
        self.seed = seed
        self.__name__ = route.strip('/') or 'index'

    def __call__(self):
        context = {
            'oldspeak_token': self.oldspeak_token or generate_token(),
            'seed': self.seed or generate_seed(self.route),
            'path': self.route,
        }
        return html(self.template_name, context=context)


class ServerComponent(BaseBlueprint):

    def __init__(self, module_name, *args, **kw):
        prefix = kw.pop('url_prefix', kw.pop('prefix', None))
        existing = __SERVER_COMPONENTS__.get(prefix, None)
        prefix = isinstance(prefix, basestring) and prefix.rstrip('/') or None

        if prefix in __SERVER_COMPONENTS__:
            raise RuntimeError(
                'prefix already in use by component: {}'.format(existing))

        name = module_name.split('.')[-1]
        kw['url_prefix'] = prefix

        self.endpoint_name = name
        self.endpoint_module = module_name
        self.endpoint_prefix = prefix or ''

        for http_method in ('GET', 'POST', 'PUT', 'DELETE', 'HEAD', 'PATCH'):
            setattr(ServerComponent, http_method.lower(),
                    partial(self.route, methods=[http_method]))

        __SERVER_COMPONENTS__[hash(self)] = self
        super(ServerComponent, self).__init__(name, module_name, *args, **kw)

    def __hash__(self):
        return int(self.endpoint_module.encode('hex'), 16)

    def __repr__(self):
        return repr(self.endpoint_module)

    def route(self, path, methods=None, view_func=None):
        path = "/{}".format("/".join([self.endpoint_prefix,
                                      path.strip().strip('/')]).strip('/'))
        name = path.replace('/', '.').strip('.')

        if callable(view_func):
            logger.debug('url rule %s', path)
            return self.add_url_rule(
                path,
                endpoint=name,
                view_func=view_func,
                methods=methods)

        logger.debug('route %s', path)
        return super(ServerComponent, self).route(path, methods=methods)

    def direct_html(self, name, path, methods=None, **defaults):
        template_name = '{}.html'.format(name)

        def handle(**kw):
            context = defaults.copy()
            context.update(kw)
            kw['seed'] = kw.pop('seed', generate_seed(path))
            return html(template_name, context=kw)

        return self.get(path, methods=methods, view_func=handle)


def get_server_components():
    return __SERVER_COMPONENTS__.values()


def set_cors_into_headers(headers, allow_origin, allow_credentials=True, max_age=30 * 5):  # 2.5 minutes
    """Takes flask.Response.headers and a string contains the origin
    to be allowed and modifies the given headers inline.

    >>> headers = {'Content-Type': 'application/json'}
    >>> set_cors_into_headers(headers, allow_origin='cahoots.in')
    """

    headers[b'Access-Control-Allow-Origin'] = allow_origin
    headers[b'Access-Control-Allow-Headers'] = request.headers.get(
        'Access-Control-Request-Headers', '*')

    headers[b'Access-Control-Allow-Methods'] = request.headers.get(
        'Access-Control-Request-Method', '*')

    headers[b'Access-Control-Allow-Credentials'] = (
        allow_credentials and 'true' or 'false')

    headers[b'Access-Control-Max-Age'] = max_age


def json_representation(
    data, code, headers={
        'Content-Type': 'application/json'}):
    set_cors_into_headers(headers, allow_origin=settings.DOMAIN)
    return json_response(data, code, headers)


def json_response(data, code, headers={}):
    serialized = json.dumps(data, indent=2)
    headers[b'Content-Type'] = 'application/json'
    return Response(serialized, status=code, headers=headers)


class JSONException(Exception):
    """A base exception class that is json serializable.

    Any controller that raise this exception will have it
    automatically logged and handled by the framework.
    """
    status_code = 400

    def as_dict(self):

        return {
            'error': str(self)
        }

    def as_response(self):
        resp = json_response(self.as_dict(), self.status_code)
        set_cors_into_headers(resp.headers, allow_origin=settings.DOMAIN)
        return resp


class JSONNotFound(JSONException):
    status_code = 404
