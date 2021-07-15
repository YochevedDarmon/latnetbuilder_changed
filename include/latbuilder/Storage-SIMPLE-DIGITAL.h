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

#ifndef LATBUILDER__STORAGE_SIMPLE_DIGITAL_H
#define LATBUILDER__STORAGE_SIMPLE_DIGITAL_H

#include "latbuilder/Storage.h"
#include "latbuilder/CompressTraits.h"
#include "latbuilder/SizeParam.h"
#include "latbuilder/Types.h"
#include "latbuilder/Util.h"

namespace LatBuilder {


template <Compress COMPRESS>
struct StorageTraits<Storage<LatticeType::DIGITAL, EmbeddingType::UNILEVEL, COMPRESS>> {
   typedef uInteger            size_type;
   typedef typename LatticeTraits<LatticeType::DIGITAL>::GenValue  value_type;
   typedef CompressTraits<COMPRESS> Compress;
   typedef Real               MeritValue;
   typedef LatBuilder::SizeParam<LatticeType::DIGITAL, EmbeddingType::UNILEVEL> SizeParam;

   /**
    * Unpermuted permutation.
    */
   class Unpermute {
   public:
      typedef StorageTraits::size_type size_type;

      Unpermute(Storage<LatticeType::DIGITAL, EmbeddingType::UNILEVEL, COMPRESS> storage): m_storage(std::move(storage)) {}
      size_type operator() (size_type i) const { return Compress::compressIndex(i, m_storage.virtualSize()); }
      size_type size() const { return m_storage.virtualSize(); }
   private:
      Storage<LatticeType::DIGITAL, EmbeddingType::UNILEVEL, COMPRESS> m_storage;
   };

   /**
    * Stride permutation.
    *
    * For digital nets: 
    * a Stride with takes as parameter a generating matrix \f$M\f$ is the mapping that maps an index \f$i\f$ as follows:
    * - convert the binary representation of \f$i\f$ to its associated Gray code \f$\boldsymbol i'\f$
    * - compute \f$M \boldsymbol i'\f$ which is the reversed binary representation of the result integer
    * 
    * Since the integers are first converted to Gray code, the computation of \f$M \boldsymbol i'\f$ can be fastened as explained in
    * these <a href="http://web.maths.unsw.edu.au/~fkuo/sobol/joe-kuo-notes.pdf">notes by Joe and Kuo on the generation of Sobol' sequences</a>.
    */
   class Stride {
   public:
      typedef StorageTraits::size_type size_type;
      typedef StorageTraits::value_type value_type;

      Stride(Storage<LatticeType::DIGITAL, EmbeddingType::UNILEVEL, COMPRESS> storage, value_type stride):
         m_storage(std::move(storage)),
         m_stride(stride),
         m_permutation(m_storage.virtualSize(), 0)
      {
        std::vector<unsigned long> cols = m_stride.getColsReverse();
        for(size_type i=m_graycode.size(); i < m_permutation.size(); i++){
          m_graycode.push_back(getmsb(((i >> 1) ^ i)^(((i+1) >> 1) ^ (i+1))));
        }
        for (size_type i=1; i<m_permutation.size(); ++i){
          m_permutation[i] = m_permutation[i-1] ^ cols[m_graycode[i-1]];
        }
      }

      size_type operator() (size_type i) const
      {
         return Compress::compressIndex(m_permutation[i], m_storage.virtualSize());
      }

      size_type size() const
      { return m_storage.size(); }

   private:
      Storage<LatticeType::DIGITAL, EmbeddingType::UNILEVEL, COMPRESS> m_storage;
      value_type m_stride;
      std::vector<size_type> m_permutation;

      static std::vector<size_type> m_graycode;

      static unsigned int
      getmsb (unsigned long long x)
      {
          unsigned int r = 0;
          if (x < 1) return 0;
          while (x >>= 1) r++;
          return r;
      }
   };

};

// template

template <Compress COMPRESS> std::vector<uInteger> StorageTraits<Storage<LatticeType::DIGITAL, EmbeddingType::UNILEVEL, COMPRESS>>::Stride::m_graycode = std::vector<uInteger>();
// StorageTraits< = std::vector<size_type>()

// template<> std::vector<uInteger> StorageTraits<Storage<LatticeType::DIGITAL, EmbeddingType::UNILEVEL, Compress::NONE>>::Stride::m_graycode = std::vector<uInteger>();
// template<> std::vector<uInteger> StorageTraits<Storage<LatticeType::DIGITAL, EmbeddingType::UNILEVEL, Compress::SYMMETRIC>>::Stride::m_graycode = std::vector<uInteger>();

}

#endif
