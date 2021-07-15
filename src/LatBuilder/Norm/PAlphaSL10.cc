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

#include "latbuilder/Norm/PAlphaSL10.h"
#include "latbuilder/WeightsDispatcher.h"
#include "latbuilder/Util.h"

#include "latticetester/CoordinateSets.h"

#include <boost/math/special_functions/zeta.hpp>
#include <vector>
#include <cmath>

namespace LatBuilder { namespace Norm {

namespace SumHelperPAlphaSL10{

   template <typename WEIGHTS>
   struct SumHelper {
      Real operator()(
            const WEIGHTS& weights,
            Real normType,
            Real z,
            Real lambda,
            Dimension dimension
            ) const
      {
         std::cerr << "warning: using default implementation of SumHelper" << std::endl;
         Real val = 0.0;
         LatticeTester::CoordinateSets::FromRanges csets(1, dimension, 0, dimension - 1);
         for (const auto& proj : csets) {
            Real weight = weights.getWeight(proj);
            if (weight)
               // weights are assumed to already be to the power normType; map
               // them to power 2
               val += intPow(z, proj.size()) * std::pow(weight, lambda * 2 / normType);
         }
         return val;
      }
   };


#define DECLARE_PALPHA_SL10_SUM(weight_type) \
      template <> \
      class SumHelper<weight_type> { \
      public: \
         Real operator()( \
               const weight_type& weights, \
               Real normType, \
               Real z, \
               Real lambda, \
               Dimension dimension \
               ) const; \
      }

   DECLARE_PALPHA_SL10_SUM(LatticeTester::ProjectionDependentWeights);
   DECLARE_PALPHA_SL10_SUM(LatticeTester::OrderDependentWeights);
   DECLARE_PALPHA_SL10_SUM(LatticeTester::ProductWeights);
   DECLARE_PALPHA_SL10_SUM(LatticeTester::PODWeights);
   DECLARE_PALPHA_SL10_SUM(LatBuilder::CombinedWeights);

#undef DECLARE_PALPHA_SL10_SUM

   //===========================================================================
   // combined weights
   //===========================================================================

   // Separating sumCombined() from
   // SumHelper<LatBuilder::CombinedWeights>::operator() is a workaround for
   // LLVM/clang++.
   Real sumCombined(
         const CombinedWeights& weights,
         Real normType,
         Real z,
         Real lambda,
         Dimension dimension
         )
   {
      Real val = 0.0;
      for (const auto& w : weights.list())
         val += WeightsDispatcher::dispatch<SumHelper>(*w, normType, z, lambda * 2 / normType, dimension);
      return val;
   }

   Real SumHelper<LatBuilder::CombinedWeights>::operator()(
         const CombinedWeights& weights,
         Real normType,
         Real z,
         Real lambda,
         Dimension dimension
         ) const
   {
      return sumCombined(weights, normType, z, lambda, dimension);
   }


   //===========================================================================
   // projection-dependent weights
   //===========================================================================

   Real SumHelper<LatticeTester::ProjectionDependentWeights>::operator()(
         const LatticeTester::ProjectionDependentWeights& weights,
         Real normType,
         Real z,
         Real lambda,
         Dimension dimension
         ) const
   {
      Real val = 0.0;
      for (Dimension largestIndex = 0; largestIndex < dimension; largestIndex++) {
         // iterate only through projections that have a weight
         for (const auto& pw : weights.getWeightsForLargestIndex(largestIndex)) {
            const auto& proj = pw.first;
            const auto& weight = pw.second;
            if (weight)
               // weights are assumed to already be to the power normType; map
               // them to power 2
               val += intPow(z, proj.size()) * pow(weight, lambda * 2 / normType);
         }
      }
      return val;
   }


   //===========================================================================
   // order-dependent weights
   //===========================================================================

   Real SumHelper<LatticeTester::OrderDependentWeights>::operator()(
         const LatticeTester::OrderDependentWeights& weights,
         Real normType,
         Real z,
         Real lambda,
         Dimension dimension
         ) const
   {
      Real val = 0.0;
      Real cumul = 1.0;
      for (Dimension order = 1; order <= dimension; order++) {
         Real weight = weights.getWeightForOrder(order);
         cumul *= (dimension - order + 1) * z / order;
         if (weight)
               // weights are assumed to already be to the power normType; map
               // them to power 2
            val += cumul * std::pow(weight, lambda * 2 / normType);
      }
      return val;
   }


   //===========================================================================
   // product weights
   //===========================================================================

   Real SumHelper<LatticeTester::ProductWeights>::operator()(
         const LatticeTester::ProductWeights& weights,
         Real normType,
         Real z,
         Real lambda,
         Dimension dimension
         ) const
   {
      Real val = 1.0;
      for (Dimension coord = 0; coord < dimension; coord++) {
         Real weight = weights.getWeightForCoordinate(coord);
         if (weight)
               // weights are assumed to already be to the power normType; map
               // them to power 2
            val *= 1.0 + z * std::pow(weight, lambda * 2 / normType);
      }
      val -= 1.0;
      return val;
   }


   //===========================================================================
   // POD weights
   //===========================================================================

   Real SumHelper<LatticeTester::PODWeights>::operator()(
         const LatticeTester::PODWeights& weights,
         Real normType,
         Real z,
         Real lambda,
         Dimension dimension
         ) const
   {
      // compute states
      std::vector<Real> states;
      states.push_back(1.0);
      for (Dimension s = 1; s <= dimension; s++) {
         // weights are assumed to already be to the power normType; map
         // them to power 2
         Real pweight = std::pow(weights.getProductWeights().getWeightForCoordinate(s), lambda * 2 / normType);
         states.push_back(0.0);
         for (Dimension order = states.size() - 1; order > 0; order--)
            states[order] += z * pweight * states[order - 1];
      }
      Real val = 0.0;
      for (Dimension order = 1; order <= dimension; order++)
         // weights are assumed to already be to the power normType; map
         // them to power 2
         val += std::pow(weights.getOrderDependentWeights().getWeightForOrder(order), lambda * 2 / normType) * states[order];
      return val;
   }

}

PAlphaSL10::PAlphaSL10(unsigned int alpha, const LatticeTester::Weights& weights, Real normType):
   NormAlphaBase<PAlphaSL10>(alpha, normType),
   m_weights(weights)
{}

template <LatticeType LR, EmbeddingType L>
Real PAlphaSL10::value(
      Real lambda,
      const SizeParam<LR, L>& sizeParam,
      Dimension dimension,
      Real norm
      ) const
{
   norm = 1.0 / (norm * sizeParam.totient());
   Real z = static_cast<Real>(2 * boost::math::zeta<Real>(this->alpha() * lambda));
   Real val = WeightsDispatcher::dispatch<SumHelperPAlphaSL10::SumHelper>(
         m_weights,
         this->normType(),
         z,
         lambda,
         dimension
         );

   return std::pow(norm * val, 1.0 / lambda);
}

template Real PAlphaSL10::value<LatticeType::ORDINARY, EmbeddingType::UNILEVEL>(Real, const SizeParam<LatticeType::ORDINARY, EmbeddingType::UNILEVEL>&, Dimension, Real) const;
template Real PAlphaSL10::value<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL>(Real, const SizeParam<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL>&, Dimension, Real) const;

template Real PAlphaSL10::value<LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL>(Real, const SizeParam<LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL>&, Dimension, Real) const;
template Real PAlphaSL10::value<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL>(Real, const SizeParam<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL>&, Dimension, Real) const;

}}
