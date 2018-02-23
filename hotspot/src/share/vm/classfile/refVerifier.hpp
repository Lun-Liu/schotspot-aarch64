#ifndef SHARE_VM_CLASSFILE_REFVERIFIER_HPP
#define SHARE_VM_CLASSFILE_REFVERIFIER_HPP

#include "memory/gcLocker.hpp"
#include "oops/klass.hpp"
#include "oops/method.hpp"
#include "runtime/handles.hpp"
#include "utilities/growableArray.hpp"
#include "utilities/exceptions.hpp"
#include "classfile/refStack.hpp"


class RawBytecodeStream;
class RefStack;

// A new instance of this class is created for each class being verified
class RefVerifier : public StackObj {
 private:
  Thread* _thread;
  void verify_method(methodHandle method);
  void verify_switch(RawBytecodeStream* bcs, u4 code_length, char* code_data, bool* jmp_target);
  char* generate_code_data(methodHandle m, u4 code_length);

  instanceKlassHandle _klass;  // the class being verified
  methodHandle        _method; // current method being verified

 public:
  enum {
    BYTECODE_OFFSET = 1,
    NEW_OFFSET = 2
  };

  // constructor
  RefVerifier(instanceKlassHandle klass);

  // destructor
  ~RefVerifier();

  Thread* thread()             { return _thread; }
  methodHandle method()        { return _method; }
  instanceKlassHandle current_class() const { return _klass; }

  // Verifies the class.  If a verify or class file format error occurs,
  // the '_exception_name' symbols will set to the exception name and
  // the message_buffer will be filled in with the exception message.
  void verify_class();


};

#endif // SHARE_VM_CLASSFILE_REFVERIFIER_HPP
