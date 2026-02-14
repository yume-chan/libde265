#include "decoder.h"

using namespace emscripten;

Decoder::Decoder()
{
    _context = de265_new_decoder();
}

Decoder::~Decoder()
{
    de265_free_decoder(_context);
}

#ifdef HAS_PTHREAD
de265_error Decoder::start_worker_threads(int threads)
{
    return de265_start_worker_threads(_context, threads);
}
#endif

de265_error Decoder::push_data(std::string input, de265_PTS pts)
{
    return de265_push_data(_context, input.data(), input.length(), pts, nullptr);
}

void Decoder::push_end_of_nal()
{
    de265_push_end_of_NAL(_context);
}

void Decoder::push_end_of_frame()
{
    de265_push_end_of_frame(_context);
}

de265_error Decoder::push_nal(std::string input, de265_PTS pts)
{
    return de265_push_NAL(_context, input.data(), input.length(), pts, nullptr);
}

de265_error Decoder::flush_data()
{
    return de265_flush_data(_context);
}

int Decoder::get_number_of_input_bytes_pending() const
{
    return de265_get_number_of_input_bytes_pending(_context);
}

int Decoder::get_number_of_nal_units_pending() const
{
    return de265_get_number_of_NAL_units_pending(_context);
}

DecodingResult Decoder::decode()
{
    int more;
    auto error = de265_decode(_context, &more);
    return DecodingResult{error, more != 0};
}

void Decoder::reset()
{
    de265_reset(_context);
}

std::optional<Image> Decoder::peek_next_picture()
{
    auto image = de265_peek_next_picture(_context);
    if (image == nullptr)
    {
        return std::nullopt;
    }
    return Image(_context, const_cast<de265_image *>(image));
}

std::optional<Image> Decoder::get_next_picture()
{
    auto image = de265_get_next_picture(_context);
    if (image == nullptr)
    {
        return std::nullopt;
    }
    return Image(_context, const_cast<de265_image *>(image));
}

de265_error Decoder::get_warning() const
{
    return de265_get_warning(_context);
}

int Decoder::get_highest_tid() const
{
    return de265_get_highest_TID(_context);
}

int Decoder::get_current_tid() const
{
    return de265_get_current_TID(_context);
}

void Decoder::set_limit_tid(int max_tid)
{
    de265_set_limit_TID(_context, max_tid);
}

void Decoder::set_framerate_ratio(int percent)
{
    de265_set_framerate_ratio(_context, percent);
}

int Decoder::change_framerate(int more_vs_less)
{
    return de265_change_framerate(_context, more_vs_less);
}

void Decoder::set_parameter_bool(enum de265_param param, bool value)
{
    de265_set_parameter_bool(_context, param, value);
}

bool Decoder::get_parameter_bool(enum de265_param param)
{
    return (bool)de265_get_parameter_bool(_context, param);
}

EMSCRIPTEN_BINDINGS(decoder)
{
    value_object<DecodingResult>("DecodingResult")
        .field("error", &DecodingResult::error)
        .field("more", &DecodingResult::more);

    enum_<enum de265_param>("DecoderParameter", enum_value_type::number)
        .value("DISABLE_DEBLOCKING", DE265_DECODER_PARAM_DISABLE_DEBLOCKING)
        .value("DISABLE_SAO", DE265_DECODER_PARAM_DISABLE_SAO);

    class_<Decoder>("Decoder")
        .constructor()
#ifdef HAS_PTHREAD
        .function("startWorkerThreads(threads)", &Decoder::start_worker_threads)
#endif
        .function("pushData(input, pts)", &Decoder::push_data)
        .function("pushEndOfNal", &Decoder::push_end_of_nal)
        .function("pushEndOfFrame", &Decoder::push_end_of_frame)
        .function("pushNal(input, pts)", &Decoder::push_nal)
        .function("flushData", &Decoder::flush_data)
        .property("inputBytesPending", &Decoder::get_number_of_input_bytes_pending)
        .property("nalUnitsPending", &Decoder::get_number_of_nal_units_pending)
        .function("decode", &Decoder::decode)
        .function("reset", &Decoder::reset)
        .function("peekNextPicture", &Decoder::peek_next_picture)
        .function("getNextPicture", &Decoder::get_next_picture)
        .property("warning", &Decoder::get_warning)
        .property("highestTid", &Decoder::get_highest_tid)
        .property("currentTid", &Decoder::get_current_tid)
        .function("setLimitTid(maxTid)", &Decoder::set_limit_tid)
        .function("setFrameRatio(percent)", &Decoder::set_framerate_ratio)
        .function("changeFrameRate(moreOrLess)", &Decoder::change_framerate)
        .function("setParameterBool(param, value)", &Decoder::set_parameter_bool)
        .function("getParameterBool(param)", &Decoder::get_parameter_bool);
}
