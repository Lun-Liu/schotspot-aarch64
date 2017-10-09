#include "precompiled.hpp"
#include "opto/scnode.hpp"
#include "opto/parse.hpp"
#include "opto/rootnode.hpp"
#include "opto/runtime.hpp"

//-----------------------------hash--------------------------------------------
uint SCCheckNode::hash() const { return NO_HASH; }

uint SCCheckNode::size_of() const { return sizeof(*this); }

//------------------------------cmp--------------------------------------------
uint SCCheckNode::cmp( const Node &n ) const {
  return (&n == this);                // Always fail except on self
}

/*const Type* SCCheckNode::Value( PhaseTransform *phase) const {
  if(!OptimizeSCDynamic)
    return TypeInt::CC;
  const Node* obj = in(1);
  int obj_child_cnt = obj->outcnt();
  for(int i = 0; i < obj_child_cnt; i+=2){
    const Node* child = obj->raw_out(i);
    if(child->is_SCCheck() && child != this){
      //found previous sccheck, remove this one
      return TypeInt::CC_EQ;
    }
  }
  return TypeInt::CC;
}*/
