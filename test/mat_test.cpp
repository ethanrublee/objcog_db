#include <objcog/db/client.hpp>
#include <objcog/db/cv.hpp>
#include <opencv2/highgui/highgui.hpp>


void showImage(const std::string& meta, const cv::Mat& image)
{
  cv::imshow("image - deserialed",image);
  cv::waitKey(0);
}
int main()
{
  cv::Mat image = cv::imread("data/a.png");
  cv::imshow("image",image);
  cv::waitKey(0);
  boost::shared_ptr<objcog::DbClient> client = objcog::createClient("objcog", "localhost", objcog::MongoDB);
  client->connect();
  client->drop();
  client->store<cv::Mat> ("images", "{ author : 'Ethan', tags: ['cv::mat', 'a.png'], info: 'is a png' }", image);
  client->store<cv::Mat> ("images", "{ author : 'Ethan', tags: ['cv::mat', 'a.png'], info: '/2' }", image / 2);
  client->retrieve<cv::Mat> ("images", "{ 'info' : '/2' }", showImage);
  client->drop();
}
