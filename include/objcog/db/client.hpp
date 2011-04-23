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
  struct DbClient
  {
    typedef  boost::archive::binary_iarchive ia;
    typedef  boost::archive::binary_oarchive oa;
    typedef boost::function<void(std::string, const char* data, size_t length)> unblob_sig;
  public:

    DbClient(const std::string& name_space, const std::string& host_name) :
      namespace_(name_space), host_name_(host_name)
    {
    }
    virtual ~DbClient(){}

    void connect()
    {
      connect_impl();
    }

    void drop()
    {
      drop_impl();
    }

    template<typename DataT>
      void retrieve(const std::string& key, const std::string& query,
                    typename boost::function<void(const std::string& meta, const DataT&)> on_each)
      {
        retrieve<DataT,ia> (key, query, on_each);
      }

    template<typename DataT>
      void store(const std::string& collection_key, const std::string& meta_info, const DataT& data)
      {
        store<DataT, oa> (collection_key, meta_info, data);
      }

    std::string getNamespace()
    {
      return namespace_;
    }
    std::string getHostname()
    {
      return host_name_;
    }

  protected:

    virtual void connect_impl() = 0;
    virtual void store_impl(const std::string& key, const std::string& type_name, const std::string& meta, const std::string & data) = 0;
    virtual void retrieve_impl(const std::string& key, const std::string& type_name, std::string query, unblob_sig on_each) = 0;
    virtual void drop_impl() = 0;

  private:
    template<typename DataT, typename archive>
      void retrieve(const std::string& collection_key, const std::string& query,
                    typename boost::function<void(const std::string& meta, const DataT&)> on_each)
      {
        retrieve_impl(collection_key,typeid(DataT).name(), query, unblobber<DataT, archive> (on_each));
      }
    template<typename DataT, typename archive>
      void store(const std::string& collection_key, const std::string& meta_info, const DataT& data)
      {
        //get a binary blob for the data
        std::stringbuf sb;
        archive oa(sb);
        oa << data;
        store_impl(collection_key,typeid(DataT).name(), meta_info, sb.str());
      }

    template<typename DataT, typename archive>
      struct unblobber
      {
        typedef boost::function<void(const std::string& meta, const DataT&)> data_callback;
        unblobber(data_callback c) :
          on_each(c)
        {
        }
        void operator()(const std::string& meta, const char* data, size_t length)
        {
          //fill the stringbuf with data.
          std::stringbuf sb;
          sb.pubsetbuf(const_cast<char*> (data), length);
          sb.pubseekpos(0);

          DataT obj;
          // create and open an archive for input
          archive ia(sb);
          // read class state from archive
          ia >> obj;
          on_each(meta, obj);
        }
        data_callback on_each;
      };

    std::string namespace_, host_name_;
  };

  enum DbType
  {
    MongoDB = 1, FilesystemDB = 2
  };
  boost::shared_ptr<DbClient> createClient(std::string name_space, const std::string& host_name, DbType dbtype);
}
