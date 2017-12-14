#ifndef HL_PYTORCH_WRAPPER_H
#define HL_PYTORCH_WRAPPER_H

#include "HalideBuffer.h"
#include "HalideRuntimeCuda.h"
#include <vector>

#include "TH/TH.h"
#include "THC/THC.h"

extern THCState *state;

using Halide::Runtime::Buffer;

namespace Halide {
namespace Pytorch {

inline Buffer<float> wrap(THFloatTensor* tensor) {
  int ndims = THFloatTensor_nDimension(tensor);
  std::vector<int> dims(ndims, 0);
  for(int dim = 0; dim < ndims; ++dim) {
    dims[dim] = THFloatTensor_size(tensor, ndims-1-dim);
  }
  float* pData  = THFloatTensor_data(tensor);
  Buffer<float> buffer(pData, dims);
  return buffer;
}

inline Buffer<float> wrap(THCudaTensor* tensor) {
  const halide_device_interface_t* cuda_interface = halide_cuda_device_interface();

  int ndims = THCudaTensor_nDimension(state, tensor);
  std::vector<int> dims(ndims, 0);
  for(int dim = 0; dim < ndims; ++dim) {
    dims[dim] = THCudaTensor_size(state, tensor, ndims-1-dim);
  }

  Buffer<float> buffer(dims);

  float* pData  = THCudaTensor_data(state, tensor);
  int err = buffer.device_wrap_native(cuda_interface, (uint64_t)pData);
  assert(err == 0);
  buffer.set_device_dirty();

  return buffer;
}

}
}

#endif  // HL_PYTORCH_WRAPPER_H