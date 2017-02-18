# -*- coding: utf-8 -*-
try:
    from io import BytesIO
except ImportError:
    from StringIO import StringIO as BytesIO


class Buffer(BytesIO):
    pass


class AutoBuffer(Buffer):

    def __init__(self, callback):
        super(BytesIO, self).__init__()
        callback(self)
