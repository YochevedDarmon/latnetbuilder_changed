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

#ifndef LATBUILDER__PARSER__LEVEL_WEIGHTS_H
#define LATBUILDER__PARSER__LEVEL_WEIGHTS_H

#include "latbuilder/Parser/Common.h"
#include "latbuilder/SizeParam.h"

namespace LatBuilder { namespace Parser {

/**
 * Exception thrown when trying to parse an invalid filter.
 */
class BadLevelWeights : public ParserError {
public:
   BadLevelWeights(const std::string& message):
      ParserError("cannot parse level weights: " + message)
   {}
};

/**
 * Parser for per-level weights.
 */
template <LatticeType LR>
struct LevelWeights {
   typedef RealVector result_type;
   /**
    * Parses per-level weights.
    *
    * Example string: <tt>select</tt>, <tt>select:5</tt>, <tt>select:5,12</tt>
    *
    * Empty string is interpreted as <tt>select</tt>.
    */
   static result_type parse(
         const std::string& arg,
         const SizeParam<LR, EmbeddingType::MULTILEVEL>& sizeParam
         );
};

extern template struct LevelWeights<LatticeType::ORDINARY>;
extern template struct LevelWeights<LatticeType::POLYNOMIAL>;

}}

#endif
