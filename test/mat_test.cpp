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
  boost::shared_ptr<objcog::DbClient> client = objcog::DbClient::createClient("objcog", "localhost", objcog::DbClient::MongoDB);
  client->connect();
  client->drop();
  client->store<cv::Mat> ("images", "{ author : 'Ethan', tags: ['cv::mat', 'a.png'], info: 'is a png' }", image);
  client->store<cv::Mat> ("images", "{ author : 'Ethan', tags: ['cv::mat', 'a.png'], info: '/2' }", image / 2);
  client->retrieve<cv::Mat> ("images", "{ 'info' : '/2' }", showImage);
  client->drop();
}
