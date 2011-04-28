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
#include <objcog/db/document.hpp>

namespace bp = boost::python;

namespace objcog
{
  struct DocumentWrap : Document, bp::wrapper<Document>

  {

    virtual void put_impl(const std::string& key, const std::string& type_name, const std::string& buffer)
    {
      if (bp::override put = this->get_override("put"))
        put(key,type_name,buffer);
      else
        throw std::logic_error("put not implemented.");
    }

    virtual void get_impl(const std::string& key, const std::string& type_name, std::string& buffer) const
    {
      if (bp::override get = this->get_override("get"))
      {
        bp::object result = get(key,type_name);
        buffer = bp::extract<std::string>(result);
      }
      else
        throw std::logic_error("get not implemented.");
    }

   static void wrap_document()
    {
      //use private names so that python people know these are internal
      bp::class_<Document, boost::shared_ptr<Document>, boost::noncopyable>("_DocumentCpp", bp::no_init);
      bp::class_<DocumentWrap, boost::shared_ptr<DocumentWrap>, boost::noncopyable> doc("_DocumentBase");

      //these happen to be unnecessary.
      //doc.def("put", bp::pure_virtual(&Document::put_impl));
      //doc.def("get", bp::pure_virtual(&Document::get_impl));

    }
  };

  void use_document(Document& doc)
  {
    doc.put<std::string> ("foo", "String data");
    doc.put<float> ("bar", 3.14f);

    float f;
    doc.get<float> ("bar", f);
    std::cout << "I got a " << f << std::endl;

    std::string s;
    doc.get("foo", s);
    std::cout << "I got a " << s << std::endl;
  }
}

BOOST_PYTHON_MODULE(objcog_db)
{
  //wrap all modules
  objcog::DocumentWrap::wrap_document();

  bp::def("use_document", objcog::use_document);
}

