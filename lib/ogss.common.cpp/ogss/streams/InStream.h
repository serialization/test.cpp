//
// Created by feldentm on 03.11.15.
//

#ifndef SKILL_CPP_COMMON_INSTREAM_H
#define SKILL_CPP_COMMON_INSTREAM_H

#include <string>
#include "Stream.h"
#include "../api/String.h"
#include "../api/Box.h"
#include "../api/Exception.h"

namespace ogss {
    namespace streams {

        using ogss::api::String;

        /**
        * Abstract input streams.
        *
        * @author Timm Felden
        */
        class InStream : public Stream {
            static inline void ensure(const bool condition) {
                if (!condition)
                    throw Exception("unexpected end of stream");
            }

        protected:

            InStream(void *base, void *end) : Stream(base, end) {}

        public:

            /**
             * Proper destruction happens in child destructors
             */
            virtual ~InStream() {}

            inline int8_t i8() {
                ensure(position < end);
                return *(position++);
            }

            static inline api::Box i8Box(InStream &self) {
                api::Box r = {0};
                r.i8 = self.i8();
                return r;
            }

            inline int16_t i16() {
                ensure(position + 1 < end);
                uint16_t r = *(position++) << 8;
                r |= *(position++);
                return r;
            }

            static inline api::Box i16Box(InStream &self) {
                api::Box r = {0};
                r.i16 = self.i16();
                return r;
            }

            inline int32_t i32() {
                ensure(position + 3 < end);
                uint32_t r = *(position++) << 24;
                r |= *(position++) << 16;
                r |= *(position++) << 8;
                r |= *(position++);
                return r;
            }

            static inline api::Box i32Box(InStream &self) {
                api::Box r = {0};
                r.i32 = self.i32();
                return r;
            }

            inline int64_t i64() {
                ensure(position + 7 < end);
                uint64_t r = 0;
                r |= ((uint64_t) *(position++)) << 56;
                r |= ((uint64_t) *(position++)) << 48;
                r |= ((uint64_t) *(position++)) << 40;
                r |= ((uint64_t) *(position++)) << 32;
                r |= ((uint64_t) *(position++)) << 24;
                r |= ((uint64_t) *(position++)) << 16;
                r |= ((uint64_t) *(position++)) << 8;
                r |= ((uint64_t) *(position++));
                return r;
            }

            static inline api::Box i64Box(InStream &self) {
                api::Box r = {0};
                r.i64 = self.i64();
                return r;
            }

            inline int32_t v32() {
                uint32_t v;

                if (position > end) goto FAILURE;
                if ((v = *(position++)) >= 0x80U) {
                    if (position > end) goto FAILURE;
                    v = (v & 0x7fU) | ((uint32_t) *(position++) << 7U);

                    if (v >= 0x4000U) {
                        if (position > end) goto FAILURE;
                        v = (v & 0x3fffU) | ((uint32_t) *(position++) << 14U);

                        if (v >= 0x200000U) {
                            if (position > end) goto FAILURE;
                            v = (v & 0x1fffffU) | ((uint32_t) *(position++) << 21U);

                            if (v >= 0x10000000U) {
                                if (position > end) goto FAILURE;
                                v = (v & 0xfffffffU) | ((uint32_t) *(position) << 28U);
                                if (*position > 0xf)
                                    throw std::logic_error("value not v32");
                            }
                        }
                    }
                }
                return v;

                FAILURE:
                throw Exception("unexpected end of stream");
            }

            inline int64_t v64() {
                uint64_t v;

                if (position > end) goto FAILURE;
                if ((v = *(position++)) >= 0x80U) {
                    if (position > end) goto FAILURE;
                    v = (v & 0x7fU) | ((uint64_t) *(position++) << 7U);

                    if (v >= 0x4000U) {
                        if (position > end) goto FAILURE;
                        v = (v & 0x3fffU) | ((uint64_t) *(position++) << 14U);

                        if (v >= 0x200000U) {
                            if (position > end) goto FAILURE;
                            v = (v & 0x1fffffU) | ((uint64_t) *(position++) << 21U);

                            if (v >= 0x10000000U) {
                                if (position > end) goto FAILURE;
                                v = (v & 0xfffffffU) | ((uint64_t) *(position++) << 28U);

                                if (v >= 0x800000000U) {
                                    if (position > end) goto FAILURE;
                                    v = (v & 0x7ffffffffU) | ((uint64_t) *(position++) << 35U);

                                    if (v >= 0x40000000000U) {
                                        if (position > end) goto FAILURE;
                                        v = (v & 0x3ffffffffffU) | ((uint64_t) *(position++) << 42U);

                                        if (v >= 0x2000000000000U) {
                                            if (position > end) goto FAILURE;
                                            v = (v & 0x1ffffffffffffU) | ((uint64_t) *(position++) << 49U);

                                            if (v >= 0x100000000000000U) {
                                                if (position > end) goto FAILURE;
                                                v = (v & 0xffffffffffffffU) | ((uint64_t) *(position++) << 56U);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                return v;

                FAILURE:
                throw Exception("unexpected end of stream");
            }

            //! checked version of v64 that will not throw an exception
            //! @note checked means checked by the caller!
            inline int64_t v64checked() noexcept {
                uint64_t v;

                if ((v = *(position++)) >= 0x80U) {
                    v = (v & 0x7fU) | ((uint64_t) *(position++) << 7U);

                    if (v >= 0x4000U) {
                        v = (v & 0x3fffU) | ((uint64_t) *(position++) << 14U);

                        if (v >= 0x200000U) {
                            v = (v & 0x1fffffU) | ((uint64_t) *(position++) << 21U);

                            if (v >= 0x10000000U) {
                                v = (v & 0xfffffffU) | ((uint64_t) *(position++) << 28U);

                                if (v >= 0x800000000U) {
                                    v = (v & 0x7ffffffffU) | ((uint64_t) *(position++) << 35U);

                                    if (v >= 0x40000000000U) {
                                        v = (v & 0x3ffffffffffU) | ((uint64_t) *(position++) << 42U);

                                        if (v >= 0x2000000000000U) {
                                            v = (v & 0x1ffffffffffffU) | ((uint64_t) *(position++) << 49U);

                                            if (v >= 0x100000000000000U) {
                                                v = (v & 0xffffffffffffffU) | ((uint64_t) *(position++) << 56U);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                return v;
            }

            static inline api::Box v64Box(InStream &self) {
                api::Box r = {0};
                r.i64 = self.v64();
                return r;
            }

            inline float f32() {
                union {
                    int32_t i;
                    float f;
                } result = {0};
                result.i = i32();
                return result.f;
            }

            inline double f64() {
                union {
                    int64_t i;
                    double f;
                } result = {0};
                result.i = i64();
                return result.f;
            }


            inline bool boolean() {
                return *(position++) != 0;
            }

            static inline api::Box boolBox(InStream &self) {
                api::Box r = {0};
                r.boolean = self.boolean();
                return r;
            }

            /**
             * create a string from the stream
             * @note the caller owns the string!
             * @note does not move position
             * @note the offset is absolute and can be before position
             */
            String string(uint32_t offset, uint32_t length, ObjectID id) {
                ensure((bool) (id > 0)
                       & (bool) (offset > 0)
                       & (bool) ((uint8_t *) base + offset + length <= (uint8_t *) end));

                String rval = new std::string((const char *) base + offset, length);
                return rval;
            }
        };
    }
}
#endif //SKILL_CPP_COMMON_INSTREAM_H
