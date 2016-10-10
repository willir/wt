/*
 * Copyright (C) 2008 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#include "Wt/Dbo/Exception"

#include <boost/lexical_cast.hpp>

namespace Wt {
  namespace Dbo {

Exception::Exception(const std::string& error, const std::string& code)
  : std::runtime_error(error),
    code_(code)
{ }

Exception::~Exception() throw() { }

RetriableException::RetriableException(const std::string& error, const std::string& code)
  : Exception(error, code)
{ }

NonRetriableException::NonRetriableException(const std::string& error, const std::string& code)
  : Exception(error, code)
{ }

DeadLockException::DeadLockException(const std::string& error, const std::string& code)
  : RetriableException("DeadLock: " + error, code)
{ }

StaleObjectException::StaleObjectException(const std::string& id, int version)
  : RetriableException("Stale object, id = " + id + ", version = "
	      + boost::lexical_cast<std::string>(version))
{ }

ObjectNotFoundException::ObjectNotFoundException(const std::string& id)
  : NonRetriableException("Object not found, id = " + id)
{ }

NoUniqueResultException::NoUniqueResultException()
  : NonRetriableException("Query: resultValue(): more than one result")
{ }

  }
}
