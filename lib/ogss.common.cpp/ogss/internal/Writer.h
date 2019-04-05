//
// Created by Timm Felden on 05.04.19.
//

#ifndef OGSS_TEST_CPP_WRITER_H
#define OGSS_TEST_CPP_WRITER_H

namespace ogss {
    namespace api {
        class File;
    }

    namespace internal {

        /**
         * Write a File to disk.
         *
         * @author Timm Felden
         */
        class Writer final {
            api::File *const state;

            Writer(api::File *state, streams::FileOutputStream &out);

            friend class api::File;
        };
    }
}

#endif //OGSS_TEST_CPP_WRITER_H
