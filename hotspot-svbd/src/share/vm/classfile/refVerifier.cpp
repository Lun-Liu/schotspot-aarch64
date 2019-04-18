
#include "precompiled.hpp"
#include "classfile/classFileStream.hpp"
#include "classfile/javaClasses.hpp"
#include "classfile/refStack.hpp"
#include "classfile/systemDictionary.hpp"
#include "classfile/refVerifier.hpp"
#include "classfile/vmSymbols.hpp"
#include "interpreter/bytecodes.hpp"
#include "interpreter/bytecodeStream.hpp"
#include "memory/oopFactory.hpp"
#include "memory/resourceArea.hpp"
#include "oops/instanceKlass.hpp"
#include "oops/oop.inline.hpp"
#include "oops/typeArrayOop.hpp"
#include "prims/jvm.h"
#include "runtime/fieldDescriptor.hpp"
#include "runtime/handles.inline.hpp"
#include "runtime/interfaceSupport.hpp"
#include "runtime/javaCalls.hpp"
#include "runtime/orderAccess.inline.hpp"
#include "runtime/os.hpp"
#ifdef TARGET_ARCH_x86
# include "bytes_x86.hpp"
#endif
#ifdef TARGET_ARCH_sparc
# include "bytes_sparc.hpp"
#endif
#ifdef TARGET_ARCH_zero
# include "bytes_zero.hpp"
#endif
#ifdef TARGET_ARCH_arm
# include "bytes_arm.hpp"
#endif
#ifdef TARGET_ARCH_ppc
# include "bytes_ppc.hpp"
#endif

// Methods in RefVerifier

RefVerifier::RefVerifier(
    instanceKlassHandle klass)
    : _klass(klass), 
      _direct_get_count(0), _common_get_count(0),
      _direct_put_count(0), _common_put_count(0) {
}

RefVerifier::~RefVerifier() {
}

void RefVerifier::verify_class() {

  Array<Method*>* methods = _klass->methods();
  int num_methods = methods->length();

  for (int index = 0; index < num_methods; index++) {
    Method* m = methods->at(index);
    if (m->is_native() || m->is_abstract() || m->is_overpass() || m->is_static()) {
      // If m is native or abstract, skip it.  It is checked in class file
      // parser that methods do not override a final method.  Overpass methods
      // are trusted since the VM generates them.
      continue;
    }
    verify_method(methodHandle(m));
  }
}

