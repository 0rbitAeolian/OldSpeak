# -*- coding: utf-8 -*-
import os
import json
import pygit2
import logging
from collections import OrderedDict
from pygit2 import Repository
from pygit2 import GitError
from pygit2 import init_repository
from pygit2 import discover_repository

from oldspeak import settings


class BlobNotFound(Exception):
    pass


class GitNode(object):
    def __init__(self, path, ancestry=None, original_path=None):
        path = path.rstrip(os.sep) or '.'
        folder_name, file_name = os.path.split(path)
        self.path = folder_name or path
        self.name = file_name or path
        self.ancestry = ancestry or []
        self.original_path = original_path

    def __eq__(self, other):
        return isinstance(other, GitNode) and repr(other) == repr(self)

    def __repr__(self):
        classname = self.__class__.__name__
        name = self.name
        path = self.path
        return b'<{classname}(name={name})>'.format(**locals())


class GitFolder(GitNode):
    @property
    def is_root(self):
        return self.ancestry is None


class GitFile(GitNode):
    pass


class AutoTreeBuilder(object):
    def __init__(self, repo=None, author_name=None, author_email=None, message='auto saving'):
        self.repo = repo
        if not repo:
            return

        try:
            self.head = repo.head.target
        except GitError:
            self.head = None

        self.signature = None
        self.root_tree_id = None
        self.tree = None

        if author_email:
            self.signature = pygit2.Signature(author_name or author_email, author_email)

            if not self.head:
                self.tree = self.repo.TreeBuilder()
                self.root_tree_id = self.tree.write()
                self.head = self.repo.create_commit(
                    'refs/heads/master',
                    self.signature,
                    self.signature,
                    'initial commit',
                    self.root_tree_id,
                    []
                )
        if self.head:
            self.root_tree_id = self.repo.get(self.head).tree.id
            self.tree = self.repo.TreeBuilder(self.root_tree_id)

    def path_to_blob(self, path):
        parts = path.split(os.sep)
        ancestry = []

        for member in parts[:-1]:
            ancestry.append(GitFolder(member, ancestry, original_path=path))

        return GitFile(parts[-1], ancestry=ancestry, original_path=path)

    def write_blob(self, path, data, root_tree=None):
        node = self.path_to_blob(path)
        reverse_ancestry = [node]
        reverse_ancestry.extend(reversed(node.ancestry))

        if not root_tree:
            try:
                root_tree = self.repo.TreeBuilder(self.repo.get(self.head).tree.id)
            except:
                pass

        def make_tree():
            if root_tree:
                return self.repo.TreeBuilder(root_tree.write())
            else:
                return self.repo.TreeBuilder()

        blob = self.repo.create_blob(data)
        content = str(blob)

        trees = []

        for index, node in enumerate(reverse_ancestry):
            current_tree = make_tree()
            trees.append(current_tree)

            if index is 0:
                logging.warning('inserting blob {}'.format(node.name))
                current_tree.insert(node.name, content, pygit2.GIT_FILEMODE_BLOB)
                content = current_tree.write()
            else:
                logging.warning('inserting tree {}'.format(node.name))
                current_tree.insert(node.name, content, pygit2.GIT_FILEMODE_TREE)
                content = current_tree.write()

        if root_tree:
            root_tree.insert(node.name, current_tree.write(), pygit2.GIT_FILEMODE_TREE)
        elif not self.repo.head_is_unborn:
            root_tree = self.repo.TreeBuilder(self.repo.head.tree.id)
        else:
            root_tree = trees[-1]

        new_root_id = root_tree.write()
        parents = []
        if self.head:
            parents.append(self.head)

        if self.signature:
            self.head = self.repo.create_commit(
                'refs/heads/master',
                self.signature,
                self.signature,
                'changing {path}'.format(**locals()),
                new_root_id,
                parents,
            )

        return self.repo.get(new_root_id), self.repo.get(blob)


