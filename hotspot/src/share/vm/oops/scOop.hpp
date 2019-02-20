#ifndef SHARE_VM_OOPS_SCOOP_HPP
#define SHARE_VM_OOPS_SCOOP_HPP

#include "oops/oop.hpp"

// The scOop describes the header that is used to detect whether an
// object is single-threaded.
//
// Note that the mark is not a real oop but just a word.
// It is placed in the oop hierarchy for historical reasons.
//
// Only 64 bits are supported now
//
// Bit-format of an object header (most significant first, big endian layout below):
//
//
//  64 bits:
//  --------
//  JavaThread*:54 unused:8 unsafe:1 written:1 (see below)
//
//  - JavaThread set to 54 to be consistent with biased locking
//  - the written bit is used to describe if there is any write before
//  - when the unsafe bit is set, there is conflict before
//
//    We assume that stack/thread pointers have the lowest two bits cleared.

class BasicLock;
class ObjectMonitor;
class JavaThread;

class scOopDesc: public oopDesc {
 private:
  // Conversion
  uintptr_t value() const { return (uintptr_t) this; }

 public:
  // Constants
  enum { thread_bits              = 54,
         unused_bits              = 8,
         unsafe_bits              = 1,
         written_bits             = 1
  };

  // The biased locking code currently requires that the age bits be
  // contiguous to the lock bits.
  enum { written_shift            = 0,
         unsafe_shift             = written_bits,
         thread_shift             = written_bits + unsafe_bits + unused_bits
  };

  enum { written_mask_in_place    = 1 << written_shift,
         unsafe_mask_in_place     = 1 << unsafe_shift,
         thread_mask              = right_n_bits(thread_bits),
         thread_mask_in_place     = thread_mask << thread_shift
  };

  enum {
         written_value            = 1,
         unsafe_value             = 2
  };

  // Alignment of JavaThread pointers encoded in object header required by biased locking
  enum { thread_alignment    = 2 << thread_shift    //not used really, is the same as markOopDesc::biased_lock_alignment
  };

  bool is_written() const{
    return (mask_bits(value(), written_mask_in_place));
  }

  bool is_unsafe() const{
    return (mask_bits(value(),unsafe_mask_in_place));
  }

  JavaThread* owner_thread() const{
    return (JavaThread*) ((intptr_t) (mask_bits(value(), thread_mask_in_place)));
  }

  scOop set_written() const{
    return scOop(value() | written_value);
  }

  scOop set_unsafe() const{
    return scOop(value() | unsafe_value);
  }

  scOop write_by_thread(JavaThread* thread){
    intptr_t tmp = (intptr_t) thread;
    return (scOop) (tmp & thread_mask_in_place | written_value);
  }

  static scOop prototype() {
    return scOop(0);
  }

};


#endif // SHARE_VM_OOPS_SCOOP_HPP
