#ifndef SHARE_VM_OPTO_SCNODE_HPP
#define SHARE_VM_OPTO_SCNODE_HPP

#include "opto/node.hpp"
#include "opto/opcodes.hpp"
#include "opto/subnode.hpp"
#if defined AD_MD_HPP
# include AD_MD_HPP
#elif defined TARGET_ARCH_MODEL_x86_32
# include "adfiles/ad_x86_32.hpp"
#elif defined TARGET_ARCH_MODEL_x86_64
# include "adfiles/ad_x86_64.hpp"
#elif defined TARGET_ARCH_MODEL_sparc
# include "adfiles/ad_sparc.hpp"
#elif defined TARGET_ARCH_MODEL_zero
# include "adfiles/ad_zero.hpp"
#elif defined TARGET_ARCH_MODEL_ppc_64
# include "adfiles/ad_ppc_64.hpp"
#endif

//-----------------------------SCCheckNode-----------------------------------
class SCCheckNode: public CmpNode {
private:

public:
  SCCheckNode(Node* ctrl, Node *oop1) : CmpNode(oop1,oop1) {
    init_req(0, ctrl);
    init_class_id(Class_SCCheck);
  }
  Node* obj_node() const { return in(1); }
  // FastLock and FastUnlockNode do not hash, we need one for each correspoding
  // LockNode/UnLockNode to avoid creating Phi's.
  virtual uint hash() const ;                  // { return NO_HASH; }
  virtual uint size_of() const;
  virtual uint cmp( const Node &n ) const ;    // Always fail, except on self
  virtual int Opcode() const;
  //virtual const Type *Value( PhaseTransform *phase ) const;
  virtual const Type *Value( PhaseTransform *phase ) const { return TypeInt::CC; }
  //virtual Node *Identity(PhaseTransform *phase);
  const Type *sub(const Type *t1, const Type *t2) const { return TypeInt::CC;}
};



#endif // SHARE_VM_OPTO_SCNODE_HPP
