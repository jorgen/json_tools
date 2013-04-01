/*
 * Copyright © 2013 Jorgen Lind
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#ifdef NDEBUG
#error "These tests uses assert. Please remove define NDEBUG"
#endif

#include <string.h>

#include "json_tree.h"

#include "json-test-data.h"

static int check_json_tree_printer()
{
    JT::TreeBuilder tree_builder;
    auto created = tree_builder.build(json_data2,sizeof(json_data2));
    JT::Node *root = created.first;
    assert(root);

    check_json_tree_from_json_data2(root);

    JT::SerializerOptions printerOption(false);
    char buffer[4096];
    memset(buffer,'\0', 4096);
    JT::TreeSerializer serializer(buffer,4096);
    assert(serializer.serialize(root->asObjectNode()));

    size_t actual_size = strlen(buffer);
    size_t reported_used = serializer.buffers().front().used;

    assert(actual_size == reported_used);

    delete root;

    created = tree_builder.build(buffer,actual_size);

    root = created.first;
    assert(root);
    check_json_tree_from_json_data2(root);

    return 0;
}

static int check_json_tree_printer_pretty()
{
    JT::TreeBuilder tree_builder;
    auto created = tree_builder.build(json_data2,sizeof(json_data2));
    JT::Node *root = created.first;
    assert(root);

    check_json_tree_from_json_data2(root);

    JT::SerializerOptions printerOption(true);
    char buffer[4096];
    memset(buffer,'\0', 4096);
    JT::TreeSerializer serializer(buffer,4096);
    assert(serializer.serialize(root->asObjectNode()));

    size_t actual_size = strlen(buffer);
    size_t reported_used = serializer.buffers().front().used;

    assert(actual_size == reported_used);

    delete root;

    created = tree_builder.build(buffer,actual_size);
    root = created.first;
    check_json_tree_from_json_data2(root);

    return 0;
}

static int check_multiple_print_buffers()
{
    JT::TreeBuilder tree_builder;
    auto created = tree_builder.build(json_data2,sizeof(json_data2));
    JT::Node *root = created.first;
    assert(root);

    size_t printed_size = sizeof(json_data2);

    JT::TreeSerializer serializer;
    serializer.setSerializerOptions(JT::SerializerOptions(true));

    char buffer1[printed_size/2];
    serializer.appendBuffer(buffer1, printed_size/2);
    char buffer2[2];
    serializer.appendBuffer(buffer2,2);
    char buffer3[printed_size];
    serializer.appendBuffer(buffer3,printed_size);

    assert(serializer.serialize(root->asObjectNode()));

    size_t complete_size = 0;
    char target_buffer[4096];
    memset(target_buffer,'\0', 4096);
    auto buffers = serializer.buffers();
    for (auto it = buffers.begin(); it != buffers.end(); ++it) {
        if ((*it).used > 0) {
            memcpy(target_buffer + complete_size, (*it).buffer, (*it).used);
            complete_size += (*it).used;
        }
        assert(complete_size <= printed_size);
    }

    assert(complete_size == printed_size);

    char valid_buffer[4096];
    memset(valid_buffer,'\0', 4096);
    serializer = JT::TreeSerializer();
    serializer.setSerializerOptions(JT::SerializerOptions(true));
    serializer.appendBuffer(valid_buffer,4096);

    serializer.serialize(root->asObjectNode());

    assert(memcmp(valid_buffer, target_buffer, printed_size) == 0);

    return 0;
}

static void add_buffer_func(JT::Serializer *printHandler)
{
    char *buffer = new char[4096];
    printHandler->appendBuffer(buffer,4096);
}

static int check_callback_print_buffers()
{
    JT::TreeBuilder tree_builder;
    auto created = tree_builder.build(json_data2,sizeof(json_data2));
    JT::Node *root = created.first;
    assert(root);

    JT::TreeSerializer serializer;
    serializer.setSerializerOptions(JT::SerializerOptions(true));
    serializer.addRequestBufferCallback(add_buffer_func);

    assert(serializer.serialize(root->asObjectNode()));

    size_t complete_size = 0;
    char target_buffer[4096];
    memset(target_buffer,'\0', 4096);
    auto buffers = serializer.buffers();
    for (auto it = buffers.begin(); it != buffers.end(); ++it) {
        if ((*it).used > 0) {
            memcpy(target_buffer + complete_size, (*it).buffer, (*it).used);
            complete_size += (*it).used;
            delete[] (*it).buffer;
        }
    }

    char valid_buffer[4096];
    memset(valid_buffer,'\0', 4096);
    serializer = JT::TreeSerializer();
    serializer.setSerializerOptions(JT::SerializerOptions(true));
    serializer.appendBuffer(valid_buffer,4096);

    serializer.serialize(root->asObjectNode());

    assert(memcmp(valid_buffer, target_buffer, 4096) == 0);

    return 0;
}

int main(int,char **)
{
    check_json_tree_printer();
    check_json_tree_printer_pretty();
    check_multiple_print_buffers();
    check_callback_print_buffers();
    return 0;
}
