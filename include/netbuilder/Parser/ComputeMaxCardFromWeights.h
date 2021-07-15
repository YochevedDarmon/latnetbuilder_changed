// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2018  Pierre L'Ecuyer and Universite de Montreal
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef NETBUILDER__COMPUTE_MAX_CARD_FROM_WEIGHTS_H
#define NETBUILDER__COMPUTE_MAX_CARD_FROM_WEIGHTS_H

#include "latticetester/Weights.h"
#include "latbuilder/CombinedWeights.h"
#include "latticetester/OrderDependentWeights.h"
#include "latticetester/ProductWeights.h"
#include "latticetester/PODWeights.h"
#include "latticetester/ProjectionDependentWeights.h"

#include <sstream>
#include <string>

namespace NetBuilder { namespace Parser {

/**
 * Helper structure template to compute the maximal order of subprojections
 * with non zero weights when possible.
 */ 
template <typename WEIGHTS>
struct ComputeMaxCardFromWeights {
    unsigned int operator()(const WEIGHTS& w) const
    { 
        std::ostringstream myStream;
        myStream << w;
        throw std::invalid_argument("The figure of merit does not support " + myStream.str() + ".");
        return 0;
    }
};

#define DECLARE_COMPUTE_MAX_CARD(weight_type) \
    template <>\
    struct ComputeMaxCardFromWeights<weight_type> { \
    unsigned int operator()(const weight_type& w) const;\
    }

   DECLARE_COMPUTE_MAX_CARD(LatticeTester::OrderDependentWeights);
   DECLARE_COMPUTE_MAX_CARD(LatticeTester::ProductWeights);
   DECLARE_COMPUTE_MAX_CARD(LatticeTester::PODWeights);
   DECLARE_COMPUTE_MAX_CARD(LatticeTester::ProjectionDependentWeights);

#undef DECLARE_COMPUTE_MAX_CARD

/**
 * Template specialization in the case of OrderDependentWeights
 */ 
unsigned int ComputeMaxCardFromWeights<LatticeTester::OrderDependentWeights>::operator()(const LatticeTester::OrderDependentWeights& w) const{
    if (w.getDefaultWeight() > 0){
        std::ostringstream myStream;
        myStream << w;
        throw std::invalid_argument("The figure of merit does not support " + myStream.str() + ": \ndefault weight must be zero.");
    }
    unsigned int maxCard = 0;
    for (unsigned int i=1; i<w.getSize(); i++){
        if (w.getWeightForOrder(i) != 0){
            maxCard = i;
        }
    }
    return maxCard;
}

/**
 * Template specialization in the case of OrderDependentWeights
 */ 
unsigned int ComputeMaxCardFromWeights<LatticeTester::ProductWeights>::operator()(const LatticeTester::ProductWeights& w) const{
    if (w.getDefaultWeight() > 0){
        std::ostringstream myStream;
        myStream << w;
        throw std::invalid_argument("The figure of merit does not support " + myStream.str() + ": \ndefault weight must be zero.");
    }
    return (unsigned int) w.getWeights().size();
}

/**
 * Template specialization in the case of PODWeights.
 */ 
unsigned int ComputeMaxCardFromWeights<LatticeTester::PODWeights>::operator()(const LatticeTester::PODWeights& w) const{
    const LatticeTester::OrderDependentWeights& ordDepWeights = w.getOrderDependentWeights();
    const LatticeTester::ProductWeights& prodWeights = w .getProductWeights();
    unsigned int maxCardOD, maxCardPD;
    maxCardOD = ComputeMaxCardFromWeights<LatticeTester::OrderDependentWeights>()(ordDepWeights);
    try
    {
        maxCardPD = ComputeMaxCardFromWeights<LatticeTester::ProductWeights>()(prodWeights);
        return std::min(maxCardOD, maxCardPD);
    }
    catch (std::invalid_argument& e)
    {
        return maxCardOD;
    }
}

/**
 * Template specialization in the case of ProjectionDependentWeights.
 */ 
unsigned int ComputeMaxCardFromWeights<LatticeTester::ProjectionDependentWeights>::operator()(const LatticeTester::ProjectionDependentWeights& w) const{
    unsigned int maxCard = 0;
    for (unsigned int i=0; i<w.getSize(); i++){
        for (const auto &kv : w.getWeightsForLargestIndex(i)){
            maxCard = std::max(maxCard, (unsigned int) kv.first.size());
        }
    }
    return maxCard;
}

}}

#endif
