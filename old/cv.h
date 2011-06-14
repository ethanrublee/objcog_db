/*
 * cv.h
 *
 *  Created on: Jun 7, 2011
 *      Author: vrabaud
 */

#ifndef CV_H_
#define CV_H_

template<>
  void Db::get<cv::Mat>(const ObjectId & object_id, const std::string key, cv::Mat &object) const
  {
    // TODO couch db calls

  }

#endif /* CV_H_ */
