# -*- coding: utf-8 -*-
from __future__ import absolute_import
import json

from requests import Session
from functools import partial
from collections import OrderedDict
from requests.structures import CaseInsensitiveDict
from oldspeak.core import get_logger
from oldspeak.lib.functions import utf8


class ClientResponse(object):

    def __init__(
            self,
            headers,
            data=None,
            status_code=None,
            oauth_token=None,
            response=None):
        self.headers = CaseInsensitiveDict(headers)
        self.data = data
        self.status_code = status_code
        self.oauth_token = oauth_token
        self.response = response
        self.cookies = OrderedDict(response.cookies.items())
        self.logger = get_logger(__name__)

    @classmethod
    def from_response(cls, response):
        return cls(
            headers=response.headers,
            data=response.content,
            status_code=response.status_code,
            response=response)

    def to_python(self):
        result = {}
        result['status_code'] = self.status_code
        result['oauth_token'] = self.oauth_token
        result['headers'] = OrderedDict(self.response.headers)
        result['cookies'] = OrderedDict(self.cookies)

        try:
            result['data'] = json.loads(self.data)
        except ValueError:
            self.logger.debug('failed to load JSON data from response')
            result['data'] = {
                'data': utf8(self.data),
            }

        return result


class InvalidServerURL(Exception):

    def __init__(self, url):
        self.url = url
        msg = 'invalid server url: {url}'
        super(InvalidServerURL, self).__init__(msg.format(**locals()))


class OldSpeakClient(object):

    def __init__(self, base_url, session=None, **kw):
        super(OldSpeakClient, self).__init__()
        self.base_url = base_url.rstrip('/')
        self.headers = dict()
        self.session = session or Session()
        self.oauth_token = None
        self.private_key = kw.pop('private_key', None)
        self.public_key = kw.pop('public_key', None)

        for http_method in ('GET', 'POST', 'PUT', 'DELETE', 'HEAD', 'PATCH'):
            setattr(OldSpeakClient, http_method.lower(),
                    partial(self.request, http_method))

    def request(self, method, url, body=None, headers=None, **kw):
        if url.startswith('/'):
            url = "/".join([self.base_url, url.lstrip('/')])
        elif not url.startswith('http'):
            raise InvalidServerURL(url)

        headers = self.make_headers(headers)
        kw['headers'] = headers
        kw['data'] = body

        response = self.session.request(method.upper(), url, **kw)
        return self.wrap_response(response)

    def make_headers(self, headers=None, cookies=None):
        headers = CaseInsensitiveDict(
            isinstance(headers, dict) and headers or {})
        if 'Content-Type' not in headers:
            headers['Content-Type'] = 'application/json'

        if self.oauth_token:
            headers['Authorization'] = 'oauth2: {}'.format(self.oauth_token)

        return dict(headers)

    def wrap_response(self, response):
        return ClientResponse.from_response(response)

    def sign_up(self, public_key):
        return self.request(
            'post', '/join', data={'public_key': public_key},
            headers={'Content-Type': 'multipart/form-data'})
