# -*- coding: utf-8 -*-
import io
import hashlib
from collections import OrderedDict
from flask import request
from flask import url_for

from oldspeak import settings
from oldspeak.http.core import html
from oldspeak.http.core import redirect
from oldspeak.http.core import generate_token
from oldspeak.http.core import generate_seed

from oldspeak.http.core import ServerComponent

from oldspeak.crypt0 import GPGKeyChain
from oldspeak.crypt0 import generate_temp_token

gpg = GPGKeyChain(settings.OLDSPEAK_DATADIR)


root = ServerComponent('root')


def reactjs_page(path, oldspeak_token=None, seed=None):
    oldspeak_token = oldspeak_token or generate_token()
    seed = seed or generate_seed(path)
    return html('index.html', context=locals())


REACT_ROUTES = OrderedDict()


root.direct_html('index', '/')

root.direct_html('join', '/join')
root.direct_html('2fa', '/2fa')
root.direct_html('login', '/login')
root.direct_html('logout', '/logout')

root.direct_html('drafts', '/drafts')
root.direct_html('essays', '/essays')

root.direct_html('roster', '/roster')
root.direct_html('settings', '/settings')


@root.get('/login/pk')
def login_pk_redirect():
    return redirect(url_for('root.login'))


@root.get('/join/pk')
def join_pk_redirect():
    return redirect(url_for('root.join'))


@root.post('/login/pk')
def login_form_gpg():
    return redirect(
        url_for('root.2fa'), cookies={
            'token': generate_temp_token()})


def generate_session_id(salt=None):
    salt = salt or settings.SECRET_KEY
    return ''.join([generate_temp_token(64).encode(
        'rot13'), hashlib.md5(salt).hexdigest()])


@root.route('/join/pk', methods=['POST'])
def join_form_gpg():
    context = {}
    pubkey = request.form.get('pubkey')
    if not pubkey:
        context['error'] = 'missing public key'
        return html('login.html', code=401, context=context)

    with io.BytesIO(pubkey.encode('utf-8').strip()) as fd:
        result = gpg.import_key(fd)

    fingerprints = [fp for (fp, _, _) in result.imports]
    keys = [gpg.key(fp) for fp in fingerprints]
    if keys:
        password = generate_temp_token(96)

        session_id = generate_session_id()
        print keys, password, session_id
        return redirect(
            url_for('root.2fa'), cookies={
                'session_id': session_id})

    return html(
        'debug.html',
        context={
            'error': {
                'title': 'Invalid Public GPG Key',
                'message': pubkey}})


@root.get('/0rb1t')
def orbit():
    return html('0rb1t.txt', headers={'Content-Type': 'text/plain'})
