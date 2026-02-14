#pragma once

#include <string>
#include <vector>

#include <libde265/en265.h>

#include "image.h"
#include "packet.h"

EMSCRIPTEN_DECLARE_VAL_TYPE(StringArray);

class Encoder
{
private:
    en265_encoder_context *_context;

public:
    Encoder();

    ~Encoder();

    StringArray list_parameters();
    enum en265_parameter_type get_parameter_type(std::string name);
    StringArray list_parameter_choices(std::string name);

    de265_error set_parameter_bool(std::string name, bool value);
    de265_error set_parameter_int(std::string name, int value);
    de265_error set_parameter_string(std::string name, std::string value);
    de265_error set_parameter_choice(std::string name, std::string value);

    de265_error start(int threads);

    Image allocate_image(int width, int height, enum de265_chroma chroma, de265_PTS pts);
    de265_error push_image(Image &image);
    de265_error push_eof();

    int get_current_input_queue_length() const;

    de265_error encode();

    enum en265_encoder_state get_state() const;

    std::optional<Packet> get_packet(int timeout_ms);
    int get_number_of_queued_packets() const;
};
