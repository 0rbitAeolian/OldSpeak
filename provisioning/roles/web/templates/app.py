#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import unicode_literals

import json


def app(environ, start_response):
    data = json.dumps({b'hello': b'world'})
    start_response("200 OK", [
        ("Content-Type", "application/json"),
        ("Content-Length", str(len(data)))
    ])
    return iter([data])
