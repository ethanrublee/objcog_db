#include <objcog/db/cv.hpp>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

typedef boost::archive::binary_oarchive boa;
typedef boost::archive::binary_iarchive bia;
typedef boost::archive::text_oarchive toa;
typedef boost::archive::text_iarchive tia;

//template void boost::serialization::save<boa>(boa&, const cv::Mat&, const unsigned int version);
//template void boost::serialization::load<bia>(bia&, cv::Mat&, const unsigned int version);
//template void boost::serialization::save<toa>(toa&, const cv::Mat&, const unsigned int version);
//template void boost::serialization::load<tia>(tia&, cv::Mat&, const unsigned int version);
