// This may look like C code, but it's really -*- C++ -*-
/*
 * Copyright (C) 2008 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#ifndef WT_DBO_SESSION_IMPL_H_
#define WT_DBO_SESSION_IMPL_H_

#include <iostream>

#include <boost/type_traits.hpp>

#include <Wt/Dbo/SqlConnection.h>
#include <Wt/Dbo/Query.h>

#include <Wt/Dbo/has_member.h>

namespace Wt {
  namespace Dbo {
    namespace Impl {
      template <class C, typename T>
      struct LoadHelper
      {
	static MetaDbo<C> *load(Session *session, SqlStatement *statement,
				 int& column)
	{
	  return session->loadWithNaturalId<C>(statement, column);
	};
      };

      template <class C>
      struct LoadHelper<C, long long>
      {
	static MetaDbo<C> *load(Session *session, SqlStatement *statement,
				 int& column)
	{
	  return session->loadWithLongLongId<C>(statement, column);
	}
      };

    GENERATE_HAS_MEMBER(onPreInsert);
    GENERATE_HAS_MEMBER(onPostInsert);
    GENERATE_HAS_MEMBER(onPreUpdate);
    GENERATE_HAS_MEMBER(onPostUpdate);
    GENERATE_HAS_MEMBER(onPreDelete);
    GENERATE_HAS_MEMBER(onPostDelete);
    GENERATE_HAS_MEMBER(onInsertCommitted);
    GENERATE_HAS_MEMBER(onUpdateCommitted);
    GENERATE_HAS_MEMBER(onDeleteCommitted);

    // On Pre Insert:
    template<typename C>
    typename boost::enable_if<has_member_onPreInsert<C>, void>::type
    callOnPreInsert(C *dboRow, const ptr<C> &dboPtr) {
      dboRow->onPreInsert(dboPtr);
    }

    template<typename C>
    typename boost::disable_if<has_member_onPreInsert<C>, void>::type
    callOnPreInsert(C *dboRow, const ptr<C> &dboPtr) {
      //Do nothing
    }

    // On Pre Update:
    template<typename C>
    typename boost::enable_if<has_member_onPreUpdate<C>, void>::type
    callOnPreUpdate(C *dboRow, const ptr<C> &dboPtr) {
        dboRow->onPreUpdate(dboPtr);
    }

    template<typename C>
    typename boost::disable_if<has_member_onPreUpdate<C>, void>::type
    callOnPreUpdate(C *dboRow, const ptr<C> &dboPtr) {
        //Do nothing
    }

    // On Pre Delete:
    template<typename C>
    typename boost::enable_if<has_member_onPreDelete<C>, void>::type
    callOnPreDelete(C *dboRow, const ptr<C> &dboPtr) {
        dboRow->onPreDelete(dboPtr);
    }

    template<typename C>
    typename boost::disable_if<has_member_onPreDelete<C>, void>::type
    callOnPreDelete(C *dboRow, const ptr<C> &dboPtr) {
        //Do nothing
    }

    //---------------------------------------
    // On Post Insert:
    template<typename C>
    typename boost::enable_if<has_member_onPostInsert<C>, void>::type
    callOnPostInsert(C *dboRow, Session &session, const ptr<C> &dboPtr) {
      dboRow->onPostInsert(session, dboPtr);
    }

    template<typename C>
    typename boost::disable_if<has_member_onPostInsert<C>, void>::type
    callOnPostInsert(C *dboRow, Session &session, const ptr<C> &dboPtr) {
      //Do nothing
    }

    // On Post Update:
    template<typename C>
    typename boost::enable_if<has_member_onPostUpdate<C>, void>::type
    callOnPostUpdate(C *dboRow, Session &session, const ptr<C> &dboPtr) {
      dboRow->onPostUpdate(session, dboPtr);
    }

    template<typename C>
    typename boost::disable_if<has_member_onPostUpdate<C>, void>::type
    callOnPostUpdate(C *dboRow, Session &session, const ptr<C> &dboPtr) {
      //Do nothing
    }

    // On Post Delete:
    template<typename C>
    typename boost::enable_if<has_member_onPostDelete<C>, void>::type
    callOnPostDelete(C *dboRow, Session &session, const ptr<C> &dboPtr) {
      dboRow->onPostDelete(session, dboPtr);
    }

    template<typename C>
    typename boost::disable_if<has_member_onPostDelete<C>, void>::type
    callOnPostDelete(C *dboRow, Session &session, const ptr<C> &dboPtr) {
      //Do nothing
    }

    //---------------------------------------
    // On Insert Committed:
    template<typename C>
    typename boost::enable_if<has_member_onInsertCommitted<C>, void>::type
    callOnInsertCommitted(C *dboRow, const ptr<C> &dboPtr) {
      dboRow->onInsertCommitted(dboPtr);
    }

    template<typename C>
    typename boost::disable_if<has_member_onInsertCommitted<C>, void>::type
    callOnInsertCommitted(C *dboRow, const ptr<C> &dboPtr) {
      //Do nothing
    }

    // On Update Committed:
    template<typename C>
    typename boost::enable_if<has_member_onUpdateCommitted<C>, void>::type
    callOnUpdateCommitted(C *dboRow, const ptr<C> &dboPtr) {
      dboRow->onUpdateCommitted(dboPtr);
    }

    template<typename C>
    typename boost::disable_if<has_member_onUpdateCommitted<C>, void>::type
    callOnUpdateCommitted(C *dboRow, const ptr<C> &dboPtr) {
      //Do nothing
    }

    // On Delete Committed:
    template<typename C>
    typename boost::enable_if<has_member_onDeleteCommitted<C>, void>::type
    callOnDeleteCommitted(C *dboRow, const ptr<C> &dboPtr) {
      dboRow->onDeleteCommitted(dboPtr);
    }

    template<typename C>
    typename boost::disable_if<has_member_onDeleteCommitted<C>, void>::type
    callOnDeleteCommitted(C *dboRow, const ptr<C> &dboPtr) {
      //Do nothing
    }

    }

#undef GENERATE_HAS_MEMBER

template <class C>
void Session::mapClass(const char *tableName)
{
  if (schemaInitialized_)
    throw Exception("Cannot map tables after schema was initialized.");

  if (classRegistry_.find(&typeid(C)) != classRegistry_.end())
    return;

  Mapping<C> *mapping = new Mapping<C>();
  mapping->tableName = tableName;

  classRegistry_[&typeid(C)] = mapping;
  tableRegistry_[tableName] = mapping;
}

template <class C>
SqlStatement *Session::getStatement(int statementIdx)
{
  initSchema();

  ClassRegistry::iterator i = classRegistry_.find(&typeid(C));
  Impl::MappingInfo *mapping = i->second;

  std::string id = statementId(mapping->tableName, statementIdx);

  SqlStatement *result = getStatement(id);

  if (!result)
    result = prepareStatement(id, mapping->statements[statementIdx]);

  return result;
}

template <class C>
const char *Session::tableName() const
{
  typedef typename std::remove_const<C>::type MutC;

  ClassRegistry::const_iterator i = classRegistry_.find(&typeid(MutC));
  if (i != classRegistry_.end())
    return dynamic_cast< Mapping<MutC> *>(i->second)->tableName;
  else
    throw Exception(std::string("Class ") + typeid(MutC).name()
		    + " was not mapped.");
}

template <class C>
const std::string Session::tableNameQuoted() const
{
  return std::string("\"") + Impl::quoteSchemaDot(tableName<C>()) + '"';
}

template <class C>
Session::Mapping<C> *Session::getMapping() const
{
  if (!schemaInitialized_)
    initSchema();

  ClassRegistry::const_iterator i = classRegistry_.find(&typeid(C));
  if (i != classRegistry_.end()) {
    Session::Mapping<C> *mapping = dynamic_cast< Mapping<C> *>(i->second);
    return mapping;
  } else
    throw Exception(std::string("Class ") + typeid(C).name()
		    + " was not mapped.");
}

template <class C>
ptr<C> Session::load(SqlStatement *statement, int& column)
{
  typedef typename std::remove_const<C>::type MutC;

  Impl::MappingInfo *mapping = getMapping<MutC>();
  MetaDboBase *dbob = mapping->load(*this, statement, column);

  if (dbob) {
    MetaDbo<MutC> *dbo = dynamic_cast<MetaDbo<MutC> *>(dbob);
    return ptr<C>(dbo);
  } else
    return ptr<C>();
}

template <class C>
MetaDbo<C> *Session::loadWithNaturalId(SqlStatement *statement, int& column)
{
  typedef typename std::remove_const<C>::type MutC;

  Mapping<MutC> *mapping = getMapping<MutC>();

  /* Natural id is possibly multiple fields anywhere */
  MetaDboBase *dbob = createDbo(mapping);
  MetaDbo<MutC> *dbo = dynamic_cast<MetaDbo<MutC> *>(dbob);
  implLoad<MutC>(*dbo, statement, column);

  if (dbo->id() == dbo_traits<MutC>::invalidId()) {
    dbo->setSession(nullptr);
    delete dbob;
    return nullptr;
  }

  typename Mapping<MutC>::Registry::iterator
    i = mapping->registry_.find(dbo->id());

  if (i == mapping->registry_.end()) {
    mapping->registry_[dbo->id()] = dbo;
    return dbo;
  } else {
    dbo->setSession(nullptr);
    delete dbob;
    return i->second;
  }
}

