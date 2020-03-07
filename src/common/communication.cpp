#include "communication.h"

intptr_t send_event(boost::asio::local::stream_protocol::socket& socket,
                    int32_t opcode,
                    int32_t index,
                    intptr_t value,
                    void* data,
                    float option) {
    auto payload =
        data == nullptr
            ? std::nullopt
            : std::make_optional(std::string(static_cast<char*>(data)));

    const Event event{opcode, index, value, option, payload};
    write_object(socket, event);

    const auto response = read_object<EventResult>(socket);
    if (response.data.has_value()) {
        char* char_data = static_cast<char*>(data);

        // For correctness we will copy the entire buffer and add a terminating
        // null byte ourselves. In practice `response.data` will only ever
        // contain C-style strings, but this would work with any other data
        // format that can contain null bytes.
        std::copy(response.data->begin(), response.data->end(), char_data);
        char_data[response.data->size()] = 0;
    }

    return response.return_value;
}