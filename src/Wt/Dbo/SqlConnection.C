/*
 * Copyright (C) 2009 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#include "Wt/Dbo/SqlConnection"
#include "Wt/Dbo/SqlStatement"
#include "Wt/Dbo/Exception"
#include "SqlConnection"
#include <boost/lexical_cast.hpp>

#include <iostream>

#include <cassert>

namespace Wt {
  namespace Dbo {

SqlConnection::SqlConnection()
  : transactionIsolationLevel_(READ_COMMITTED)
{ }

SqlConnection::SqlConnection(const SqlConnection& other)
  : transactionIsolationLevel_(READ_COMMITTED),
    properties_(other.properties_)
{ }

SqlConnection::~SqlConnection()
{
  assert(statementCache_.empty());
}

void SqlConnection::clearStatementCache()
{
  for (StatementMap::iterator i = statementCache_.begin();
       i != statementCache_.end(); ++i)
    delete i->second;

  statementCache_.clear();
}

void SqlConnection::executeSql(const std::string& sql)
{
  SqlStatement *s = prepareStatement(sql);
  s->execute();
  delete s;
}

SqlStatement *SqlConnection::getStatement(const std::string& id) const
{
  StatementMap::const_iterator i = statementCache_.find(id);
  if (i != statementCache_.end()) {
    SqlStatement *result = i->second;
    /*
     * Later, if already in use, manage reentrant use by cloning the statement
     * and adding it to a linked list in the statementCache_
     */
    if (!result->use())
      throw Exception("A collection for '" + id + "' is already in use."
		      " Reentrant statement use is not yet implemented."); 

    return result;
  } else
    return 0;
}

void SqlConnection::saveStatement(const std::string& id,
				  SqlStatement *statement)
{
  statementCache_[id] = statement;
}

std::string SqlConnection::property(const std::string& name) const
{
  std::map<std::string, std::string>::const_iterator i = properties_.find(name);

  if (i != properties_.end())
    return i->second;
  else
    return std::string();
}

void SqlConnection::setProperty(const std::string& name,
				const std::string& value)
{
  properties_[name] = value;
  if (name == "show-queries") {
    std::cerr << "setProperty(\"show-queries\") is deprecated. One should use setShowQueriesHandler instead" << std::endl;
    if (value == "true") {
      setShowQueriesHandler([](const std::string& queryAsStr) {
        std::cerr << queryAsStr << std::endl;
      });
    } else {
      setShowQueriesHandler(nullptr);
    }
  }
}

bool SqlConnection::usesRowsFromTo() const
{
  return false;
}

LimitQuery SqlConnection::limitQueryMethod() const
{
  return Limit;
}

bool SqlConnection::supportAlterTable() const
{
  return false;
}

bool SqlConnection::supportDeferrableFKConstraint() const
{
  return false;
}

const char *SqlConnection::alterTableConstraintString() const
{
  return "constraint";
}

void SqlConnection::setShowQueriesHandler(std::function<void(const std::string &)> &&handler)
{
  queryLogger_ = std::move(handler);
}

void SqlConnection::showQueries(const std::string &queryAsStr) const
{
  if (queryLogger_) {
    queryLogger_(queryAsStr);
  }
}

void SqlConnection::setTransactionIsolationLevel(TransactionIsolationLevel transactionIsolationLevel) {
  transactionIsolationLevel_ = transactionIsolationLevel;
}

std::string SqlConnection::textType(int size) const
{
  if (size == -1)
    return "text";
  else{
    return "varchar(" + boost::lexical_cast<std::string>(size) + ")";
  }
}

std::string SqlConnection::longLongType() const
{
  return "bigint";
}

const char *SqlConnection::booleanType() const
{
  return "boolean";
}

bool SqlConnection::supportUpdateCascade() const
{
  return true;
}

bool SqlConnection::requireSubqueryAlias() const
{
  return false;
}

void SqlConnection::prepareForDropTables()
{ }

std::string SqlConnection::forUpdateClause(bool noWait) const
{
  return (!noWait) ? "FOR UPDATE" : "FOR UPDATE NOWAIT";
}

  }
}