template <class C>
MetaDbo<C> *Session::loadWithLongLongId(SqlStatement *statement, int& column)
{
  typedef typename std::remove_const<C>::type MutC;

  Mapping<MutC> *mapping = getMapping<MutC>();

  if (mapping->surrogateIdFieldName) {
    /*
     * If mapping uses surrogate keys, then we can first read the id and
     * decide if we already have it.
     *
     * If not, then we need to first read the object, get the id, and if
     * we already had it, delete the redundant copy.
     */
    long long id = dbo_traits<C>::invalidId();

    /*
     * Auto-generated surrogate key is first field
     *
     * NOTE: getResult will return false if the id field is NULL. This
     * could occur with a LEFT JOIN involving the table. See:
     * dbo_test4c.
     */
    if (!statement->getResult(column++, &id)) {
      column += (int)mapping->fields.size()
	+ (mapping->versionFieldName ? 1 : 0);
      return nullptr;
    }

    typename Mapping<MutC>::Registry::iterator i = mapping->registry_.find(id);

    if (i == mapping->registry_.end()) {
      MetaDboBase *dbob = createDbo(mapping);
      MetaDbo<MutC> *dbo = dynamic_cast<MetaDbo<MutC> *>(dbob);
      dbo->setId(id);
      implLoad<MutC>(*dbo, statement, column);

      mapping->registry_[id] = dbo;

      return dbo;
    } else {
      if (!i->second->isLoaded())
	implLoad<MutC>(*i->second, statement, column);
      else
	column += (int)mapping->fields.size() + (mapping->versionFieldName ? 1 : 0);

      return i->second;
    }
  } else
    return loadWithNaturalId<C>(statement, column);
}