void RefVerifier::verify_method(methodHandle m) {
  HandleMark hm();
  _method = m;   // initialize _method
  //tty->print("RefVerifier: verify_method: ");
  //m->print_short_name();
  ///tty->print_cr(".");

  int32_t max_stack = m->verifier_max_stack();
  int32_t max_locals = m->max_locals();
  constantPoolHandle cp(m->constants());

  u4 code_length = m->code_size();

  // Scan the bytecode and map each instruction's start offset to a number.
  char* code_data = generate_code_data(m, code_length);

  RawBytecodeStream bcs(m);

  // Scan the byte code linearly from the start to the end
  bool no_control_flow = false; // Set to true when there is no direct control
                                // flow from current instruction to the next
                                // instruction in sequence
  RefStack stack(max_locals, max_stack);
  bool* jmp_target = NEW_RESOURCE_ARRAY(bool, code_length);
  for(u4 i = 0; i < code_length; i++){
    jmp_target[i] = false;
  }

  Bytecodes::Code opcode;
  bool change_0 = false;
  while (!bcs.is_last_bytecode()) {
    // Check for recursive re-verification before each bytecode.
    opcode = bcs.raw_next();
    u2 bci = bcs.bci();

    {
      u2 index;
      int target;

      switch (opcode) {
        case Bytecodes::_if_icmpeq:
        case Bytecodes::_if_icmpne:
        case Bytecodes::_if_icmplt:
        case Bytecodes::_if_icmpge:
        case Bytecodes::_if_icmpgt:
        case Bytecodes::_if_icmple:
        case Bytecodes::_if_acmpeq :
        case Bytecodes::_if_acmpne :
        case Bytecodes::_ifeq:
        case Bytecodes::_ifne:
        case Bytecodes::_iflt:
        case Bytecodes::_ifge:
        case Bytecodes::_ifgt:
        case Bytecodes::_ifle:
        case Bytecodes::_ifnull :
        case Bytecodes::_ifnonnull :
        case Bytecodes::_goto :
            target = bcs.dest();
            jmp_target[bci] = true;
            break;
        case Bytecodes::_goto_w :
            target = bcs.dest_w();
            jmp_target[bci] = true;
            break;
        case Bytecodes::_tableswitch :
        case Bytecodes::_lookupswitch :
            verify_switch(&bcs, code_length, code_data, jmp_target);
            break;
        case Bytecodes::_athrow :
            break;
        case Bytecodes::_istore_0:
        case Bytecodes::_lstore_0:
        case Bytecodes::_fstore_0:
        case Bytecodes::_dstore_0:
        case Bytecodes::_astore_0:
            change_0 = true;
            break;
        case Bytecodes::_istore:
        case Bytecodes::_lstore:
        case Bytecodes::_fstore:
        case Bytecodes::_dstore:
        case Bytecodes::_astore:
            if(bcs.get_index() == 0){
                change_0 = true;
            }
            break;
        default:
            break;
      }  // end switch
    }
  }

  bcs.set_start(0);
  while (!bcs.is_last_bytecode()) {
    // Check for recursive re-verification before each bytecode.
    opcode = bcs.raw_next();
    u2 bci = bcs.bci();
    if(jmp_target[bci]){
        stack.reset_stack();
        if(change_0)
          stack.reset_locals();
        else
          stack.reset_locals_but_0();
    }

    //tty->print("bci: %d", bci);
    //stack.print_on(tty);
    {
      u2 index;
      int target;

      switch (opcode) {
        case Bytecodes::_nop :
            break;
        case Bytecodes::_aconst_null :
        case Bytecodes::_iconst_m1 :
        case Bytecodes::_iconst_0 :
        case Bytecodes::_iconst_1 :
        case Bytecodes::_iconst_2 :
        case Bytecodes::_iconst_3 :
        case Bytecodes::_iconst_4 :
        case Bytecodes::_iconst_5 :
        case Bytecodes::_fconst_0 :
        case Bytecodes::_fconst_1 :
        case Bytecodes::_fconst_2 :
        case Bytecodes::_sipush :
        case Bytecodes::_bipush :
        case Bytecodes::_ldc :
        case Bytecodes::_ldc_w :
        case Bytecodes::_iload :
        case Bytecodes::_iload_0 :
        case Bytecodes::_iload_1 :
        case Bytecodes::_iload_2 :
        case Bytecodes::_iload_3 :
        case Bytecodes::_fload :
        case Bytecodes::_fload_0 :
        case Bytecodes::_fload_1 :
        case Bytecodes::_fload_2 :
        case Bytecodes::_fload_3 :
            stack.push_stack(false);
            break;
        case Bytecodes::_lconst_0 :
        case Bytecodes::_lconst_1 :
        case Bytecodes::_dconst_0 :
        case Bytecodes::_dconst_1 :
        case Bytecodes::_ldc2_w :
        case Bytecodes::_lload :
        case Bytecodes::_lload_0 :
        case Bytecodes::_lload_1 :
        case Bytecodes::_lload_2 :
        case Bytecodes::_lload_3 :
        case Bytecodes::_dload :
        case Bytecodes::_dload_0 :
        case Bytecodes::_dload_1 :
        case Bytecodes::_dload_2 :
        case Bytecodes::_dload_3 :
            stack.push_stack2(false, false);
            break;
        case Bytecodes::_aload :
            stack.push_stack(stack.local_at(bcs.get_index()));
            break;
        case Bytecodes::_aload_0 :
        case Bytecodes::_aload_1 :
        case Bytecodes::_aload_2 :
        case Bytecodes::_aload_3 :
            index = opcode - Bytecodes::_aload_0;
            stack.push_stack(stack.local_at(index));
            break;
        case Bytecodes::_iaload :
        case Bytecodes::_baload :
        case Bytecodes::_caload :
        case Bytecodes::_saload :
        case Bytecodes::_faload :
        case Bytecodes::_aaload :
            if(stack.pop_stack2())
                stack.push_stack(false);
            break;
        case Bytecodes::_laload :
        case Bytecodes::_daload :
            if(stack.pop_stack2())
                stack.push_stack2(false, false);
            break;
        case Bytecodes::_istore :
        case Bytecodes::_fstore :
            if(stack.pop_stack())
                stack.set_local(bcs.get_index(), false);
            break;
        case Bytecodes::_lstore :
        case Bytecodes::_dstore :
            if(stack.pop_stack2())
                stack.set_local(bcs.get_index(), false);
            break;
        case Bytecodes::_istore_0 :
        case Bytecodes::_istore_1 :
        case Bytecodes::_istore_2 :
        case Bytecodes::_istore_3 :
            if(stack.pop_stack()){
                index = opcode - Bytecodes::_istore_0;
                stack.set_local(index, false);
            }
            break;
        case Bytecodes::_fstore_0 :
        case Bytecodes::_fstore_1 :
        case Bytecodes::_fstore_2 :
        case Bytecodes::_fstore_3 :
            if(stack.pop_stack()){
                index = opcode - Bytecodes::_fstore_0;
                stack.set_local(index, false);
            }
            break;
        case Bytecodes::_lstore_0 :
        case Bytecodes::_lstore_1 :
        case Bytecodes::_lstore_2 :
        case Bytecodes::_lstore_3 :
            if(stack.pop_stack2()){
                index = opcode - Bytecodes::_lstore_0;
                stack.set_local(index, false);
            }
            break;
        case Bytecodes::_dstore_0 :
        case Bytecodes::_dstore_1 :
        case Bytecodes::_dstore_2 :
        case Bytecodes::_dstore_3 :
            if(stack.pop_stack2()){
                index = opcode - Bytecodes::_dstore_0;
                stack.set_local(index, false);
            }
            break;
            break;
        case Bytecodes::_astore :
            {
                bool top = stack.top_stack();
                if(stack.pop_stack()){
                    stack.set_local(bcs.get_index(), top);
                }
            }
            break;
        case Bytecodes::_astore_0 :
        case Bytecodes::_astore_1 :
        case Bytecodes::_astore_2 :
        case Bytecodes::_astore_3 :
            {
                index = opcode - Bytecodes::_astore_0;
                bool top = stack.top_stack();
                if(stack.pop_stack()){
                    stack.set_local(index, top);
                }
            }
            break;
        case Bytecodes::_iastore :
        case Bytecodes::_bastore :
        case Bytecodes::_castore :
        case Bytecodes::_sastore :
        case Bytecodes::_fastore :
        case Bytecodes::_aastore :
            stack.pop_stack();
            stack.pop_stack();
            stack.pop_stack();
            break;
        case Bytecodes::_lastore :
        case Bytecodes::_dastore :
            stack.pop_stack2();
            stack.pop_stack();
            stack.pop_stack();
            break;
        case Bytecodes::_pop :
            stack.pop_stack();
            break;
        case Bytecodes::_pop2 :
            stack.pop_stack2();
            break;
        case Bytecodes::_dup :
            {
                bool top = stack.top_stack();
                if(stack.pop_stack()){
                    stack.push_stack(top);
                    stack.push_stack(top);
                }
                break;
            }
        case Bytecodes::_dup_x1 :
            {
                if(stack.stack_size()>=2){
                    bool top1 = stack.top_stack();
                    stack.pop_stack();
                    bool top2 = stack.top_stack();
                    stack.pop_stack();
                    stack.push_stack(top1);
                    stack.push_stack(top2);
                    stack.push_stack(top1);
                }
                break;
            }
        case Bytecodes::_dup_x2 :
            {
                if(stack.stack_size()>=3){
                    bool top1 = stack.top_stack();
                    stack.pop_stack();
                    bool top2 = stack.top_stack();
                    stack.pop_stack();
                    bool top3 = stack.top_stack();
                    stack.pop_stack();
                    stack.push_stack(top1);
                    stack.push_stack(top3);
                    stack.push_stack(top2);
                    stack.push_stack(top1);
                }
                break;
            }
        case Bytecodes::_dup2 :
            {
                if(stack.stack_size()>=2){
                    bool top1 = stack.top_stack();
                    stack.pop_stack();
                    bool top2 = stack.top_stack();
                    stack.pop_stack();
                    stack.push_stack(top2);
                    stack.push_stack(top1);
                    stack.push_stack(top2);
                    stack.push_stack(top1);
                }
                break;
            }
        case Bytecodes::_dup2_x1 :
            {
                if(stack.stack_size()>=3){
                    bool top1 = stack.top_stack();
                    stack.pop_stack();
                    bool top2 = stack.top_stack();
                    stack.pop_stack();
                    bool top3 = stack.top_stack();
                    stack.pop_stack();
                    stack.push_stack(top2);
                    stack.push_stack(top1);
                    stack.push_stack(top3);
                    stack.push_stack(top2);
                    stack.push_stack(top1);
                }
                break;
            }
        case Bytecodes::_dup2_x2 :
            {
                if(stack.stack_size()>=4){
                    bool top1 = stack.top_stack();
                    stack.pop_stack();
                    bool top2 = stack.top_stack();
                    stack.pop_stack();
                    bool top3 = stack.top_stack();
                    stack.pop_stack();
                    bool top4 = stack.top_stack();
                    stack.pop_stack();
                    stack.push_stack(top2);
                    stack.push_stack(top1);
                    stack.push_stack(top4);
                    stack.push_stack(top3);
                    stack.push_stack(top2);
                    stack.push_stack(top1);
                }
                break;
            }
        case Bytecodes::_swap :
            {
                if(stack.stack_size()>=2){
                    bool top1 = stack.top_stack();
                    stack.pop_stack();
                    bool top2 = stack.top_stack();
                    stack.pop_stack();
                    stack.push_stack(top1);
                    stack.push_stack(top2);
                }
                break;
            }
        case Bytecodes::_iadd :
        case Bytecodes::_isub :
        case Bytecodes::_imul :
        case Bytecodes::_idiv :
        case Bytecodes::_irem :
        case Bytecodes::_ishl :
        case Bytecodes::_ishr :
        case Bytecodes::_iushr :
        case Bytecodes::_ior :
        case Bytecodes::_ixor :
        case Bytecodes::_iand :
        case Bytecodes::_ineg :
        case Bytecodes::_fadd :
        case Bytecodes::_fsub :
        case Bytecodes::_fmul :
        case Bytecodes::_fdiv :
        case Bytecodes::_frem :
        case Bytecodes::_fneg :
            if(stack.pop_stack2())
                stack.push_stack(false);
            break;
        case Bytecodes::_ladd :
        case Bytecodes::_lsub :
        case Bytecodes::_lmul :
        case Bytecodes::_ldiv :
        case Bytecodes::_lrem :
        case Bytecodes::_land :
        case Bytecodes::_lor :
        case Bytecodes::_lxor :
        case Bytecodes::_lneg :
        case Bytecodes::_lshl :
        case Bytecodes::_lshr :
        case Bytecodes::_lushr :
        case Bytecodes::_dadd :
        case Bytecodes::_dsub :
        case Bytecodes::_dmul :
        case Bytecodes::_ddiv :
        case Bytecodes::_drem :
        case Bytecodes::_dneg :
            if(stack.stack_size()>=4){
                stack.pop_stack2();
                stack.pop_stack2();
                stack.push_stack2(false, false);
            }
            break;
        case Bytecodes::_iinc :
        case Bytecodes::_i2f :
        case Bytecodes::_f2i :
        case Bytecodes::_l2d :
        case Bytecodes::_d2l :
        case Bytecodes::_i2b :
        case Bytecodes::_i2c :
        case Bytecodes::_i2s :
            break;
        case Bytecodes::_i2l :
        case Bytecodes::_i2d :
        case Bytecodes::_f2l :
        case Bytecodes::_f2d :
            stack.push_stack(false);
            break;
        case Bytecodes::_l2i :
        case Bytecodes::_l2f :
        case Bytecodes::_d2i :
        case Bytecodes::_d2f :
            stack.pop_stack();
            break;
        case Bytecodes::_lcmp :
        case Bytecodes::_dcmpl :
        case Bytecodes::_dcmpg :
            if(stack.stack_size()>=4){
                stack.pop_stack2();
                stack.pop_stack2();
                stack.push_stack(false);
            }
            break;
        case Bytecodes::_fcmpl :
        case Bytecodes::_fcmpg :
            if(stack.stack_size()>=2){
                stack.pop_stack();
                stack.pop_stack();
                stack.push_stack(false);
            }
            break;
        case Bytecodes::_if_icmpeq:
        case Bytecodes::_if_icmpne:
        case Bytecodes::_if_icmplt:
        case Bytecodes::_if_icmpge:
        case Bytecodes::_if_icmpgt:
        case Bytecodes::_if_icmple:
        case Bytecodes::_if_acmpeq :
        case Bytecodes::_if_acmpne :
            stack.pop_stack2();
            break;
        case Bytecodes::_ifeq:
        case Bytecodes::_ifne:
        case Bytecodes::_iflt:
        case Bytecodes::_ifge:
        case Bytecodes::_ifgt:
        case Bytecodes::_ifle:
        case Bytecodes::_ifnull :
        case Bytecodes::_ifnonnull :
            stack.pop_stack();
            break;
        case Bytecodes::_goto :
        case Bytecodes::_goto_w :
        case Bytecodes::_return :
            break;
        case Bytecodes::_tableswitch :
        case Bytecodes::_lookupswitch :
        case Bytecodes::_ireturn :
        case Bytecodes::_freturn :
        case Bytecodes::_areturn :
            stack.pop_stack();
            break;
        case Bytecodes::_dreturn :
        case Bytecodes::_lreturn :
            stack.pop_stack2();
            break;
        case Bytecodes::_putstatic :
            {
            index = bcs.get_index_u2();
            Symbol* field_sig = cp->signature_ref_at(index);
            SignatureStream sig_stream(field_sig, false);
            BasicType bt = sig_stream.type();
            switch(bt){
                case T_OBJECT:
                case T_ARRAY:
                case T_INT:
                case T_BOOLEAN:
                case T_BYTE:
                case T_CHAR:
                case T_SHORT:
                case T_FLOAT:
                    stack.pop_stack();
                    break;
                case T_LONG:
                case T_DOUBLE:
                    stack.pop_stack2();
                    break;
                default:
                    ShouldNotReachHere();
                    break;
            }
            break;
            }
        case Bytecodes::_getstatic :
            {
            index = bcs.get_index_u2();
            Symbol* field_sig = cp->signature_ref_at(index);
            SignatureStream sig_stream(field_sig, false);
            BasicType bt = sig_stream.type();
            switch(bt){
                case T_OBJECT:
                case T_ARRAY:
                case T_INT:
                case T_BOOLEAN:
                case T_BYTE:
                case T_CHAR:
                case T_SHORT:
                case T_FLOAT:
                    stack.push_stack(false);
                    break;
                case T_LONG:
                case T_DOUBLE:
                    stack.push_stack2(false, false);
                    break;
                default:
                    ShouldNotReachHere();
                    break;
            }
            break;
            }
        case Bytecodes::_putfield :
            {
            index = bcs.get_index_u2();
            Symbol* field_sig = cp->signature_ref_at(index);
            SignatureStream sig_stream(field_sig, false);
            BasicType bt = sig_stream.type();
            switch(bt){
                case T_OBJECT:
                case T_ARRAY:
                case T_INT:
                case T_BOOLEAN:
                case T_BYTE:
                case T_CHAR:
                case T_SHORT:
                case T_FLOAT:
                    stack.pop_stack();
                    break;
                case T_LONG:
                case T_DOUBLE:
                    stack.pop_stack2();
                    break;
                default:
                    ShouldNotReachHere();
                    break;
            }
            bool classtype = stack.top_stack();
            if(!stack.pop_stack() || !classtype){
              //tty->print_cr("direct");
              *(m->bcp_from(bci)) = Bytecodes::_direct_putfield;
	      _direct_put_count++;
            }else{
              _common_put_count++;
	    }

            break;
            }
        case Bytecodes::_getfield :
            {
            bool classtype = stack.top_stack();
            bool success = stack.pop_stack();
            if(!success || !classtype){
              //tty->print_cr("direct");
              *m->bcp_from(bci) = Bytecodes::_direct_getfield;
	      _direct_get_count++;
            }else{
              _common_get_count++;
	    }

            if(!success)
                break;
            index = bcs.get_index_u2();
            Symbol* field_sig = cp->signature_ref_at(index);
            SignatureStream sig_stream(field_sig, false);
            BasicType bt = sig_stream.type();
            switch(bt){
                case T_OBJECT:
                case T_ARRAY:
                case T_INT:
                case T_BOOLEAN:
                case T_BYTE:
                case T_CHAR:
                case T_SHORT:
                case T_FLOAT:
                    stack.push_stack(false);
                    break;
                case T_LONG:
                case T_DOUBLE:
                    stack.push_stack2(false, false);
                    break;
                default:
                    ShouldNotReachHere();
                    break;
            }
            break;
            }
        case Bytecodes::_invokevirtual :
        case Bytecodes::_invokespecial :
        case Bytecodes::_invokestatic :
        case Bytecodes::_invokeinterface :
        case Bytecodes::_invokedynamic :
            //conservative here, reset stack
            stack.reset_stack();
            break;
        case Bytecodes::_new :
            stack.push_stack(false);
            break;
        case Bytecodes::_newarray :
        case Bytecodes::_anewarray :
        case Bytecodes::_arraylength :
            if(stack.pop_stack())
                stack.push_stack(false);
            break;
        case Bytecodes::_checkcast :
        case Bytecodes::_instanceof :
        case Bytecodes::_athrow :
            break;
        case Bytecodes::_monitorenter :
        case Bytecodes::_monitorexit :
            stack.pop_stack();
            break;
        case Bytecodes::_multianewarray :
            {
            u2 dim = *(bcs.bcp()+3);
            for (int i = 0; i < dim; i++) {
                if(!stack.pop_stack()) break;;
            }
            stack.push_stack(false);
            break;
            }
        case Bytecodes::_jsr:
        case Bytecodes::_ret:
            break;
        default:
          // We only need to check the valid bytecodes in class file.
          // And jsr and ret are not in the new class file format in JDK1.5.
          tty->print_cr("Bad instruction at location %d: %02x", bci, opcode);
          no_control_flow = false;
          return;
      }  // end switch
    }  // end Merge with the next instruction
  }  // end while
  if(_common_put_count > 0 || _common_get_count > 0){
    m->set_has_sc_check();
  }else {
    m->clear_has_sc_check();
    //tty->print_cr("no sc check for method");
  }
  FREE_RESOURCE_ARRAY(bool, jmp_target, code_length);
}

