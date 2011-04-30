import objcog_db
import couchdb

def create_db(db, couch):
    try:
        couch.create(db)
    except couchdb.http.PreconditionFailed, e:
        pass
    return couch[db]

########################################################################################################################

class Document(objcog_db._DocumentBase):
    TYPES = set([''])

    def __init__(self, db, meta_info):
        """ Base class
        """
        objcog_db._DocumentBase.__init__(self)
        self._db = db
        self._meta_info = meta_info
        self.doc_id, self.doc_rev = db.save(metainfo)
        self.doc = db[self.doc_id]

    def put(self, key,type_name, buffer):
        self.db.put_attachment(self.doc, buffer, key + type_name, "application/octet-stream") 

    def get(self, key,type_name):
        attachment = self.db.get_attachment(self.doc, key + type_name, None)
        return attachment.read()
    
    def persists(self, db = None):
        """ Persist an object to a database
        @param db db to persist the object to (the one from the constructor by default)
        """
        

########################################################################################################################

if __name__ == "__main__":
    couch = couchdb.Server() 
    documents = create_db("objcog/documents", couch)
    doc = Document(documents, {"type":"document", "tags":["hello", "bogus", "foos"]})
    objcog_db.use_document(doc)
    try:
        couch.delete("objcog/documents")
    except Exception, e:
        print e
