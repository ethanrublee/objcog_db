#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <objcog/db/document.hpp>
#include <objcog/db/cv.hpp>


namespace objcog
{
  void insert_image(const std::string image_fname, const std::string& key, Document::Ptr doc)
  {
    cv::Mat image = cv::imread(image_fname);
    doc->put<cv::Mat>(key,image);
  }
}