void RefVerifier::verify_switch(
        RawBytecodeStream* bcs, u4 code_length, char* code_data, bool* jmp_target) {
    int bci = bcs->bci();
    address bcp = bcs->bcp();
    address aligned_bcp = (address) round_to((intptr_t)(bcp + 1), jintSize);

    int default_offset = (int) Bytes::get_Java_u4(aligned_bcp);
    int keys, delta;
    if (bcs->raw_code() == Bytecodes::_tableswitch) {
        jint low = (jint)Bytes::get_Java_u4(aligned_bcp + jintSize);
        jint high = (jint)Bytes::get_Java_u4(aligned_bcp + 2*jintSize);
        keys = high - low + 1;
        delta = 1;
    } else {
        keys = (int)Bytes::get_Java_u4(aligned_bcp + jintSize);
        delta = 2;
        // Make sure that the lookupswitch items are sorted
        for (int i = 0; i < (keys - 1); i++) {
            jint this_key = Bytes::get_Java_u4(aligned_bcp + (2+2*i)*jintSize);
            jint next_key = Bytes::get_Java_u4(aligned_bcp + (2+2*i+2)*jintSize);
        }
    }
    int target = bci + default_offset;
    jmp_target[target] = true;
    for (int i = 0; i < keys; i++) {
        // Because check_jump_target() may safepoint, the bytecode could have
        // moved, which means 'aligned_bcp' is no good and needs to be recalculated.
        aligned_bcp = (address)round_to((intptr_t)(bcs->bcp() + 1), jintSize);
        target = bci + (jint)Bytes::get_Java_u4(aligned_bcp+(3+i*delta)*jintSize);
        jmp_target[target] = true;
    }
}


char* RefVerifier::generate_code_data(methodHandle m, u4 code_length) {
  char* code_data = NEW_RESOURCE_ARRAY(char, code_length);
  memset(code_data, 0, sizeof(char) * code_length);
  RawBytecodeStream bcs(m);

  while (!bcs.is_last_bytecode()) {
    if (bcs.raw_next() != Bytecodes::_illegal) {
      int bci = bcs.bci();
      if (bcs.raw_code() == Bytecodes::_new) {
        code_data[bci] = NEW_OFFSET;
      } else {
        code_data[bci] = BYTECODE_OFFSET;
      }
    } else {
      tty->print_cr("Bad instruction at %d", bcs.bci());
      return NULL;
    }
  }

  return code_data;
}
