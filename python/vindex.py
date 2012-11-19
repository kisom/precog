"""
Inverted index implementation of the value store.
"""

import os
import struct
import types
from Crypto.Hash import SHA

HASHFUN = SHA
INDEX_DIR = '.index'


class InvalidLookup(Exception):
    """
    Exception raised when an invalid lookup occurs.
    """
    pass


class ValueBucket:
    """
    A value bucket is an individual value representation in the inverted
    index.
    """
    def __init__(self, value, keys):
        """
        Initialise a new bucket for value with an initial key. There is
        no reason to store a value without a key, and a ValueBucket is
        only created when a value is stored to a key.
        """
        self.name = SHA.new(data=value).hexdigest()
        self.value = value
        if isinstance(keys, types.StringTypes):
            self.keys = [keys, ]
        else:
            self.keys = keys

    def has(self, keyname):
        """
        Determine whether the value bucket has a given key.
        """
        return keyname in self.keys

    def fname(self):
        """
        Generate the filename for the bucket.
        """
        fname = self.name[:2] + os.path.sep + self.name[2:]
        fname = os.path.join(INDEX_DIR, fname)
        return fname

    def dump(self):
        """
        Write the bucket to disk.
        """
        fmt = '>q%ds' % (len(self.value), )
        raw = struct.pack(fmt, len(self.value), self.value)
        for key in self.keys:
            fmt = '>q%ds' % (len(key), )
            raw += struct.pack(fmt, len(key), key)
        fname = self.fname()
        if not os.path.exists(INDEX_DIR):
            os.mkdir(INDEX_DIR)
        if not os.path.exists(os.path.join(INDEX_DIR, self.name[:2])):
            os.mkdir(os.path.join(INDEX_DIR, self.name[:2]))
        with open(fname, 'w+') as bucket:
            bucket.write(raw)

    def add(self, keyname):
        """
        Associate a key with this value.
        """
        if not keyname in self.keys:
            self.keys.append(keyname)

    def remove(self, keyname):
        """
        Deassociate a key with this value.
        """
        if keyname in self.keys:
            self.keys.remove(keyname)
        if len(self.keys) == 0:
            self.destroy()

    def destroy(self):
        """
        Remove the bucket from the store.
        """
        fname = self.fname()
        os.unlink(fname)
        del(self)


def load_value(value_hash):
    """
    Load a value from a value's hash.
    """
    bucket_filename = os.path.join(INDEX_DIR, value_hash[:2],
                                   value_hash[2:])
    value_bucket = None
    with open(bucket_filename) as bucket:
        value_len = int(struct.unpack('>q', bucket.read(8))[0])
        value = struct.unpack('>%ds' % (value_len,), bucket.read(value_len))
        value = value[0]
        key_list = []
        while not bucket.read(1) == "":
            bucket.seek(-1, os.SEEK_CUR)
            keylen = int(struct.unpack('>q', bucket.read(8))[0])
            key = struct.unpack('>%ds' % (keylen,), bucket.read(keylen))[0]
            key_list.append(key)
        value_bucket = ValueBucket(value, key_list)
    return value_bucket


def __read_keys_from_value__(value_hash):
    bucket_filename = os.path.join(INDEX_DIR, value_hash[:2],
                                   value_hash[2:])
    value_bucket = None
    key_list = []
    with open(bucket_filename) as bucket:
        value_len = int(struct.unpack('>q', bucket.read(8))[0])
        bucket.seek(value_len, os.SEEK_CUR)
        while not bucket.read(1) == "":
            bucket.seek(-1, os.SEEK_CUR)
            keylen = int(struct.unpack('>q', bucket.read(8))[0])
            key = struct.unpack('>%ds' % (keylen,), bucket.read(keylen))[0]
            key_list.append(key)
    return key_list


def check(key, value=None, value_hash=None):
    """
    Determine whether a key and value are associated.
    """
    if value and value_hash:
        raise InvalidLookup("cannot specify both a value and a hash")
    if value and not value_hash:
        value_hash = SHA.new(data=value).hexdigest()

    keys = __read_keys_from_value__(value_hash)
    return key in keys


def lookup(value_hash):
    """
    Given a value hash, retrieve the full value.
    """
    vb = load_value(value_hash)
    return vb.value
