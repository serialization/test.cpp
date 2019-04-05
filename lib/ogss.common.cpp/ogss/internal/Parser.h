//
// Created by Timm Felden on 03.04.19.
//

#ifndef OGSS_TEST_CPP_PARSER_H
#define OGSS_TEST_CPP_PARSER_H

#include "PoolBuilder.h"
#include "StateInitializer.h"
#include "../streams/InStream.h"

namespace ogss {
    namespace internal {

        /**
         * Files smaller than this size are passed to SeqParser.
         */
        const size_t SEQ_PARSER_LIMIT = 512000;

        /**
         * @author Timm Felden
         */
        class Parser : public StateInitializer {

            void parseFile(FileInputStream *in);

            /**
             * Turns a field type into a preliminary type information. In case of user types, the declaration of the respective
             * user type may follow after the field declaration.
             */
            FieldType *fieldType();

        protected:
            const PoolBuilder &pb;

            /**
             * This buffer provides the association of file fieldID to field.
             */
            std::vector<RTTIBase *> fields;

            /**
             * File defined types. This array is used to resolve type IDs while parsing. The type IDs assigned to created
             * entities may not correspond to fdts indices (shifted by 10).
             */
            std::vector<FieldType *> fdts;

            Parser(const std::string &path, streams::FileInputStream *in, const PoolBuilder &pb);

            static void ParseException(streams::InStream *in, const std::string &msg);

            virtual void typeBlock() = 0;

            std::unordered_set<TypeRestriction *> *typeRestrictions(int count);

            /**
             * Parse type definitions and merge them into the known type hierarchy
             */
            void typeDefinitions();

            void TContainer();

            void TEnum();

            void readFields(AbstractPool *p);

            virtual void processData() = 0;
        };
    }
}

#endif //OGSS_TEST_CPP_PARSER_H
