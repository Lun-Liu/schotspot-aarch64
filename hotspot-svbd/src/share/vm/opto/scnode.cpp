#include "precompiled.hpp"
#include "opto/scnode.hpp"
#include "opto/parse.hpp"
#include "opto/rootnode.hpp"
#include "opto/runtime.hpp"


uint SCCheckNode::size_of() const { return sizeof(*this); }

//------------------------------cmp--------------------------------------------
uint SCCheckNode::cmp( const Node &n ) const {
  return (&n == this);                // Always fail except on self
}

Node* SCCheckNode::Identity(PhaseTransform *phase) {
  return this;


}

const Type* SCCheckNode::Value( PhaseTransform *phase) const {
  return TypeInt::CC;
}
