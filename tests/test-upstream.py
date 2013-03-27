#!/usr/bin/env python
# Copyright 2013 Canonical Ltd.
#
# This file is part of u1db.
#
# u1db is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# as published by the Free Software Foundation.
#
# u1db is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with u1db.  If not, see <http://www.gnu.org/licenses/>.

import os, sys, unittest

try:
    from testtools import run
    import testscenarios
except:
    print('Required modules: python-testtools python-testscenarios')
    sys.exit(1)

try:
    import u1dbqt
except Exception:
    msg = sys.exc_info()[1] # Python 2/3 compatibility
    print(msg)
    print('u1dbqt Python wrapper not built? Did you run "make"?')
    sys.exit(1)

try:
    import u1db # Database, SyncTarget, Document, errors
    import u1db.tests
    from u1db.tests import test_backends
except:
    print('python-u1db required, with tests support')
    print('An easy way of doing that is getting lp:u1db and setting PYTHONPATH=/path/to/u1db')
    sys.exit(1)

# u1db-qt specific test cases

class TestQt(u1db.tests.DatabaseBaseTests):
    def setUp(self):
        super(TestQt, self).setUp()

    def test_sanity(self):
        self.assertTrue('Qt' in str(self))

# wrap Qt code in Python classes (via ui1dbqt Python module)
# cf. http://bazaar.launchpad.net/~pedronis/u1db/u1db-js/view/head:/tests/bridged.py

class QtDatabase(u1dbqt.Database):
    def __init__(self, replica_uid):
        u1dbqt.Database.__init__(self, replica_uid)

    def create_doc_from_json(self, json, doc_id=None):
        # FIXME create in db
        doc = u1db.Document(doc_id=doc_id)
        doc.set_json(json)
        return doc

    def put_doc (self, doc):
        u1dbqt.Database.put_doc(self, contents=doc.get_json(), doc_id=doc.doc_id)

    def delete_doc (self, doc):
        # Deleted means empty contents in the database
        self.put_doc(contents=None, doc_id=doc.doc_id)

    def get_docs (self):
        pass # TODO

    def get_all_docs (self):
        pass # TODO

    def close (self):
        pass # TODO

# TODO complete wrappers

def make_database(test, replica_uid):
    db = QtDatabase(replica_uid)
    def cleanup():
        pass # FIXME delete old databases
    test.addCleanup(cleanup)
    return db

# upstream Python test cases
# cf http://bazaar.launchpad.net/~pedronis/u1db/u1db-js/view/head:/tests/test_bridged.py

SCENARIOS = [("jsbridged", {
        "make_database_for_test": make_database,
        })]
class BridgedAllDatabaseTests(test_backends.AllDatabaseTests):
    scenarios = SCENARIOS

# TODO enable more cases

load_tests = u1db.tests.load_with_scenarios
if __name__ == '__main__':
    loader = unittest.TestLoader()
    suite = load_tests (loader, loader.loadTestsFromName(__name__), '*')
    unittest.TextTestRunner(verbosity=2).run(suite)
