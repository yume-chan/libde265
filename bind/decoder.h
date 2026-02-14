#pragma once

#include <string>

#include "image.h"

struct DecodingResult
{
    de265_error error;
    bool more;
};

class Decoder
{
private:
    de265_decoder_context *_context;

public:
    Decoder();

    ~Decoder();

#ifdef HAS_PTHREAD
    de265_error start_worker_threads(int threads);
#endif

    de265_error push_data(std::string input, de265_PTS pts);
    void push_end_of_nal();
    void push_end_of_frame();
    de265_error push_nal(std::string input, de265_PTS pts);
    de265_error flush_data();

    int get_number_of_input_bytes_pending() const;
    int get_number_of_nal_units_pending() const;

    DecodingResult decode();

    void reset();

    std::optional<Image> peek_next_picture();
    std::optional<Image> get_next_picture();

    de265_error get_warning() const;

    int get_highest_tid() const;
    int get_current_tid() const;
    void set_limit_tid(int max_tid);

    void set_framerate_ratio(int percent);
    int change_framerate(int more_vs_less);

    void set_parameter_bool(enum de265_param param, bool value);
    bool get_parameter_bool(enum de265_param param);
};
