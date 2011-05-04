#!/usr/bin/env/python
# Software License Agreement (BSD License)
#
# Copyright (c) 2008, Willow Garage, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided
#    with the distribution.
#  * Neither the name of the Willow Garage nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

# Author: Vincent Rabaud

import objcog_db
from object_db import ObjectDB

########################################################################################################################

class Document(objcog_db._DocumentBase):
    """ Virtual base class for every DB document
    """
    _attributes = set()
    
    def __init__(self, db, tags=[] , meta_info={}):
        """ Base class
        """
        objcog_db._DocumentBase.__init__(self)
        self._db = db
        self._tags = tags
        self._meta_info = meta_info

        # info we can only know after persistence
        self._doc_id = None
        self._doc_rev = None

        # The attributes for the Type, that will be persisted to the db
        self._attributes = {}

        # the db document itself
        self._doc = None

        # dependencies: when persisting the object, it will refer to those
        self._dependencies = []

        # the attributes of an object
        self._attributes = {}

        # flag indicating whether we have persisted the object
        self._is_persisted = False

    def put(self, key, type_name, buffer):
        if not self._is_persisted:
            self.persist()
        self.db.put_attachment(self._doc, buffer, key + type_name, "application/octet-stream")

    def get(self, key, type_name):
        if not self._is_persisted:
            self.persist()
        attachment = self.db.get_attachment(self._doc, key + type_name, None)
        return attachment.read()

    def persist(self, db=None):
        """ Persist an object to a database
        @param db db to persist the object to (the one from the constructor by default)
        """
        if db is None:
            db = self._db
        document = {'type':self.type(), 'tags':self._tags, 'meta': self._meta_info}

        # make sure all the dependencies are persisted
        for dependency in self._dependencies:
            dependency.persist(db)

        # add info proper to the specific object
        document.update(self.data())

        self._doc_id, self._doc_rev = db.save(document)
        self._doc = db[self._doc_id]
        self._is_persisted = True

    def data(self):
        """ Virtual function returning a JSON dictionary of what to persist in the db
        """
        print 'Need to implement that class for children'
        raise

    def set_attributes(self, attributes):
        """ Used to set the attributes to store of an object. This is proper to a type
        """
        for key, val in attributes.iteritems():
            if key in self._attributes:
                self._attributes[key] = val

    @staticmethod
    def type():
        """ Virtual static function returning the type of a class
        This is how it will be stored in the DB
        """
        print 'Need to implement that class for children'
        raise

########################################################################################################################

class Session(Document):
    """
    Class defining a session: a set of homogeneous data (same object, person, conditions
    """
    _attributes = set()
    
    def __init__(self, db, tags=[] , meta_info={}):
        """ Define the different ids of the session
        @param tags list of tags
        @param meta_info a string that can be whatever meta info (json, xml ...)
        """
        Document.__init__(self, db, tags, meta_info)

    def data(self):
        return {}

    @staticmethod
    def type():
        return 'session'

########################################################################################################################

class Frame(Document):
    """
    Class defining a session: a set of homogeneous data (same object, person, conditions
    """
    def __init__(self, db, tags=[] , meta_info={}):
        """ Define the different ids of the session
        @param tags list of tags
        @param meta_info a string that can be whatever meta info (json, xml ...)
        """
        Document.__init__(self, db, tags, meta_info)

    def data(self):
        return {'path_origin': self._attributes['training_path']}

    def set_image(self, file):
        """
        """
        self.put('image', 'image', file)

    @staticmethod
    def type():
        return 'frame'
