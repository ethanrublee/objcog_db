/*
 * observation.h
 *
 *  Created on: Jun 7, 2011
 *      Author: vrabaud
 */

#ifndef OBSERVATION_H_
#define OBSERVATION_H_

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Observation : public DbObject
{
public:
  virtual void read(const Db & db, const ObjectId &object_id)
  {
    db.get<cv::Mat>(object_id, "image/blah", image_);
  }

  virtual void write(const Db & db, const ObjectId &object_id) const
  {
  }

  cv::Mat image_;
  //pcl::point_cloud pcd;
private:
  /** database unique identifier */
  std::string object_id_;
};

#endif /* OBSERVATION_H_ */
