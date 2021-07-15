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

#ifndef LATBUILDER__KERNEL__P_ALPHA_H
#define LATBUILDER__KERNEL__P_ALPHA_H

#include "latbuilder/Kernel/Base.h"
#include "latbuilder/Kernel/FunctorAdaptor.h"
#include "latbuilder/Functor/PAlpha.h"

namespace LatBuilder { namespace Kernel {

class PAlpha : public FunctorAdaptor<Functor::PAlpha> {
public:
   PAlpha(unsigned int alpha):
	  FunctorAdaptor<Functor>(Functor(alpha))
   {}

   unsigned int alpha() const
   { return functor().alpha(); }

   static constexpr Real CUPower = 2;
};

}}

#endif