class GitRepository(object):
    def __init__(self, path, root_dir=None):
        self.relative_path = path
        self.root_dir = root_dir or "."
        self._current_tree = None
        self.commit_cache = []

    @property
    def path(self):
        return os.path.join(
            os.path.abspath(self.root_dir),
            self.relative_path
        )

    @classmethod
    def get(self, path):
        try:
            repo_path = discover_repository(path)
        except KeyError:
            return None

        return Repository(repo_path)

    @classmethod
    def create(cls, path, **kw):
        kw['bare'] = kw.pop('bare', False)
        return init_repository(path, **kw)

    def get_or_create(self):
        repo = self.get(self.path)
        if repo:
            return repo

        return self.create(self.path)

    @property
    def git(self):
        return self.get_or_create()

    @property
    def head(self):
        if self.git.head_is_unborn:
            return None

        return self.git.head.get_object()

    @property
    def tree(self):
        if self._current_tree:
            return self._current_tree

        elif self.head:
            self._current_tree = self.new_tree(self.head.tree.id)

        else:
            self._current_tree = self.new_tree()

        return self._current_tree

    def new_tree(self, *args, **kw):
        logging.warning('new_tree() {args} {kw}'.format(**locals()))
        return self.git.TreeBuilder(*args, **kw)

    def traverse_blobs(self, tree=None):
        if not tree and not self.head and not self.tree:
            raise StopIteration

        elif self.tree:
            tree = self.tree

        elif not tree:
            tree = self.head.tree

        elif tree.type == 'blob':
            entry = tree
            node = self.git.get(entry.hex)
            yield entry.name, OrderedDict([
                (b'type', entry.type),
                (b'size', node.size),
                (b'is_binary', node.is_binary),
            ])
        else:

            for entry in tree:
                node = self.git.get(entry.hex)
                if entry.type == 'blob':
                    yield entry.name, OrderedDict([
                        (b'type', entry.type),
                        (b'size', node.size),
                        (b'is_binary', node.is_binary),
                    ])

                elif entry.type == 'tree':
                    yield entry.name, OrderedDict([
                        (b'type', entry.type),
                        (b'size', len(node)),
                    ])
                    for key, value in self.traverse_blobs(entry):
                        yield key, value

    def __iter__(self):
        for k, v in self.traverse_blobs(self.head.tree):
            yield k, v

    @property
    def name(self):
        return os.path.basename(self.path)

    def commit(self, tree=None,
               author_name=None,
               author_email=None,
               message=None,
               reference_name=None):

        author_name = author_name or settings.VFS_PERSISTENCE_USER
        author_email = author_email or settings.VFS_PERSISTENCE_USER
        author = pygit2.Signature(author_name, author_email)

        if not self.git.head_is_unborn:
            parents = [self.git.head.target]
        else:
            parents = []

        tree_id = tree.id

        if not reference_name:
            reference_name = 'refs/heads/master'

        sha = self.git.create_commit(
            reference_name,
            author,
            author,
            message or 'auto-save',
            tree_id,
            parents
        )
        commit = self.git.get(sha)

        self.git.reset(sha, pygit2.GIT_RESET_HARD)

        self.commit_cache.append((sha, tree_id, parents))
        return commit


class Bucket(object):
    def __init__(self, path=None, author_name=None, author_email=None, *args, **kw):
        self.new(path, *args, **kw)
        self.path = path or self.get_path()
        self.repo = GitRepository(self.path, settings.OLDSPEAK_DATADIR)

        self.author_name = author_name or 'oldspeak'
        self.author_email = author_email or 'oldspeak@oldspeak'

    def write_file(self, name, data, message=None, author_name=None, author_email=None, **kw):
        commit_data = {
            'message': message or 'changing {name}'.format(**locals()),
            'author_name': author_name or self.author_name,
            'author_email': author_email or self.author_email,
        }
        writer = AutoTreeBuilder(self.repo.git)
        tree, blob = writer.write_blob(name, data)
        commit_data['tree'] = tree
        self.repo.commit(**commit_data)
        return blob

    def save(self, message=None, author_name=None, author_email=None, **kw):
        kw = {}
        if message:
            kw['message'] = message

        kw['author_name'] = author_name or self.author_name
        kw['author_email'] = author_email or self.author_email

        # commit = self.repo.commit(**kw)
        # return commit

    def resolve(self, oid):
        return self.repo.git.get(oid)

    def new(self, *args, **kw):
        pass

    def list(self):
        if not self.repo.head:
            return []

        data = []
        tree = self.repo.head.tree
        for patch in tree.diff_to_tree():
            data.append(patch.delta.new_file.path)

        return data


class System(Bucket):
    def new(self, child='core'):
        self.child = child

    def get_path(self):
        return '/'.join(filter(bool, ('system', self.child)))

    def add_fingerprint(self, fingerprint, email, parent_fingerprint, **kw):
        blob = self.write_file('fingerprints/{}.json'.format(fingerprint), json.dumps({
            'email': email,
            'fingerprint': fingerprint,
            'parent_fingerprint': parent_fingerprint,
        }, indent=4))
        return blob


class Member(Bucket):
    def new(self, fingerprint):
        if not fingerprint:
            raise RuntimeError('members require a fingerprint')

        self.fingerprint = fingerprint

    def get_path(self):
        return '/'.join(('fingerprint', self.fingerprint))
