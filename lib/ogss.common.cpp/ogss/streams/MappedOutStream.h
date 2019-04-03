//
// Created by Timm Felden on 03.02.16.
//

#ifndef SKILL_CPP_COMMON_MAPPEDOUTSTREAM_H
#define SKILL_CPP_COMMON_MAPPEDOUTSTREAM_H

#include "Stream.h"
#include "../api/String.h"

namespace ogss {
    namespace streams {

        class FileOutputStream;

        /**
         * @author Timm Felden
         */
        struct MappedOutStream : public Stream {
            friend class FileOutputStream;

            MappedOutStream(void *begin, void *end) : Stream(begin, end) {}

            virtual ~MappedOutStream() {}

            /**
             * create a clone of this outstream, that is to be deleted by the caller
             * and that will have virtually ranges between begin and end.
             * @note DO NOT unmap result
             */
            MappedOutStream *clone(size_t begin, size_t end) const {
                return new MappedOutStream(position + begin, position + end);
            }

            inline void boolean(bool v) {
                *(position++) = v ? 0xff : 0;
            }

            inline void i8(int8_t v) {
                *(position++) = (uint8_t) v;
            }

            inline void i16(int16_t v) {
                *(position++) = (uint8_t) (v >> 8);
                *(position++) = (uint8_t) v;
            }

            inline void i32(int32_t v) {
                *(position++) = (uint8_t) (v >> 24);
                *(position++) = (uint8_t) (v >> 16);
                *(position++) = (uint8_t) (v >> 8);
                *(position++) = (uint8_t) v;
            }

            inline void i64(int64_t v) {
                *(position++) = (uint8_t) (v >> 56);
                *(position++) = (uint8_t) (v >> 48);
                *(position++) = (uint8_t) (v >> 40);
                *(position++) = (uint8_t) (v >> 32);
                *(position++) = (uint8_t) (v >> 24);
                *(position++) = (uint8_t) (v >> 16);
                *(position++) = (uint8_t) (v >> 8);
                *(position++) = (uint8_t) v;
            }

        private:
            void largeV64(uint64_t v) {
                *(position++) = (uint8_t) ((0x80U | v >> 7u));
                if (v < 0x200000U) {
                    *(position++) = (uint8_t) ((v >> 14u));
                } else {
                    *(position++) = (uint8_t) ((0x80U | v >> 14u));
                    if (v < 0x10000000U) {
                        *(position++) = (uint8_t) ((v >> 21u));
                    } else {
                        *(position++) = (uint8_t) ((0x80U | v >> 21u));
                        if (v < 0x800000000U) {
                            *(position++) = (uint8_t) ((v >> 28u));
                        } else {
                            *(position++) = (uint8_t) ((0x80U | v >> 28u));
                            if (v < 0x40000000000U) {
                                *(position++) = (uint8_t) ((v >> 35u));
                            } else {
                                *(position++) = (uint8_t) ((0x80U | v >> 35u));
                                if (v < 0x2000000000000U) {
                                    *(position++) = (uint8_t) ((v >> 42u));
                                } else {
                                    *(position++) = (uint8_t) ((0x80U | v >> 42u));
                                    if (v < 0x100000000000000U) {
                                        *(position++) = (uint8_t) ((v >> 49u));
                                    } else {
                                        *(position++) = (uint8_t) ((0x80U | v >> 49u));
                                        *(position++) = (uint8_t) ((v >> 56u));
                                    }
                                }
                            }
                        }
                    }
                }
            }

        public:

            inline void v64(int64_t p) {
                auto v = ::ogss::unsign(p);

                if (v < 0x80U) {
                    *(position++) = (uint8_t) (v);
                } else {
                    *(position++) = (uint8_t) ((0x80U | v));
                    if (v < 0x4000U) {
                        *(position++) = (uint8_t) ((v >> 7u));
                    } else {
                        largeV64(v);
                    }
                }
            }


            inline void f32(float v) {
                register union {
                    uint32_t i;
                    float f;
                } tmp;
                tmp.f = v;
                i32(tmp.i);
            }

            inline void f64(double v) {
                register union {
                    uint64_t i;
                    double f;
                } tmp;
                tmp.f = v;
                i64(tmp.i);
            }
        };
    }
}

#endif //SKILL_CPP_COMMON_MAPPEDOUTSTREAM_H
