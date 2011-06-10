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

#include <boost/foreach.hpp>
#include <boost/any.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/shared_ptr.hpp>
#include "opencv2/core/core.hpp"

typedef std::string CollectionName;
typedef std::string Field;
typedef std::string FieldName;
typedef std::string ObjectId;
typedef std::string RevisionId;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace db_serialization
{
template<class Archive, typename T>
  void save(Archive & ar, const T & m);

template<class Archive, typename T>
  void load(const Archive & ar, T & m);
} // namespace serialization

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** The main class that interact with the db
 * A collection is similar to the term used in CouchDB. It could be a schema/table
 */
class Db
{
public:
  virtual void persist_fields(const ObjectId & object_id, const CollectionName &collection,
                              std::map<FieldName, Field> fields) const;

  virtual void load_fields(const ObjectId & object_id, const CollectionName &collection,
                           std::map<FieldName, Field> fields) const;

  virtual void query(const std::vector<CollectionName> collections, std::map<FieldName, std::string> regexps
                     , std::vector<ObjectId> & object_ids) const;
};

template<typename DbType, typename Attachment>
  void load_attachment(const DbType&db, const ObjectId & object_id, const CollectionName &collection,
                       const FieldName &field_name, Attachment &attachment) const;

template<typename DbType, typename Attachment>
  void persist_attachment(const DbType&db, const ObjectId & object_id, const CollectionName &collection,
                          const FieldName &field, Attachment &attachment) const;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** A Document holds fields (in the CouchDB sense) which are strings that are queryable, and attachments (that are
 * un-queryable binary blobs)
 */
class Document
{
public:
  Document(const Db & db) :
      db_(db)
  {
  }

  Document(const Db & db, const CollectionName & collection, const ObjectId &object_id) :
      object_id_(object_id), db_(db), collection_(collection)
  {
    // Load all fields from the DB (not the attachments)
    db_.load_fields(object_id_, collection_, fields_);
  }

  virtual ~Document();

  virtual void persist() const
  {
    // Persist the object if it does not exist in the DB
    if (object_id_.empty())
    {
      // TODO
      object_id_ = "";
    }

    // Persist the fields first
    for (std::map<FieldName, Field>::const_iterator field = fields_.begin(), field_end = fields_.end();
        field != field_end; ++field)
        {
      // TODO persist the field for that object to the DB
    }

    // Persist the attachments
    boost::any nothing_any;
    for (std::map<FieldName, boost::any>::const_iterator attachment = attachments_.begin(), attachment_end =
                                                             attachments_.end(); attachment != attachment_end;
        ++attachment)
        {
      if (attachment->second.empty())
        continue;
      // Persist the attachment
      persist_attachment(db, object_id_, collection_, attachment->first, attachment->second);
    }
  }

  /** Extract a specific field from the pre-loaded Document
   * @param field
   * @param t
   */
  template<typename T>
    void get(const FieldName &field, T & t) const
    {
      // check if it is loaded
      std::map<FieldName, boost::any>::const_iterator val = attachments_.find(field);
      if ((val != attachments_.end()) && (!val->second.empty()))
      {
        t = *val;
        return;
      }
      else
      {
        // Otherwise, load it from the DB
        // TODO : maybe we want to load all attachments first
        db_.load_attachment(object_id_, collection_, field, t);
        attachments_[field] = t;
      }
    }

  /** Add a specific field to a Document (that has been pre-loaded or not)
   * @param field
   * @param t
   */
  template<typename T>
    void set(const FieldName &field, const T & t)
    {
      attachments_[field] = t;
    }
private:
  bool is_loaded_;
  mutable ObjectId object_id_;
  mutable CollectionName collection_;
  RevisionId revision_id;
  const Db db_;
  std::map<FieldName, boost::any> attachments_;
  std::map<FieldName, Field> fields_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<Document T>
  class QueryIterator : public std::iterator<std::forward_iterator_tag, int>
  {
  public:
    QueryIterator()
    {
    }

    QueryIterator(const Db& db, std::vector<std::string> & object_ids) :
        db_(db), object_ids_(object_ids)
    {
      // Load the first element in the db
      object_->read();
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

    /** Add requirements for the documents to retrieve
     * @param field a field to match
     * @param regex the regular expression the field verifies, in TODO format
     */
    void add_where(FieldName & field, std::string & regex);

    /** Add collections that should be checked for specific fields
     * @param collection
     */
    void add_collection(CollectionName & collection);

    QueryIterator<T> query(const Db &db)
    {
      // Process the query and get the ids of several objects
      std::vector<ObjectId> object_ids;
      db.query(collections_, regexes_, object_ids);
      return QueryIterator<T>(db, object_ids);
    }
  private:
    std::vector<CollectionName> collections_;
    std::map<FieldName, std::string> regexes_;
  };

