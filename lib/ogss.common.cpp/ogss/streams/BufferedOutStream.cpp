//
// Created by Timm Felden on 25.03.19.
//

#include "BufferedOutStream.h"

using namespace ogss::streams;

void BufferedOutStream::flush() {
    if (current.size) {
        int p = FileOutputStream::BUFFER_SIZE - (current.end - current.begin);
        completed.emplace_back(Buffer({current.begin - p, current.end, p}));
    }
    current.begin = new uint8_t[FileOutputStream::BUFFER_SIZE];
    *const_cast<uint8_t **>(&current.end) = current.begin + FileOutputStream::BUFFER_SIZE;
}