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

#include <boost/serialization/vector.hpp>


#include <curl/curl.h>

#include <boost/foreach.hpp>
#include <boost/thread/mutex.hpp>

#include <cstdio>

#define DEFAULT_COUCHDB_URL "http://localhost:5984"

namespace
{

  struct cURL : boost::noncopyable
  {
    /*
     * This example shows a HTTP PUT operation. PUTs a file given as a command
     * line argument to the URL also given on the command line.
     *
     * This example also uses its own read callback.
     *
     * Here's an article on how to setup a PUT handler for Apache:
     * http://www.apacheweek.com/features/put
     */

    static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream)
    {
      std::cerr << "here" << __PRETTY_FUNCTION__ << std::endl;
      if(!stream)
      {
        std::cout << "no input data" << std::endl;
        return 0;
      }
      size_t retcode;

      /* in real-world cases, this would probably get this data differently
         as this fread() stuff is exactly what the library already would do
         by default internally */
      retcode = fread(ptr, size, nmemb, (FILE*)stream);

      fprintf(stderr, "*** We read %d bytes from file\n", (int)retcode);

      return retcode;
    }

    /* curl calls this routine to get more data */
    static size_t write_callback(char *buffer,
                                 size_t size,
                                 size_t nitems,
                                 void *userp)
    {
      std::cout << buffer;
      std::cout.flush();
      return size;
    }

    cURL() :
      curl_(curl_easy_init())
    {
      if(curl_ == NULL)
        throw std::runtime_error("Unable to connect CURL.");

#if 0
      curl_easy_setopt(curl_, CURLOPT_VERBOSE,1L);
#endif
      curl_easy_setopt(curl_, CURLOPT_READFUNCTION, cURL::read_callback);
      curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, cURL::write_callback);
//      curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &out_);
//      curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &out_);



    }
    ~cURL()
    {
      curl_easy_cleanup(curl_);
    }
    void perform( )
    {
      curl_easy_perform(curl_);
    }
    void setURL(const std::string& url)
    {
      curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
    }

    void PUT()
    {
      /* HTTP PUT please */
      curl_easy_setopt(curl_, CURLOPT_PUT, 1L);
    }

    void GET()
    {
      curl_easy_setopt(curl_,CURLOPT_HTTPGET, 1L);
    }

    void DELETE()
    {
      /* HTTP PUT please */
      curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "DELETE");
    }
    void reset()
    {
      curl_easy_reset(curl_);
    }
    CURL * curl_;
    std::stringstream out_,in_;
  };

  struct cURL_GS
  {
    cURL_GS()
    {
      std::cout << "curl init" << std::endl;
      curl_global_init(CURL_GLOBAL_ALL);
    }
    ~cURL_GS()
    {
      std::cout << "curl cleanup" << std::endl;
      curl_global_cleanup();
    }
  };

  cURL_GS curl_init_cleanup;
}
namespace image
{
  struct Image
  {
    std::vector<int> image_data;
    size_t width, height;
    Image(size_t width = 0, size_t height = 0) :
      image_data(width * height), width(width), height(height)
    {
    }
    inline int & at(int u, int v)
    {
      return image_data[v * width + u];
    }
    inline const int & at(int u, int v) const
    {
      return image_data[v * width + u];
    }
    template<class Archive>
      void serialize(Archive & ar, const unsigned int version)
      {
        ar & width;
        ar & height;
        ar & image_data;
      }
  };
  struct PointType
  {
    float x, y, z;
    template<class Archive>
      void serialize(Archive & ar, const unsigned int version)
      {
        ar & x;
        ar & y;
        ar & z;
      }
  };
  struct PointCloud
  {
    std::vector<PointType> point_data;
    explicit PointCloud(size_t size = 0) :
      point_data(size)
    {
    }

    template<class Archive>
      void serialize(Archive & ar, const unsigned int version)
      {
        ar & point_data;
      }
  };
}

void printImages(const std::string& meta, const image::Image& image)
{
  std::cout << "meta: " << meta << std::endl;
  std::cout << "got an image: width = " << image.width << " height = " << image.height << std::endl;
}
void printClouds(const std::string& meta, const image::PointCloud& cloud)
{
  std::cout << "meta: " << meta << std::endl;
  std::cout << "cloud data: size = " << cloud.point_data.size() << std::endl;
}


void run()
{

  cURL c;
  c.setURL(std::string(DEFAULT_COUCHDB_URL) + "/fellows");
  c.PUT();
  c.perform();

  c.setURL(std::string(DEFAULT_COUCHDB_URL) + "/bed_fellows");
  c.perform();
  c.setURL(std::string(DEFAULT_COUCHDB_URL) + "/bed_fellows");
  c.perform();

  c.reset();
  c.setURL(std::string(DEFAULT_COUCHDB_URL) + "/_all_dbs");
  c.GET();
  c.perform();

  c.reset();
  c.setURL(std::string(DEFAULT_COUCHDB_URL) + "/fellows");
  c.DELETE();
  c.perform();
  c.setURL(std::string(DEFAULT_COUCHDB_URL) + "/bed_fellows");
  c.perform();
}

int main()
{
  try
  {
    run();
  }
  catch (std::exception &e)
  {
    std::cout << "caught " << e.what() << std::endl;
  }
  return 0;
}
