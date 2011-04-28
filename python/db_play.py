import couchdb
from uuid import uuid4


couch = couchdb.Server()

def create(db):
    try:
        couch.create(db)
    except couchdb.http.PreconditionFailed, e:
        pass
    return couch[db]

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
        if attachment:
            print attachment.read(-1)
except Exception,e:
    print e
 
couch.delete("python-tests")