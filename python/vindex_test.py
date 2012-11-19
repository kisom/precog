#!/usr/bin/env

import os
import sets
import unittest
import vindex
from Crypto.Hash import SHA


class ValueBucketTests(unittest.TestCase):
    def setUp(self):
        self.default_value = "hello, world"
        self.default_hash = SHA.new(data=self.default_value).hexdigest()
        self.vb = vindex.ValueBucket('hello, world', 'foo')

    def test_00(self):
        print 'dumping basic bucket'
        try:
            self.vb.dump()
        except Exception as exc:
            print exc
            self.assertTrue(exc == None)
        else:
            self.assertTrue(os.path.exists(self.vb.fname()))

    def test_01(self):
        print 'loading basic bucket'
        loaded_vb = vindex.load_value(self.default_hash)
        self.assertTrue(self.vb.name == loaded_vb.name)
        self.assertTrue(self.vb.value == loaded_vb.value)
        key_set = frozenset(self.vb.keys)
        loaded_key_set = frozenset(loaded_vb.keys)
        difference = key_set ^ loaded_key_set
        self.assertTrue(len(difference) == 0)

    def test_02(self):
        print 'checking keys in stored bucket'
        self.assertTrue(vindex.check('foo', value=self.default_value))
        self.assertTrue(vindex.check('foo', value_hash=self.default_hash))
        self.assertFalse(vindex.check('bar', value=self.default_value))
        self.assertFalse(vindex.check('bar', value_hash=self.default_hash))


if __name__ == '__main__':
    suite   = unittest.TestSuite()
    loader  = unittest.TestLoader()
    suite.addTests(loader.loadTestsFromTestCase(ValueBucketTests))
    unittest.TextTestRunner(verbosity = 2).run(suite)
