# -*- coding: utf-8 -*-
from __future__ import absolute_import
from collections import OrderedDict
from oldspeak.lib.functions import normalize_http_header_name

Enum = type('Enum', (tuple, ), {
    '__init__': lambda s, i: [
        setattr(s, k, 1 << x)
        for x, k in enumerate(i)
    ] and tuple.__init__(s, i),
})


class HttpDict(OrderedDict):
    """an OrderedDict that enforces appropriate http header keys"""

    def __setitem__(self, key, value):
        return super(HttpDict, self).__setitem__(
            normalize_http_header_name(key),
            value,
        )

    def __getitem__(self, key):
        return super(HttpDict, self).__getitem__(
            normalize_http_header_name(key),
        )

    def keys(self):
        return map(normalize_http_header_name, super(HttpDict, self).keys())
