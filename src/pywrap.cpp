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
#include <boost/python.hpp>
#include <boost/python/raw_function.hpp>
#include <objcog/db/Session.hpp>

namespace bp = boost::python;

namespace objcog
{
  struct LoadWrap : Load, bp::wrapper<Load>

  {

    virtual void add_impl(const std::string& key, const std::string& buffer)
    {
      if (bp::override add = this->get_override("add"))
        add(key, buffer);
      else
        throw std::logic_error("add");
    }

    virtual void get_impl(const std::string& key, std::string& buffer) const
    {
      if (bp::override get = this->get_override("get"))
      {
        bp::object result = get(key);
        buffer = bp::extract<std::string>(result);
      }
      else
        throw std::logic_error("get is not overridden it seems");
    }

  };

  struct TrainingSessionWrap : TrainingSession, bp::wrapper<TrainingSession>
  {

    virtual Load::Ptr makeLoad()
    {
      if (bp::override app = this->get_override("makeLoad"))
      {
        bp::object result = app();
        return bp::extract<Load::Ptr>(result);
      }
      else
        throw std::logic_error("append is not overridden it seems");
    }
  };

  void wrapLoad()
  {
    //use private names so that python people know these are internal
    bp::class_<Load, boost::shared_ptr<Load>, boost::noncopyable>("_LoadCpp", bp::no_init);
    bp::class_<LoadWrap, boost::shared_ptr<LoadWrap>, boost::noncopyable> l("_LoadBase");
    l.def("add", bp::pure_virtual(&Load::add_impl));
    l.def("get", bp::pure_virtual(&Load::get_impl));

    //use private names so that python people know these are internal
    bp::class_<TrainingSession, boost::shared_ptr<TrainingSession>, boost::noncopyable>("_TrainingSessionCpp",
                                                                                        bp::no_init);
    bp::class_<TrainingSessionWrap, boost::shared_ptr<TrainingSessionWrap>, boost::noncopyable>
                                                                                                ts(
                                                                                                   "_TrainingSessionBase");
    ts.def("makeLoad", bp::pure_virtual(&TrainingSession::makeLoad));
    ts.def("append", &TrainingSession::append);
  }

  void useLoad(Load& l)
  {
    l.add<std::string> ("foo", "String data");
    l.add<float> ("bar", 3.14f);

    float f;
    l.get<float> ("bar", f);
    std::cout << "I got a " << f << std::endl;

    std::string s;
    l.get("foo", s);
    std::cout << "I got a " << s << std::endl;
  }

  void useSession(TrainingSession& session)
  {
    for(int i = 0; i < 5; i++)
    {
      Load::Ptr load = session.makeLoad();
      load->add("foobar",69);
      load->add<std::string>("foo","a string val");
    }

  }
}

BOOST_PYTHON_MODULE(objcog_db)
{
  //wrap all modules
  objcog::wrapLoad();

  bp::def("use_load", objcog::useLoad);
  bp::def("use_session", objcog::useSession);

}

