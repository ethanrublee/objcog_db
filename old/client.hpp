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
#pragma once

#include <iostream>
#include <sstream>

#include <boost/format.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <boost/function.hpp>
#include <boost/regex.hpp>

namespace objcog
{

  struct Cursor_impl
  {
    virtual ~Cursor_impl()
    {
    }
    virtual std::pair<const char*, size_t> getData() = 0;
    virtual std::string getMetaData() = 0;
    virtual Cursor_impl& operator++() = 0;
    virtual bool end() = 0;
  };

  template<typename DataT>
    struct Cursor
    {
      typedef boost::archive::binary_iarchive ia;
      Cursor(boost::shared_ptr<Cursor_impl> impl) :
        cursor_impl(impl), data_read(false), meta_read(false)
      {
      }
      const DataT& getData()
      {
        if (!data_read)
        {
          std::pair<const char*, size_t> bd = cursor_impl->getData();
          //fill the stringbuf with data.
          std::stringbuf sb;
          sb.pubsetbuf(const_cast<char*> (bd.first), bd.second);
          sb.pubseekpos(0);
          // create and open an archive for input
          ia ia(sb);
          // read class state from archive
          ia >> data;
          data_read = true;
        }
        return data;
      }
      const std::string& getMetaData()
      {
        if (!meta_read)
        {
          meta_data = cursor_impl->getMetaData();
          meta_read = true;
        }
        return meta_data;
      }
      Cursor& operator++()
      {
        meta_read = false;
        data_read = false;
        ++(*cursor_impl);
        return *this;
      }
      bool end()
      {
        return cursor_impl->end();
      }
    private:
      boost::shared_ptr<Cursor_impl> cursor_impl;
      bool data_read; //populate on read
      bool meta_read; //populate on read
      DataT data;
      std::string meta_data;
    };

  /**
   * \brief the objcog database client interface, uses boost::serialization and templates
   * for typesafe extensible blob storage.
   */
  struct DbClient
  {
  public:
    typedef boost::archive::binary_iarchive ia;//!< \brief The input archive type. Binary until convinced otherwise.
    typedef boost::archive::binary_oarchive oa;//!< \brief The output archive type. Binary until convinced otherwise.
    /**
     * \brief The signature for unblobbing data from the db.
     */
    typedef boost::function<void(const std::string& meta_data, const char* data, size_t length)> unblob_sig;

    /**
     * \brief A template detail structure, that gives a typedef for a data callback signature.
     * \tparam DataT The datatype that determines the callback signature.
     */
    template<typename DataT>
      struct callback
      {
        /**A callback function, that the unblobber will thunk to, after deserialization
         */
        typedef boost::function<void(const std::string& meta, const DataT& data)> signature;
      };

    /**
     * \brief virtual destructor, as this is an abstract interface.
     */
    virtual ~DbClient()
    {
    }

    /**
     * \brief Connect to the database, this should be called before anything else.
     * This may throw, if the connection fails.
     */
    void connect()
    {
      connect_impl();
    }

    /**
     * \brief Drop the database that this client is pointing to, meaning everything under the
     * namespace.
     * FIXME make this throw on permission issues
     */
    void drop()
    {
      drop_impl();
    }

    /**
     * \brief Store data, by type and a key, with meta info for indexing purposes.
     * @tparam DataT The data type to retrieve.
     * @param collection_key Data is stored in unique collections based on this key and the typename.
     * @param meta_info A JSON string representing the meta info for this blob.
     * @param data This object will be persisted as a binary blob in the database, using boost::serialization
     */
    template<typename DataT>
      void store(const std::string& collection_key, const std::string& meta_info, const DataT& data)
      {
        store<DataT, oa> (collection_key, meta_info, data);
      }

    /**
     * \brief Retrieve data from the database given a type and query.
     * The on_each callback will be called for every result returned.
     * @tparam DataT The data type to retrieve.
     * @param collection_key The key, one that was used during store.
     * @param query A JSON string query (reference http://www.mongodb.org/display/DOCS/Querying)
     * @param on_each This is of the form : void(const std::string& meta, const DataT& data), and will be called with the
     *        meta info and a const reference to the data.
     */
    template<typename DataT>
      void retrieve(const std::string& collection_key, const std::string& query,
                    typename callback<DataT>::signature on_each)
      {
        retrieve<DataT, ia> (collection_key, query, on_each);
      }