template <class C>
ptr<C> Session::add(ptr<C>& obj)
{
  typedef typename std::remove_const<C>::type MutC;

  initSchema();

  MetaDbo<MutC> *dbo = obj.obj();
  if (dbo && !dbo->session()) {
    dbo->setSession(this);
    if (flushMode() == FlushMode::Auto)
      needsFlush(dbo);
    else
      objectsToAdd_.push_back(dbo);

    SessionAddAction act(*dbo, *getMapping<MutC>());
    act.visit(*dbo->obj());
  }

  return obj;
}

template <class C>
ptr<C> Session::add(std::unique_ptr<C> obj)
{
  ptr<C> result(std::move(obj));
  return add(result);
}

template <class C>
ptr<C> Session::load(const typename dbo_traits<C>::IdType& id,
		     bool forceReread)
{
  ptr<C> result = loadLazy<C>(id);
  if (forceReread)
    result.reread();
  *result; // Dereference to do actual load or throw exception
  return result;
}

template <class C>
ptr<C> Session::loadLazy(const typename dbo_traits<C>::IdType& id)
{
  initSchema();

  Mapping<C> *mapping = getMapping<C>();
  typename Mapping<C>::Registry::iterator i = mapping->registry_.find(id);

  if (i == mapping->registry_.end()) {
    MetaDboBase *dbob = createDbo(mapping);
    MetaDbo<C> *dbo = dynamic_cast<MetaDbo<C> *>(dbob);
    dbo->setId(id);
    mapping->registry_[id] = dbo;
    return ptr<C>(dbo);
  } else
    return ptr<C>(i->second);
}

