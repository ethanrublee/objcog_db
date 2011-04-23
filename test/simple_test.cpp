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

  boost::shared_ptr<objcog::DbClient> client = objcog::createClient("objcog", "localhost", objcog::MongoDB);
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
