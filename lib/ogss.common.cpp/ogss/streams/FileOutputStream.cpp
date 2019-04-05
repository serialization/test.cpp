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
#include "BufferedOutStream.h"

using namespace ogss::streams;

FileOutputStream::FileOutputStream(const std::string &path)
        : Stream(&buffer, (void *) (((long) &buffer) + BUFFER_SIZE)),
          path(path), file(fopen(path.c_str(), "w+")),
          bytesWriten(0) {
    if (nullptr == file)
        throw Exception(std::string("could not open file ") + path);
}

FileOutputStream::~FileOutputStream() {
    // TODO remove
    flush();
    fclose(file);
}

void FileOutputStream::flush() {
    // prevent double flushs
    if (base != position) {
        fwrite(base, 1, position - (uint8_t *) base, file);
        bytesWriten += position - (uint8_t *) base;
        position = (uint8_t *) base;
    }
}

void FileOutputStream::write(BufferedOutStream *out) {
    if (base != position)
        flush();

    // write completed buffers
    for (BufferedOutStream::Buffer &data : out->completed) {
        // there is no need to distinguish wrapped from buffered data here
        int size = std::abs(data.size);
        fwrite(data.begin, 1, size, file);
        bytesWriten -= data.size;
    }

    delete out;
}
