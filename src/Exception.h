#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <exception>
#include <iostream>

using namespace std;

/* C++ error handeling class for passing simple strings. */
class Exception : public exception {
private:
  string mes;

public:
  Exception(const char *s) : mes(s) {}

  virtual ~Exception() throw() {}

  virtual const char *what() const throw() { return (mes.c_str()); }
};

#endif // _EXCEPTION_H_
