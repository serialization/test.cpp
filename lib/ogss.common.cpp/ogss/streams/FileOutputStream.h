//
// Created by Timm Felden on 03.02.16.
//

#ifndef SKILL_CPP_COMMON_FILEOUTPUTSTREAM_H
#define SKILL_CPP_COMMON_FILEOUTPUTSTREAM_H

#include <string>
#include <assert.h>
#include "Stream.h"
#include "../api/String.h"

namespace ogss {
    namespace streams {

        class MappedOutStream;
        class BufferedOutStream;

        /**
         * File out streams manages file; uses a buffer for its write operations.
         * Can create a map of correct size for mapped streams.
         *
         * @author Timm Felden
         */
        class FileOutputStream : public Stream {
            friend class BufferedOutStream;

            /**
             * the path where this stream was opened from
             */
            const std::string path;

            /**
             * the file object used for communication to the fs
             */
            FILE *const file;

            //! number of bytes written to the file; used for truncation on close
            size_t bytesWriten;

            /**
             * flush the buffer
             */
            void flush();

            inline void require(size_t i) {
                if (!has(i))
                    flush();
            }

            //! the backing buffer (allocate plain to get rid of one pointer deref)
            uint8_t buffer[BUFFER_SIZE];

        public:

            /**
             * open the file at the target location
             *
             * @param append set to true, if content shall be appended to the file
             */
            FileOutputStream(const std::string &path, bool append);

            /**
             * close the stream
             */
            virtual ~FileOutputStream();

            /**
             * total file size
             */
            size_t fileSize() const noexcept {
                return bytesWriten;
            }

            const std::string &filePath() const noexcept {
                return path;
            }

            /**
            * Maps from current position until offset.
            *
            * @return a buffer that has exactly offset many bytes remaining
            */
            MappedOutStream *jumpAndMap(long offset);

            /**
             * Delete a mapping.
             * This method has to be called for the result of jump and map eventually.
             * At that point, no clone of the argument stream must be in use.
             * @note do NOT call this for clones!
             * @note the argument map will be deleted!
             * @note clones must be deleted by the cloner
             */
            void unmap(MappedOutStream *map);

            inline void i8(int8_t v) {
                require(1);
                *(position++) = (uint8_t) v;
            }

            inline void i16(int16_t v) {
                require(2);
                *(position++) = (uint8_t) (v >> 8);
                *(position++) = (uint8_t) v;
            }

            inline void i32(int32_t v) {
                require(4);
                *(position++) = (uint8_t) (v >> 24);
                *(position++) = (uint8_t) (v >> 16);
                *(position++) = (uint8_t) (v >> 8);
                *(position++) = (uint8_t) v;
            }

            inline void i64(int64_t p) {
                auto v = ::ogss::unsign(p);
                require(8);
                *(position++) = (uint8_t) (v >> 56u);
                *(position++) = (uint8_t) (v >> 48u);
                *(position++) = (uint8_t) (v >> 40u);
                *(position++) = (uint8_t) (v >> 32u);
                *(position++) = (uint8_t) (v >> 24u);
                *(position++) = (uint8_t) (v >> 16u);
                *(position++) = (uint8_t) (v >> 8u);
                *(position++) = (uint8_t) v;
            }

            inline void v64(int64_t p) {
                auto v = ::ogss::unsign(p);
                require(9);

                if (v < 0x80U) {
                    *(position++) = (uint8_t) (v);
                } else {
                    *(position++) = (uint8_t) ((0x80U | v));
                    if (v < 0x4000U) {
                        *(position++) = (uint8_t) ((v >> 7u));
                    } else {
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
                }
            }

            inline void put(const api::String s) {
                const auto size = s->size();
                if (size >= BUFFER_SIZE) {
                    flush();
                    fwrite(s->c_str(), 1, size, file);
                    bytesWriten += size;
                } else {
                    require(size);
                    for (uint8_t c : *s)
                        *(position++) = c;
                }
            }
        };
    }
}

#endif //SKILL_CPP_COMMON_FILEOUTPUTSTREAM_H
