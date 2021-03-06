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
  : std::runtime_error("Code:" + code + ". " + error),
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

SerializationFailureException::SerializationFailureException(const std::string& error, const std::string& code)
  : RetriableException("SerializationFailure: " + error, code)
{ }

StaleObjectException::StaleObjectException(const std::string& id, int version)
  : RetriableException("Stale object, id = " + id + ", version = "
	      + boost::lexical_cast<std::string>(version))
{ }

ViolationException::ViolationException(const std::string& error, const std::string& code)
  : NonRetriableException("ViolationException: " + error, code)
{ }

ViolationException::ViolationException(const std::string &name, const std::string& error, const std::string& code)
  : NonRetriableException(name + ": " + error, code)
{ }

UniqViolationException::UniqViolationException(const std::string& error, const std::string& code)
  : ViolationException("UniqViolationException", error, code)
{ }

ForeignKeyViolationException::ForeignKeyViolationException(const std::string& error, const std::string& code)
  : ViolationException("ForeignKeyViolationException", error, code)
{ }

ConnectionDbFailureException::ConnectionDbFailureException(const std::string& error)
  : NonRetriableException(error)
{ }

ObjectNotFoundException::ObjectNotFoundException(const std::string& id)
  : NonRetriableException("Object not found, id = " + id)
{ }

NoUniqueResultException::NoUniqueResultException()
  : NonRetriableException("Query: resultValue(): more than one result")
{ }

  }
}
