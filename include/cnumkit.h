#ifndef CNUMKIT_H
#define CNUMKIT_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * CMake generates this header for configured builds. The source-tree fallback
 * keeps direct compiler builds possible, while the generated version supplies
 * the platform-specific import/export attributes required by shared libraries.
 */
#include "cnumkit_export.h"

#include "cnumkit/vector.h"
#include "cnumkit/matrix.h"
#include "cnumkit/linalg.h"
#include "cnumkit/optim.h"
#include "cnumkit/ml.h"
#include "cnumkit/error.h"
#include "cnumkit/contracts.h"

#ifdef __cplusplus
}
#endif

#endif /* CNUMKIT_H */
