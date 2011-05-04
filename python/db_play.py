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


import couchdb
from uuid import uuid4


couch = couchdb.Server()

def create(db):
    try:
        couch.create(db)
    except couchdb.http.PreconditionFailed, e:
        pass
    return couch[db]

if __name__ == '__main__':
    db1 = create("fellows/bed")
    db2 = create("fellows")
    for x in couch:
        print x
    print db1
    print db2
    
    doc = {"meta": "data", "binary":"blob"}
    doc_id = uuid4().hex #generate doc id on the client side.
    db1[doc_id] = doc
    for doc_id in db1:
        print db1[doc_id]
    couch.delete("fellows/bed")
    couch.delete("fellows")
    db = create('python-tests')
    try:
        db['johndoe'] = dict(type='Person', name='John Doe')
        db.put_attachment(db['johndoe'],{"hello":"there"}, "hello_file")
        db['maryjane'] = dict(type='Person', name='Mary Jane')
        db['gotham'] = dict(type='City', name='Gotham City')
        map_fun = '''function(doc) {
             if (doc.type == 'Person')
                 emit(doc.name, doc);
         }'''
         
        for row in db.query(map_fun):
            print row.key
            print row.value
            doc_id = row.value['_id']
            attachment = db.get_attachment(doc_id,'hello_file',None)
            #if attachment:
             #   print attachment.read(-1)
    except Exception,e:
        print e
     
    #couch.delete("python-tests")