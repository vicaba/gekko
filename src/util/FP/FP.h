/**
 * This macro was found on the internet and is used to define closures (GNU only)
 */
#ifndef __FP_H__
#define __FP_H__

/**
 * lambda/closure macro definition
 */
#define lambda(return_type, function_body) \
({ \
      return_type __fn__ function_body \
          __fn__; \
})

#endif
