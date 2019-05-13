#include "tokenizer.h"
#include <ctype.h>
#include <string.h>

int tokenizer_next_token(tokenizer *tokenizer, token *token_out) {
    char *stream = tokenizer->stream;
    if (!*stream) return 0;
    if (token_out != NULL) {
        token_out->token = stream;
        token_out->token_length = strlen(stream);
    }
    const char delimiter = tokenizer->delimiter;
    while (*stream != delimiter && *stream != '\0') ++stream;
    *stream = '\0';
    tokenizer->stream = ++stream;
    return 1;
}

size_t tokenizer_count_remaining_tokens(tokenizer *tokenizer) {
    char *stream = tokenizer->stream;
    if (!*stream) return 0;
    size_t tokens = 0U;
    const char delimiter = tokenizer->delimiter;
    while (*stream) if (*stream++ == delimiter) ++tokens;
    return ++tokens;
}