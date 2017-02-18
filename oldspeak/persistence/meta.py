# -*- coding: utf-8 -*-
import re
import sys
from collections import OrderedDict

self = sys.modules[__name__]


class MetaRegistry(object):
    """registry for connection items"""

    def __init__(self, parent, new, default_key=None, get_default_key=None):
        if not callable(new):
            raise TypeError(
                'ParentRegistry(new) must be a callable: {new:repr}',
                **locals())

        self.parent = parent
        self.new = new
        self.__items__ = OrderedDict()
        if callable(get_default_key):
            default_key = get_default_key(parent)

        self.default_key = default_key

    def __getitem__(self, key):
        if key is None:
            key = self.default_key

        item = self.__items__.get(key, None)
        if item is None:
            item = self.new(key)
            self.__items__[key] = item

        return item

    def __setitem__(self, key, item):
        if key in self.__items__:
            msg = '{self} already contains a item key {key}: {item}'
            raise RuntimeError(msg.format(**locals()))

        self.__items__[key] = item
        return item


_HELPER_REGISTRY = MetaRegistry(self, OrderedDict)
_MODEL_REGISTRY = MetaRegistry(self, OrderedDict)
_CONNECTOR_REGISTRY = MetaRegistry(self, OrderedDict)


def is_valid_python_name(string=None):
    string = string or br''
    return re.match(r'^[a-zA-Z_][\w_]*$', string) is not None


class helpers(type):

    def __init__(cls, name, bases, attrs):
        if cls == helpers or name == 'ModelHelper':
            return super(helpers, cls).__init__(name, bases, attrs)

        ns = getattr(cls, '__ns__', None)
        if not isinstance(ns, basestring):
            raise TypeError(
                '{}.__ns__ must be a string, not {}'.format(cls, type(ns)))

        elif not is_valid_python_name(ns):
            raise TypeError(
                '{}.__ns__ must be a valid python variable name'.format(ns))

        handle = getattr(cls, '__handle__', None)

        if not isinstance(handle, basestring):
            raise TypeError(
                '{}.__handle__ must be a string, not {}'.format(
                    cls, type(handle)))

        elif not is_valid_python_name(handle):
            raise TypeError(
                '{}.__handle__ must be a valid python variable name'.format(ns))

        _HELPER_REGISTRY[name] = OrderedDict([
            ('ns', ns),
            ('handle', handle),
        ])

        return super(helpers, cls).__init__(name, bases, attrs)


class connector_meta(type):

    def __init__(cls, name, bases, attrs):
        if cls == connector_meta or name == 'base_connector':
            return super(connector_meta, cls).__init__(name, bases, attrs)

        service_name = getattr(cls, '__service__', None)
        if not isinstance(service_name, basestring):
            raise TypeError('{}.__service__ must be a string, not {}'.format(
                cls, type(service_name)))

        # create_pool = getattr(cls, 'create_pool', None)
        # if not callable(create_pool):
        #     raise TypeError('{}.create_pool must be a classmethod, not {}'.format(cls, type(create_pool)))

        # create_connection = getattr(cls, 'create_connection', None)
        # if not callable(create_connection):
        #     raise TypeError('{}.create_connection must be a classmethod, not {}'.format(cls, type(create_connection)))

        _CONNECTOR_REGISTRY[service_name] = cls
        # cls.__pools__ = MetaRegistry(cls, lambda alias: cls(alias).create_pool(alias))
        # cls.__connections__ = MetaRegistry(cls, lambda alias=settings.DEFAULT_CONNECTOR_ALIAS: cls(alias).create_connection(alias))
        return super(connector_meta, cls).__init__(name, bases, attrs)


class orm(type):

    def __init__(cls, name, bases, attrs):
        if not hasattr(cls, 'table'):
            return

        cls.__columns__ = {c.name: c.type.python_type
                           for c in cls.table.columns}
        setattr(orm, name, cls)
        super(orm, cls).__init__(name, bases, attrs)
