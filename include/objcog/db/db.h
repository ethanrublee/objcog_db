/*
 * query.h
 *
 *  Created on: Jun 3, 2011
 *      Author: vrabaud
 */

#include <algorithm>
#include <iterator>
#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>
#include "opencv2/core/core.hpp"

typedef std::string ObjectId;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Db
{
public:
  template<typename T>
    void get(const ObjectId & object_id, const std::string key, T &object) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class DbObject
{
  virtual void read(const Db & db, const ObjectId &object_id) = 0;
  virtual void write(const Db & db, const ObjectId &object_id) const = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
  class QueryIterator : public std::iterator<std::forward_iterator_tag, int>
  {
  public:
    QueryIterator()
    {
    }

    virtual QueryIterator(const Db& db, std::vector<std::string> & object_ids) :
        db_(db), object_ids_(object_ids)
    {
    }

    QueryIterator<T> & operator++()
    {
      // Move forward in the list of Objects to check
      object_ids_.pop_back();
      // Return the end iterator if we are done
      if (object_ids_.empty())
        return QueryIterator();
      // Fill the current object
      object_->read(db_, object_ids_.back());
      return *this;
    }

    bool operator!=(const QueryIterator<T> & query_iterator) const
    {
      if (query_iterator.object_ids_.empty())
        return (!object_ids_.empty());
      if (object_ids_.size() >= query_iterator.object_ids_.size())
        return std::equal(object_ids_.begin(), object_ids_.end(), query_iterator.object_ids_.begin());
      else
        return std::equal(query_iterator.object_ids_.begin(), query_iterator.object_ids_.end(), object_ids_.begin());
    }

    QueryIterator end()
    {
      return QueryIterator();
    }
    friend class Query;
  private:
    Db db_;
    boost::shared_ptr<T> object_;
    std::vector<ObjectId> object_ids_;
  };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
  class Query
  {
    Query();
    void add_where();

    QueryIterator<T> begin(const Db &db)
    {
      // Process the query and get the ids of several objects
      // TODO Call CouchDB and get a list of Object ID's
      std::vector<ObjectId> object_ids;
      return QueryIterator(db, object_ids);
    }

    QueryIterator end()
    {
      return QueryIterator::end();
    }
  };