template <class C, typename BindStrategy>
Query< ptr<C>, BindStrategy > Session::find(const std::string& where)
{
  initSchema();

  return Query< ptr<C>, BindStrategy >
    (*this, '"' + Impl::quoteSchemaDot(tableName<C>()) + '"', where);
}

template <class Result>
Query<Result> Session::query(const std::string& sql)
{
  return query<Result, DynamicBinding>(sql);
}

template <class Result, typename BindStrategy>
Query<Result, BindStrategy> Session::query(const std::string& sql)
{
  initSchema();

  return Query<Result, BindStrategy>(*this, sql);
}

template <class C>
void Session::prune(MetaDbo<C> *obj)
{
  getMapping<C>()->registry_.erase(obj->id());

  discardChanges(obj);
}

template<class C>
void Session::implSave(MetaDbo<C>& dbo)
{
  if (!transaction_)
    throw Exception("Dbo save(): no active transaction");

  if (!dbo.savedInTransaction())
    transaction_->objects_.push_back(new ptr<C>(&dbo));

  Session::Mapping<C> *mapping = getMapping<C>();

  bool isInsert = dbo.isNew() && !dbo.savedInTransaction();
  const ptr<C> dbRow(&dbo);
  if (isInsert) {
    Impl::callOnPreInsert(dbo.obj(), dbRow);
  } else {
    Impl::callOnPreUpdate(dbo.obj(), dbRow);
  }

  SaveDbAction<C> action(dbo, *mapping);
  action.visit(*dbo.obj());

  if (isInsert) {
    dbo.setInsertedInTransaction();
    Impl::callOnPostInsert(dbo.obj(), *this, dbRow);
  } else {
    Impl::callOnPostUpdate(dbo.obj(), *this, dbRow);
  }

  mapping->registry_[dbo.id()] = &dbo;
}

template<class C>
void Session::implDelete(MetaDbo<C>& dbo)
{
  if (!transaction_)
    throw Exception("Dbo save(): no active transaction");

  // when saved in transaction, we are already in this list
  if (!dbo.savedInTransaction())
    transaction_->objects_.push_back(new ptr<C>(&dbo));

  const ptr<C> dbRow(&dbo);
  Impl::callOnPreDelete(dbo.obj(), dbRow);

  bool versioned = getMapping<C>()->versionFieldName && dbo.obj() != nullptr;
  SqlStatement *statement
    = getStatement<C>(versioned ? SqlDeleteVersioned : SqlDelete);

  // when saved in the transaction, we will be at version() + 1

  statement->reset();
  ScopedStatementUse use(statement);

  int column = 0;
  dbo.bindModifyId(statement, column);

  int version = -1;
  if (versioned) {
    version = dbo.version() + (dbo.savedInTransaction() ? 1 : 0);
    statement->bind(column++, version);
  }

  statement->execute();

  if (versioned) {
    int modifiedCount = statement->affectedRowCount();
    if (modifiedCount != 1)
      throw StaleObjectException(std::string()/*std::to_string(dbo.id())*/,
                                 this->tableName<C>(), version);
  }

  Impl::callOnPostDelete(dbo.obj(), *this, dbRow);
}

