/*
 * Copyright (C) 2024 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stddef.h>

extern void *sk_new_null(void);
void *OPENSSL_sk_new_null(void) {
    return sk_new_null();
}

extern size_t sk_num(void *sk);
size_t OPENSSL_sk_num(void *sk) {
    return sk_num(sk);
}

extern size_t sk_push(void *sk, void *p);
size_t OPENSSL_sk_push(void *sk, void *p) {
    return sk_push(sk, p);
}

extern void *sk_value(const void *sk, size_t i);
void* OPENSSL_sk_value(const void *sk, size_t i) {
    return sk_value(sk, i);
}
