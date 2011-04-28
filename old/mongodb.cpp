#include <objcog/db/client.hpp>
#include <mongo/client/dbclient.h>
#include <boost/foreach.hpp>
namespace objcog
{
  struct DbClientMongoDB : public DbClient
  {
    DbClientMongoDB(const std::string& name_space, const std::string& host_name) :
      DbClient(name_space, host_name)
    {
    }

  protected:
    void connect_impl()
    {
      try
      {
        connection_.connect(getHostname());
      }
      catch (const mongo::DBException &e)
      {
        throw std::runtime_error("Could not connect to " + getHostname() + " with error: " + e.what());
      }
    }

    void store_impl(const std::string& key, const std::string& type_name, const std::string& meta, const char* data,
                    size_t length)
    {
      try
      {
        //build a bson object
        mongo::BSONObjBuilder b;
        mongo::BSONObj meta_obj = mongo::fromjson(meta);
        b.appendElements(meta_obj);
        b.append("__meta_data__", meta_obj);
        b.appendBinData("__binary_data__", length, mongo::ByteArray, data);
        connection_.insert(resolveFullCollectionName(key, type_name), b.obj());
        connection_.ensureIndex(resolveFullCollectionName(key, type_name), meta_obj);
        return;
      }
      catch (const mongo::DBException& e)
      {
        throw std::runtime_error(std::string("Failed to insert data : ") + e.what());
      }
    }

    struct MongoCursor : Cursor_impl
    {

      MongoCursor(std::auto_ptr<mongo::DBClientCursor> cursor) :
        cursor(cursor)
      {
        if (this->cursor->more())
        {
          p = this->cursor->next();
          ended = false;
        }
        else
          ended = true;
      }

      ~MongoCursor()
      {
      }

      std::pair<const char*, size_t> getData()
      {
        if (end())
          throw std::logic_error("This iterator is past the end...");
        std::pair<const char*, size_t> data;
        int length;
        blob = p.getField("__binary_data__");
        data.first = blob.binData(length);
        data.second = length;
        return data;
      }
      std::string getMetaData()
      {
        if (end())
          throw std::logic_error("This iterator is past the end...");
        mongo::BSONObj meta = p.getObjectField("__meta_data__");
        return meta.toString();
      }
      Cursor_impl& operator++()
      {
        if (cursor->more())
        {
          p = cursor->next();
          ended = false;
        }
        else
          ended = true;
        return *this;
      }
      virtual bool end()
      {
        return ended;
      }
      bool ended;
      std::auto_ptr<mongo::DBClientCursor> cursor;
      mongo::BSONObj p;
      mongo::BSONElement blob;
    };

    boost::shared_ptr<Cursor_impl> query_impl(const std::string& collection_key, const std::string& type_name,
                                              std::string query)
    {
      mongo::BSONObj mongo_query = mongo::fromjson(query);
      boost::shared_ptr<Cursor_impl> c(
                                       new MongoCursor(
                                                       connection_.query(
                                                                         resolveFullCollectionName(collection_key,
                                                                                                   type_name),
                                                                         mongo_query)));
      return c;
    }

    void drop_impl()
    {
      BOOST_FOREACH( std::string x, connection_.getCollectionNames(getNamespace()))
              connection_.dropCollection(x);
      connection_.dropDatabase(getNamespace(), 0);
    }

    std::string resolveFullCollectionName(const std::string& key, const std::string& type_name) const
    {
      //only letters, numbers and underscores
      static const boost::regex e("^[a-zA-Z0-9_]*$");
      if (!boost::regex_match(key, e))
        throw std::runtime_error("bad key : " + key);
      return str(boost::format("%s.%s%s") % getNamespace() % key % type_name);
    }

    mongo::DBClientConnection connection_;
  };
  boost::shared_ptr<DbClient> mongodb_client(const std::string& name_space, const std::string& host_name)
  {
    return boost::shared_ptr<DbClient>(new DbClientMongoDB(name_space, host_name));
  }
}
