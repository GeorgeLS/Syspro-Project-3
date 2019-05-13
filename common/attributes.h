#ifndef EXERCISE_III_ATTRIBUTES_H
#define EXERCISE_III_ATTRIBUTES_H

#ifdef __GNUC__

#define __NO_RETURN__ __attribute__((noreturn))
#define __NON_NULL__(...) __attribute__((nonnull(__VA_ARGS__)))
#define __FORMAT__(format_type, index, first_to_check) __attribute__((format(format_type, index, first_to_check)))
#define __INLINE__ inline __attribute__((always_inline))
#define __WARN_UNUSED_RESULT__ __attribute__((warn_unused_result))

#else

#define __NO_RETURN__
#define __NON_NULL__(...)
#define __FORMAT__(format_type, index, first_to_check)
#define __INLINE__
#define __WARN_UNUSED_RESULT__

#endif

#endif //EXERCISE_III_ATTRIBUTES_H
