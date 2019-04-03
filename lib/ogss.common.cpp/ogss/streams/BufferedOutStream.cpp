//
// Created by Timm Felden on 25.03.19.
//

#include "BufferedOutStream.h"

using namespace ogss::streams;

void BufferedOutStream::flush() {
    int p = FileOutputStream::BUFFER_SIZE - (current.end - current.begin);
    Buffer put = {current.begin - p, current.end, p};
    completed.push_back(put);
    current.begin = new uint8_t[FileOutputStream::BUFFER_SIZE];
    *const_cast<uint8_t **>(&current.end) = current.begin + FileOutputStream::BUFFER_SIZE;
}