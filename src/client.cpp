/*
 * Copyright (c) 2011, Willow Garage, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Willow Garage, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
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

    void store_impl(const std::string& key, const std::string& type_name, const std::string& meta,  const char* data, size_t length)
    {
      try
      {
        //build a bson object
        mongo::BSONObjBuilder b;
        mongo::BSONObj meta_obj = mongo::fromjson(meta);
        b.appendElements(meta_obj);
        b.append("__meta_data__", meta_obj);
        b.appendBinData("__binary_data__",length, mongo::ByteArray,data);
        connection_.insert(resolveFullCollectionName(key,type_name), b.obj());
        connection_.ensureIndex(resolveFullCollectionName(key,type_name), meta_obj);
        return;
      }
      catch (const mongo::DBException& e)
      {
        throw std::runtime_error(std::string("Failed to insert data : ") + e.what());
      }
    }

    void retrieve_impl(const std::string& key, const std::string& type_name, std::string query, DbClient::unblob_sig on_each)
    {
      mongo::BSONObj mongo_query = mongo::fromjson(query);
      boost::scoped_ptr<mongo::DBClientCursor> cursor(connection_.query(resolveFullCollectionName(key,type_name), mongo_query));
      while (cursor->more())
      {
        mongo::BSONObj p = cursor->next();
        mongo::BSONObj meta = p.getObjectField("__meta_data__");
        mongo::BSONElement blob = p.getField("__binary_data__");

        int length;
        const char * b = blob.binData(length);
        on_each(meta.toString(), b, length);
      }
    }

    void drop_impl()
    {
      BOOST_FOREACH( std::string x, connection_.getCollectionNames(getNamespace()))
          connection_.dropCollection(x);
      connection_.dropDatabase(getNamespace(),0);
    }

    std::string resolveFullCollectionName(const std::string& key, const std::string& type_name)
    {
      //only letters, numbers and underscores
      static const boost::regex e("^[a-zA-Z0-9_]*$");
      if (!boost::regex_match(key, e))
        throw std::runtime_error("bad key : " + key);
      return str(boost::format("%s.%s%s") % getNamespace() % key % type_name);
    }

    mongo::DBClientConnection connection_;
  };

  boost::shared_ptr<DbClient> createClient(std::string name_space, const std::string& host_name, DbType dbtype)
  {
    switch (dbtype)
    {
      case FilesystemDB:
        throw std::logic_error("FilesystemDB not implemented");
      case MongoDB:
        return boost::shared_ptr<DbClient>(new DbClientMongoDB(name_space, host_name));
        break;
    }
    return  boost::shared_ptr<DbClient>();
  }

}
