import objcog_db
import couchdb

def create(db, couch):
    try:
        couch.create(db)
    except couchdb.http.PreconditionFailed, e:
        pass
    return couch[db]

class Load(objcog_db._LoadBase):
    def __init__(self, db, metainfo):
        objcog_db._LoadBase.__init__(self)
        self.db = db
        self.metainfo = metainfo
        self.doc_id,self.doc_rev = db.save(metainfo)
        self.doc = db[self.doc_id]
        
    def add(self, key, buffer):
#        print "Adding a blob..."
#        print "key", key
#        print "buffer length: ", len(buffer)
        self.db.put_attachment(self.doc,buffer,key,"application/octet-stream") 
                
    def get(self, key):
        attachment = self.db.get_attachment(self.doc,key,None)
        return attachment.read()


class TrainingSession(objcog_db._TrainingSessionBase):
    def __init__(self, db, loads_db, metainfo):
        objcog_db._TrainingSessionBase.__init__(self)
        self.db = db
        self.loads_db = loads_db
        self.metainfo = metainfo
        self.doc_id,self.doc_rev = db.save(metainfo)
        self.doc = db[self.doc_id]
        self.count = 0
    def makeLoad(self):
        count = self.count
        self.count += 1
        metainfo = {"session_id":self.doc_id,"stamp":count}
        load = Load(self.loads_db,metainfo)
        self.append(load)
        return load

if __name__ == "__main__":

    couch = couchdb.Server() 
    sessions = create("objcog/sessions",couch)
    loads = create("objcog/loads",couch)
    s = TrainingSession(sessions,loads,{"session_type":"tod"})
    objcog_db.use_session(s)
    l = s.makeLoad()
    objcog_db.use_load(l)
    try:
        couch.delete("objcog/sessions")
        couch.delete("objcog/loads")
    except Exception, e:
        print e
