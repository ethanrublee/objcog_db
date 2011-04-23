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

  image::Image im(20, 50);
  image::PointCloud pc(33);

  boost::shared_ptr<objcog::DbClient> client = objcog::DbClient::createClient("objcog", "localhost", objcog::DbClient::MongoDB);
  client->connect();
  client->store<image::PointCloud> ("images", "{ author : 'Vincent', tags: ['coolness', 'nuggets','apples'] }", pc);
  client->store<image::Image> ("images", "{ author : 'Gary', tags: ['coolness', 'nuggets','apples'] }", im);
  client->store<image::Image> ("images", "{ author : 'Ethan', tags: ['toons', 'nuggets','apples'] }", im);

  client->retrieve<image::Image> ("images", "{ 'tags' : { $all : ['nuggets'] } }", printImages);
  client->retrieve<image::Image> ("images", "{ 'author' : { $in : ['Ethan','Vincent'] } }", printImages);
  client->retrieve<image::PointCloud> ("images", "{ 'author' : { $in : ['Ethan','Vincent'] } }", printClouds);

  //delete everything at end..
  client->drop();
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
