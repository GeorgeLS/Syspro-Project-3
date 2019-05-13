#ifndef EXERCISE_I_TOKENIZER_H
#define EXERCISE_I_TOKENIZER_H

#include <stddef.h>
#include "Common/attributes.h"

typedef struct tokenizer {
    char *stream;
    char delimiter;
} tokenizer;

typedef struct token {
    char *token;
    size_t token_length;
} token;

__NON_NULL__(1)
int tokenizer_next_token(tokenizer *tokenizer, token* token_out);

__NON_NULL__(1)
size_t tokenizer_count_remaining_tokens(tokenizer *tokenizer);

#endif //EXERCISE_I_TOKENIZER_H
