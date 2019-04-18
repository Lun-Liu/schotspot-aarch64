#include "classfile/refStack.hpp"

RefStack::RefStack(u2 max_locals, u2 max_stack) :
    _stack_size(0),
    _max_locals(max_locals), _max_stack(max_stack) {
  _locals = NEW_RESOURCE_ARRAY(bool, _max_locals);
  _locals[0] = true;
  for (int i = 1; i < _max_locals; ++i) {
      _locals[i] = false;
  }
  _stack = NEW_RESOURCE_ARRAY(bool, _max_stack);
  for (int i = 0; i < _max_stack; ++i) {
      _stack[i] = false;
  }
}

RefStack::~RefStack(){
  FREE_RESOURCE_ARRAY(bool, _locals, _max_locals);
  FREE_RESOURCE_ARRAY(bool, _stack, _max_stack);
}

void RefStack::print_on(outputStream* str) const {
  str->indent().print("locals: {");
  for (int32_t i = 0; i < _max_locals; ++i) {
    str->print(" ");
    str->print("%s", _locals[i] ? "true": "false");
    if (i != _max_locals - 1) {
      str->print(",");
    }
  }
  str->print_cr(" }");
  str->indent().print("stack: {");
  for (int32_t j = 0; j < _stack_size; ++j) {
    str->print(" ");
    str->print("%s", _stack[j]? "true": "false");
    if (j != _stack_size - 1) {
      str->print(",");
    }
  }
  str->print_cr(" }");
}
