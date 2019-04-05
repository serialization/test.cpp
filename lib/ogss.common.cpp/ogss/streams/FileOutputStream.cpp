//
// Created by Timm Felden on 03.02.16.
//

#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include "FileOutputStream.h"
#include "MappedOutStream.h"
#include "../api/Exception.h"

ogss::streams::FileOutputStream::FileOutputStream(const std::string &path)
        : Stream(&buffer, (void *) (((long) &buffer) + BUFFER_SIZE)),
          path(path), file(fopen(path.c_str(), "w+")),
          bytesWriten(0) {
    if (nullptr == file)
        throw Exception(std::string("could not open file ") + path);
}

ogss::streams::FileOutputStream::~FileOutputStream() {
    // TODO remove
    flush();
    fclose(file);
}

void ogss::streams::FileOutputStream::flush() {
    // prevent double flushs
    if (base != position) {
        fwrite(base, 1, position - (uint8_t *) base, file);
        bytesWriten += position - (uint8_t *) base;
        position = (uint8_t *) base;
    }
}

ogss::streams::MappedOutStream *ogss::streams::FileOutputStream::jumpAndMap(long length) {
    flush();

    // advance file position
    fseek(file, length, SEEK_CUR);
    bytesWriten += length;

    // create map
    void *rval = mmap(NULL, bytesWriten, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(file), 0);

    if (MAP_FAILED == rval) {
        fprintf(stderr, "mmap.c: %s\n errno was: %s\n", "failed to create write map", strerror(errno));
        return NULL;
    }

    if (-1 == posix_madvise(rval, length, MADV_SEQUENTIAL | MADV_WILLNEED)) {
        fprintf(stderr, "mmap.c: %s\n errno was: %s\n", "failed to advise write map", strerror(errno));
        return NULL;
    }

    // resize file
    if (ftruncate(fileno(file), bytesWriten)) {
        fprintf(stderr, "mmap.c: %s\n errno was: %s\n", "failed to resize file", strerror(errno));
        return NULL;
    }

    auto map = new MappedOutStream(rval, (void *) (((long) rval) + bytesWriten));
    map->position = &((uint8_t *) rval)[bytesWriten - length];
    return map;
}

void ogss::streams::FileOutputStream::unmap(ogss::streams::MappedOutStream *map) {
    msync(map->base, (size_t) map->end - (size_t) map->base, MS_ASYNC);
    munmap(map->base, (size_t) map->end - (size_t) map->base);
    delete map;
}
