#include "precompiled.hpp"
#include "opto/scnode.hpp"
#include "opto/parse.hpp"
#include "opto/rootnode.hpp"
#include "opto/runtime.hpp"

//-----------------------------hash--------------------------------------------
//uint SCCheckNode::hash() const { 
//	return NO_HASH;
//}


uint SCCheckNode::size_of() const { return sizeof(*this); }

//------------------------------cmp--------------------------------------------
uint SCCheckNode::cmp( const Node &n ) const {
  return (&n == this);                // Always fail except on self
}

Node* SCCheckNode::Identity(PhaseTransform *phase) {
  //go all the way up through control node
  //Node* ctrl = in(0);
  //while(true){
  //  if(ctrl->is_Region() && ctrl->req() == 2){
  //    ctrl = ctrl->in(1);
  //  } else if (ctrl -> is_Proj() && (ctrl->in(0)->is_MemBar() || ctrl->in(0)->is_SC())){
  //    ctrl = ctrl->in(0)->in(0);
  //  } else {
  //    break;
  //  }
  //}
  //int cnt = ctrl->outcnt();
  //for(int i = 0; i < cnt; i++){
  //  Node* child = ctrl->raw_out(i);
  //  if(child->is_SCCheck() && child->in(1) == in(1)&&child!=this){
  //    return child;
  //  }
  //}
  return this;


}

const Type* SCCheckNode::Value( PhaseTransform *phase) const {
  //if(!OptimizeSCDynamic)
  //  return TypeInt::CC;
  //const Node* obj = in(1);
  //int obj_child_cnt = obj->outcnt();
  //for(int i = 0; i < obj_child_cnt; i+=2){
  //  const Node* child = obj->raw_out(i);
  //  if(child->is_SCCheck() && child != this){
  //    //found previous sccheck, remove this one
  //    return TypeInt::CC_EQ;
  //  }
  //}
  return TypeInt::CC;
}
