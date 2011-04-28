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

#include <curl/curl.h>

#include <boost/foreach.hpp>
#include <boost/thread/mutex.hpp>

#include <cstdio>

#define DEFAULT_COUCHDB_URL "http://localhost:5984"

namespace objcog
{

//  struct cURL : boost::noncopyable
//  {
//    /*
//     * This example shows a HTTP PUT operation. PUTs a file given as a command
//     * line argument to the URL also given on the command line.
//     *
//     * This example also uses its own read callback.
//     *
//     * Here's an article on how to setup a PUT handler for Apache:
//     * http://www.apacheweek.com/features/put
//     */
//
//    static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream)
//    {
//      size_t retcode;
//
//      /* in real-world cases, this would probably get this data differently
//         as this fread() stuff is exactly what the library already would do
//         by default internally */
//      retcode = fread(ptr, size, nmemb, (FILE*)stream);
//
//      fprintf(stderr, "*** We read %d bytes from file\n", (int)retcode);
//
//      return retcode;
//    }
//
//    /* curl calls this routine to get more data */
//    static size_t write_callback(char *buffer,
//                                 size_t size,
//                                 size_t nitems,
//                                 void *userp)
//    {
//      std::cout << buffer << std::endl;
//      return size;
//    }
//
//    cURL() :
//      curl_(curl_easy_init())
//    {
//      if(curl_ == NULL)
//        throw std::runtime_error("Unable to connect CURL.");
//
//      curl_easy_setopt(curl_, CURLOPT_READFUNCTION, cURL::read_callback);
//      curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, cURL::write_callback);
//
//
//    }
//    ~cURL()
//    {
//      curl_easy_cleanup(curl_);
//    }
//    void perform( )
//    {
//      curl_easy_perform(curl_);
//    }
//    void setURL(const std::string& url)
//    {
//      curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
//    }
//
//    void put()
//    {
//      /* HTTP PUT please */
//      curl_easy_setopt(curl_, CURLOPT_PUT, 1L);
//    }
//
//
//    CURL * curl_;
//  };
//
//  struct cURL_GS
//  {
//    cURL_GS()
//    {
//      std::cout << "curl init" << std::endl;
//      curl_global_init(CURL_GLOBAL_ALL);
//    }
//    ~cURL_GS()
//    {
//      std::cout << "curl cleanup" << std::endl;
//      curl_global_cleanup();
//    }
//  };

  struct CouchCursor : Cursor_impl
  {

    CouchCursor()
    {

    }

    ~CouchCursor()
    {
    }

    std::pair<const char*, size_t> getData()
    {
      std::pair<const char*, size_t> data;
      return data;
    }
    std::string getMetaData()
    {
      return "";
    }
    Cursor_impl& operator++()
    {
      return *this;
    }
    virtual bool end()
    {
      return true;
    }

  };

  struct CouchDBClient : public DbClient
  {
    CouchDBClient(const std::string& name_space, const std::string& host_name) :
      DbClient(name_space, host_name)
     //,
      //curl_()
    {
    }
    ~CouchDBClient()
    {
    }

  protected:
    void connect_impl()
    {
     // curl_.reset(new cURL());
    }

    void store_impl(const std::string& key, const std::string& type_name, const std::string& meta, const char* data,
                    size_t length)
    {
    }

    boost::shared_ptr<Cursor_impl> query_impl(const std::string& collection_key, const std::string& type_name,
                                              std::string query)
    {
      boost::shared_ptr<Cursor_impl> c;
      return c;
    }

    void drop_impl()
    {
    }

    std::string resolveFullCollectionName(const std::string& key, const std::string& type_name) const
    {
      //only letters, numbers and underscores
      static const boost::regex e("^[a-zA-Z0-9_]*$");
      if (!boost::regex_match(key, e))
        throw std::runtime_error("bad key : " + key);
      return str(boost::format("%s.%s%s") % getNamespace() % key % type_name);
    }

   // boost::scoped_ptr<cURL> curl_;
  };



  namespace
  {
 //   cURL_GS curl_init_cleanup;

  }

  boost::shared_ptr<DbClient> couchdb_client(const std::string& name_space, const std::string& host_name)
  {
    return boost::shared_ptr<DbClient>(new CouchDBClient(name_space, host_name));
  }
#if __NAME_IS_MAIN__

#endif
}
