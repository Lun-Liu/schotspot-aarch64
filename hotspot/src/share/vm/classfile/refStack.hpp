#ifndef SHARE_VM_CLASSFILE_REFSTACK_HPP
#define SHARE_VM_CLASSFILE_REFSTACK_HPP

// A RefStack represents a stack used for bare field access detection

#include "oops/method.hpp"
#include "runtime/handles.hpp"
#include "runtime/signature.hpp"
#include "utilities/exceptions.hpp"

class RefStack : public ResourceObj {
 private:

  int32_t _stack_size;   // number of valid type elements in _stack

  int32_t _max_locals;
  int32_t _max_stack;

  bool* _locals; // local variable ref array
  bool* _stack;  // operand stack ref array


 public:
  // constructors

  RefStack(u2 max_locals, u2 max_stack);
  ~RefStack();

  inline void set_stack_size(u2 stack_size)   { _stack_size = stack_size; }
  inline bool* locals() const     { return _locals; }
  inline int32_t stack_size() const           { return _stack_size; }
  inline bool* stack() const      { return _stack; }
  inline int32_t max_locals() const           { return _max_locals; }
  inline int32_t max_stack() const            { return _max_stack; }

  // Set stack types to bogus
  inline void reset_stack() {
    int32_t i;
    for (i = 0; i < _max_stack; i++) {
      _stack[i] = false;
    }
    _stack_size = 0;
  }

  // Set locals types to bogus
  inline void reset_locals() {
    int32_t i;
    for (i = 0; i < _max_locals; i++) {
      _locals[i] = false;
    }
  }

  // Set locals types to bogus
  inline void reset_locals_but_0() {
    int32_t i;
    for (i = 1; i < _max_locals; i++) {
      _locals[i] = false;
    }
  }

  // Push type into stack type array.
  inline void push_stack(bool is_this) {
    if (_stack_size >= _max_stack) {
      //tty->print_cr("RefVerifier: stack overflow");
      return;
    }
    _stack[_stack_size++] = is_this;
  }

  inline void push_stack2(
      bool type1, bool type2) {
    if (_stack_size >= _max_stack - 1) {
      //tty->print_cr("RefVerifier: stack overflow");
      return;
    }
    _stack[_stack_size++] = type1;
    _stack[_stack_size++] = type2;
  }

  inline bool top_stack(){
    if(_stack_size <= 0){
        return false;
    }

    return _stack[_stack_size - 1];
  }

  //true of pop successfully,
  inline bool pop_stack() {
    if (_stack_size <= 0) {
      return false;
    }
    _stack_size--;
    return true;
  }

  inline bool pop_stack2() {
    if (_stack_size <= 1) {
      //tty->print_cr("RefVerifier: stack underflow");
      return false;
    }
    _stack_size -= 2;
    return true;
  }

  bool local_at(int index) {
    return _locals[index];
  }

  bool stack_at(int index) {
    return _stack[index];
  }

  void set_local(int index, bool ref){
    _locals[index] = ref;
  }

  void print_on(outputStream* str) const;
};

#endif // SHARE_VM_CLASSFILE_REFSTACK_HPP
