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

#include "latbuilder/GenSeq/CyclicGroup.h"
#include "latbuilder/TextStream.h"

#include "Path.h"

#include <iostream>

using namespace LatBuilder;
using TextStream::operator<<;

//! [main]
typedef GenSeq::CyclicGroup<LatticeType::ORDINARY, Compress::NONE> WholeIntSeq;
typedef GenSeq::CyclicGroup<LatticeType::ORDINARY, Compress::SYMMETRIC> HalfIntSeq;
typedef GenSeq::CyclicGroup<LatticeType::POLYNOMIAL, Compress::NONE> WholePolySeq;

void displayIntSeq(int base, int power)
{
   std::cout << "lattice size: " << base << "^" << power << std::endl;
   std::cout << "    whole sequence: " << WholeIntSeq(base, power) << std::endl;
   std::cout << "     half sequence: " << HalfIntSeq(base, power) << std::endl;
}

void displayPolySeq(Polynomial base, int power)
{
   std::cout << "polynomial lattice size: " << base << "^" << power << std::endl;
   std::cout << "    whole sequence: " << WholePolySeq(base, power) << std::endl;
}

int main()
{
   SET_PATH_TO_LATNETBUILDER_FOR_EXAMPLES();
   displayIntSeq(7, 1);
   displayIntSeq(2, 3);
   displayIntSeq(3, 2);
   displayPolySeq(PolynomialFromInt(13),1); 

   return 0;
}
//! [main]