    template<typename DataT>
      Cursor<DataT> query(const std::string& collection_key, const std::string& query)
      {
        return Cursor<DataT> (query_impl(collection_key, typeid(DataT).name(), query));
      }

    /**
     * \brief get the namespace for this client
     * @return a string, "objcog", "my_awesome_database"
     */
    const std::string& getNamespace() const
    {
      return namespace_;
    }
    /**
     * \brief get the hostname for this client
     * @return a string, "localhost", "10.0.1.20", "dri.local"
     */
    const std::string& getHostname() const
    {
      return host_name_;
    }
    /**
     * \brief Some predefined database backends
     */
    enum DbType
    {
      MongoDB = 1, //!< MongoDB Uses mongodb on the backend, namespace should be a simple name, hostname localhost, ip
      FilesystemDB = 2,     //!< FilesystemDB Uses filesystem for the database backend.
      CouchDB = 3, //!< CouchDB couchdb based db.
    };

    /**
     * \brief A client factory, meant to hide implementation details from the user.
     * @param name_space The namespace for the database, think 'objcog' or '/home/database'
     * @param host_name localhost, or 10.0.1.13, etc.
     * @param dbtype The database backend specifier.
     * @return
     */
    static boost::shared_ptr<DbClient> createClient(const std::string& name_space, const std::string& host_name,
                                                    DbType dbtype);
  protected:

    /**
     * @param name_space The namespace for the database, think 'objcog' or '/home/database'
     * @param host_name localhost, or 10.0.1.13, etc.
     */
    DbClient(const std::string& name_space, const std::string& host_name) :
      namespace_(name_space), host_name_(host_name)
    {
    }
    /**
     * \brief Called at connection time. Should do all necessary work to make the client ready
     * for action. May throw exceptions.
     */
    virtual void connect_impl() = 0;

    /**
     * \brief the store impl. This will be called with all meta information and std::string that is the
     * binary blob to store.
     * @param collection_key A collection name, e.g. "images", where this is expected to lie under the
     *                       namespace for this database
     * @param type_name A type_name, that should be used for differentiating blobs in the collection_key. This is what
     *                  enforces the type safety, so make sure to use it!
     * @param meta A json string that should be indexed on.
     * @param data The binary blob.
     * @param length The length in bytes of the binary blob.
     */
    virtual void store_impl(const std::string& collection_key, const std::string& type_name, const std::string& meta,
                            const char* data, size_t length) = 0;

    /**
     * \brief Given a query this should find the meta data and blob and then pass these to the unblob_sig on_each method.
     * @param collection_key
     * @param type_name A type_name, that should be used for differentiating blobs in the collection_key
     * @param query a json query
     * @param on_each void(std::string, const char* data, size_t length)
     */
    virtual boost::shared_ptr<Cursor_impl> query_impl(const std::string& collection_key, const std::string& type_name,
                                                      std::string query) = 0;

    /**
     * Drop the entire table, under namespace.
     */
    virtual void drop_impl() = 0;

  private:

    template<typename DataT, typename archive>
      void store(const std::string& collection_key, const std::string& meta_info, const DataT& data)
      {
        //get a binary blob for the data
        std::stringbuf sb;
        archive oa(sb);
        oa << data;
        std::string data_str = sb.str();
        store_impl(collection_key, typeid(DataT).name(), meta_info, data_str.c_str(), data_str.size());
      }

    template<typename DataT, typename archive>
      void retrieve(const std::string& collection_key, const std::string& q,
                    typename boost::function<void(const std::string& meta, const DataT&)> on_each)
      {
        Cursor<DataT> cursor(query<DataT> (collection_key, q));
        while (!cursor.end())
        {
          on_each(cursor.getMetaData(), cursor.getData());
          ++cursor;
        }
      }
    std::string namespace_, host_name_;
  };

}