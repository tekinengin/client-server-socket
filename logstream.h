// $Id: logstream.h,v 1.6 2021-02-24 15:29:53-08 - - $

//
// class logstream
// replacement for initial cout so that each call to a logstream
// will prefix the line of output with an identification string
// and a process id.  Template functions must be in header files
// and the others are trivial.
//

#ifndef __LOGSTREAM_H__
#define __LOGSTREAM_H__

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include <sys/types.h>
#include <unistd.h>

class logstream {
   private:
      ostream& out_;
      string execname_;
   public:

      // Constructor may or may not have the execname available.
      logstream (ostream& out, const string& execname = ""):
                 out_ (out), execname_ (execname) {
      }

      // First line of main should set execname if logstream is global.
      void execname (const string& name) { execname_ = name; }
      string execname() { return execname_; }

      // First call should be the logstream, not cout.
      // Then forward result to the standard ostream.
      template <typename T>
      ostream& operator<< (const T& obj) {
         assert (execname_.size() > 0);
         out_ << execname_ << "(" << getpid() << "): " << obj;
         return out_;
      }

};

#endif

