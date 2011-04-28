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

#pragma once

#include <iostream>
#include <sstream>
#include <list>

#include <boost/format.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <boost/function.hpp>
#include <boost/regex.hpp>
#define  SHOW() std::cout << __PRETTY_FUNCTION__ << std::endl
namespace objcog
{
  //idea for type safety
  //namespace StandardNames
  //{
  //  static const std::string IMAGE;
  //  static const std::string IMAGE_LEFT;
  //  static const std::string IMAGE_RIGHT;
  //}
  struct Load
  {
    typedef boost::archive::binary_oarchive OutputArchive;
    typedef boost::archive::binary_iarchive InputArchive;
    typedef boost::shared_ptr<Load> Ptr;

    Load()
    {
    }
    virtual ~Load()
    {
    }
    /**
     * \brief Add data to the
     */
    template<typename DataT>
      void add(const std::string& key, const DataT& data)
      {

        //only letters, numbers and underscores
        //verify symbolic name for the key (for json, python)
        static const boost::regex e("^[a-zA-Z0-9_]*$");
        if (!boost::regex_match(key, e))
          throw std::runtime_error("bad key, must be symbolic : " + key);

        //get a binary blob for the data
        std::stringbuf sb;
        OutputArchive oa(sb);
        oa << data;
        //FIXME get rid of extra copy here
        std::string data_str = sb.str();
        add_impl(magic_key<DataT> (key), data_str);
      }

    template<typename DataT>
      void get(const std::string& key, DataT& data_out) const
      {
        //SHOW();
        std::string buffer;
        get_impl(magic_key<DataT> (key), buffer);
        //std::cout << "buffer size " << buffer.size() << std::endl;
        //fill the stringbuf with data.
        std::stringbuf sb;
        sb.pubsetbuf(const_cast<char*> (buffer.c_str()), buffer.size());
        sb.pubseekpos(0);
        // create and open an archive for input
        InputArchive ia(sb);
        // read class state from archive
        ia >> data_out;
      }

    template<typename DataT>
      std::string magic_key(const std::string& key) const
      {
        std::string magic_key = key + "_" + typeid(DataT).name();
        return magic_key;
      }

  protected:

    /**
     * \brief Takes a byte buffer and stores it with a given unique key.
     * @param buffer Byte buffer
     * @param length  Length in bytes of the byte buffer
     * @param key A symbolic name to store the buffer with.
     */
    virtual void add_impl(const std::string& key, const std::string& buffer) = 0;

    virtual void get_impl(const std::string& key, std::string& buffer) const = 0;

    friend void objcog::wrapLoad();
  };

  struct TrainingSession
  {
    typedef std::list<Load::Ptr> LoadList;
    typedef LoadList::const_iterator const_iterator;
    typedef LoadList::iterator iterator;

    virtual Load::Ptr makeLoad() = 0;

    void append(Load::Ptr load)
    {
      loads.push_back(load);
    }

    iterator begin()
    {
      return loads.begin();
    }
    const_iterator begin() const
    {
      return loads.begin();
    }
    const_iterator end() const
    {
      return loads.end();
    }

    LoadList loads;
  };

}
//int main()
//{
//
//  Session s;
//  while(true)
//  {
//    Load::Ptr load = s.makeLoad();
//    load->add(1.0f,"foo");
//    load->add<std::string>("hello", "bar");
//    load->add<bool>(false,"bar");
//  }
//}