template<class C>
void Session::implTransactionDone(MetaDbo<C>& dbo, bool success)
{
  TransactionDoneAction action(dbo, *this, *getMapping<C>(), success);
  action.visit(*dbo.obj());
  if(!success) return;

  ptr<C> dbRow(&dbo);
  if(dbo.deletedInTransaction()) {
    // It does before, in MetaDbo<C>::doTransactionDone

  } else if(dbo.insertedInTransaction()) {
    Impl::callOnInsertCommitted(dbo.obj(), dbRow);

  } else if(dbo.savedInTransaction()) {
    Impl::callOnUpdateCommitted(dbo.obj(), dbRow);

  } else {
    throw Exception("Session::implTransactionDone but it seems that there were not any actions with this object.");
  }
}

template <class C>
void Session::implLoad(MetaDbo<C>& dbo, SqlStatement *statement, int& column)
{
  if (!transaction_)
    throw Exception("Dbo load(): no active transaction");

  LoadDbAction<C> action(dbo, *getMapping<C>(), statement, column);

  C *obj = new C();
  try {
    action.visit(*obj);
    dbo.setObj(obj);
  } catch (...) {
    delete obj;
    throw;
  }
}

template<class C>
void Session::callOnDeleteCommitted(MetaDbo<C>& dbo)
{
  ptr<C> dbRow(&dbo);
  Impl::callOnDeleteCommitted(dbo.obj(), dbRow);
}

template <class C>
Session::Mapping<C>::~Mapping()
{
  for (typename Registry::iterator i = registry_.begin();
       i != registry_.end(); ++i) {
    i->second->setState(MetaDboBase::Orphaned);
  }
}

template <class C>
void Session::Mapping<C>
::dropTable(Session& session, std::set<std::string>& tablesDropped)
{
  if (tablesDropped.count(tableName) == 0) {
    DropSchema action(session, *this, tablesDropped);
    C dummy;
    action.visit(dummy);
  }
}

template <class C>
void Session::Mapping<C>::rereadAll()
{
  std::vector<ptr<C> > objects;
  for (typename Registry::iterator i = registry_.begin();
       i != registry_.end(); ++i) {
    // we cannot call reread() here because that would change the
    // registry and invalidate the iterators
    objects.push_back(ptr<C>(i->second));
  }
  for (typename std::vector<ptr<C> >::iterator i = objects.begin();
       i != objects.end(); ++i) {
    (*i).reread();
  }
}

template <class C>
MetaDbo<C> *Session::Mapping<C>::create(Session& session)
{
  return new MetaDbo<C>(session);
}

template <class C>
void Session::Mapping<C>::load(Session& session, MetaDboBase *obj)
{
  MetaDbo<C> *dbo = dynamic_cast<MetaDbo<C> *>(obj);
  int column = 0;
  session.template implLoad<C>(*dbo, nullptr, column);
}

template <class C>
MetaDbo<C> *Session::Mapping<C>::load(Session& session, SqlStatement *statement,
				      int& column)
{
  typedef typename std::remove_const<C>::type MutC;

  return Impl::LoadHelper<C, typename dbo_traits<MutC>::IdType>
    ::load(&session, statement, column);
}

template <class C>
void Session::Mapping<C>::init(Session& session)
{
  typedef typename std::remove_const<C>::type MutC;

  if (!initialized_) {
    initialized_ = true;

    InitSchema action(session, *this);
    MutC dummy;
    action.visit(dummy);
  }
}

  }
}

#endif // WT_DBO_SESSION_IMPL_H_
