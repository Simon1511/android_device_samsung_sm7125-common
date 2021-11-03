#ifndef INIT_SEC_H
#define INIT_SEC_H

#include <string.h>

enum device_variant {
    VARIANT_A525F = 0,
    VARIANT_A525M,
    VARIANT_MAX
};

typedef struct {
    std::string model;
    std::string codename;
} variant;

static const variant international_models = {
    .model = "SM-A525F",
    .codename = "a52q"
};

static const variant america_models = {
    .model = "SM-A525M",
    .codename = "a52q"
};

static const variant *all_variants[VARIANT_MAX] = {
    &international_models,
    &america_models
};

#endif // INIT_SEC_H
