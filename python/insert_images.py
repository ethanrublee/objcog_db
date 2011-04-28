from document import *
import objcog_db
import os

from optparse import OptionParser

def insert_session(sessions, training_path, meta_info):
    session = {"type":"session", "tags":["images"], "path_origin": training_path, "meta": meta_info}
    doc_id, doc_rev = sessions.save(session)
    doc = sessions[doc_id]
    return (doc, doc_id)

def create_session_frame(documents, session_id):
    doc_meta = {"type":"frame", "session_id":session_id}
    doc = Document(documents,doc_meta)
    return doc
    
if __name__ == '__main__':
    parser = OptionParser(description='Insert all images into the db from a specific folder.')
    parser.add_option("-p", "--training_path", dest="training_path",
                  help="The path to all the training bags", metavar="PATH_TO_IMAGES",
                   default='./')
    parser.add_option("-e", "--extension", dest="extension",
                  help="The extension", metavar="IMAGE_TYPE_EXTENSION",
                   default='.png')
    parser.add_option("-m", "--meta_info", dest="meta_info",
                  help="The meta info for the given object.", metavar="META_INFO_JSON_STRING",
                   default="Images from training path.")

    (options, args) = parser.parse_args()
    print "Training directory is %s" % options.training_path
    print "Meta info is %s" % options.meta_info
    idx = 0
    
    couch = couchdb.Server()
    
    sessions = create_db("objcog/sessions", couch)

    documents = create_db("objcog/documents", couch)
    
    (session, session_id) = insert_session(sessions,options.training_path,options.meta_info)
    
    for file in sorted(os.listdir(options.training_path)):
        if(file.endswith(options.extension)):
          print "putting %s" % file
          frame = create_session_frame(documents,session_id)
          objcog_db.insert_image(os.path.join(options.training_path,file),"image",frame)
          idx += 1

