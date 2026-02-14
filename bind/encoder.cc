#include "encoder.h"

Encoder::Encoder() : _context(en265_new_encoder()) {}

Encoder::~Encoder() { en265_free_encoder(_context); }

static StringArray to_string_array(const char **strings)
{
    val result = val::array();
    while (*strings != nullptr)
    {
        result.call<void>("push", val::u8string(*strings));
    }
    return StringArray(result);
}

StringArray Encoder::list_parameters()
{
    auto list = en265_list_parameters(_context);
    return to_string_array(list);
}

enum en265_parameter_type Encoder::get_parameter_type(std::string name)
{
    return en265_get_parameter_type(_context, name.c_str());
}

StringArray Encoder::list_parameter_choices(std::string name)
{
    auto list = en265_list_parameter_choices(_context, name.c_str());
    return to_string_array(list);
}

de265_error Encoder::set_parameter_bool(std::string name, bool value)
{
    return en265_set_parameter_bool(_context, name.c_str(), value);
}

de265_error Encoder::set_parameter_int(std::string name, int value)
{
    return en265_set_parameter_int(_context, name.c_str(), value);
}

de265_error Encoder::set_parameter_string(std::string name, std::string value)
{
    return en265_set_parameter_string(_context, name.c_str(), value.c_str());
}

de265_error Encoder::set_parameter_choice(std::string name, std::string value)
{
    return en265_set_parameter_choice(_context, name.c_str(), value.c_str());
}

de265_error Encoder::start(int threads)
{
    return en265_start_encoder(_context, threads);
}

Image Encoder::allocate_image(int width, int height, enum de265_chroma chroma, de265_PTS pts)
{
    auto image = en265_allocate_image(_context, width, height, chroma, pts, nullptr);
    return Image(this, image);
}

de265_error Encoder::push_image(Image &image)
{
    return en265_push_image(_context, image.image);
}

de265_error Encoder::push_eof()
{
    return en265_push_eof(_context);
}

int Encoder::get_current_input_queue_length() const
{
    return en265_current_input_queue_length(_context);
}

de265_error Encoder::encode()
{
    return en265_encode(_context);
}

en265_encoder_state Encoder::get_state() const
{
    return en265_get_encoder_state(_context);
}

std::optional<Packet> Encoder::get_packet(int timeout_ms)
{
    auto packet = en265_get_packet(_context, timeout_ms);
    if (packet == nullptr)
    {
        return std::nullopt;
    }
    return Packet(packet);
}

int Encoder::get_number_of_queued_packets() const
{
    return en265_number_of_queued_packets(_context);
}

EMSCRIPTEN_BINDINGS(encoder)
{
    register_type<StringArray>("string[]");

    enum_<enum en265_parameter_type>("EncoderParameterType", enum_value_type::number)
        .value("BOOL", en265_parameter_bool)
        .value("INT", en265_parameter_int)
        .value("STRING", en265_parameter_string)
        .value("CHOICE", en265_parameter_choice);

    class_<Encoder>("Encoder")
        .constructor()
        .function("listParameters", &Encoder::list_parameters)
        .function("getParameterType(name)", &Encoder::get_parameter_type)
        .function("listParameterChoices(name)", &Encoder::list_parameter_choices)
        .function("setParameterBool(name, value)", &Encoder::set_parameter_bool)
        .function("setParameterInt(name, value)", &Encoder::set_parameter_int)
        .function("setParameterString(name, value)", &Encoder::set_parameter_string)
        .function("setParameterChoice(name, value)", &Encoder::set_parameter_choice)
        .function("start(threads)", &Encoder::start)
        .function("allocateImage(width, height, chroma, pts)", &Encoder::allocate_image)
        .function("pushImage(image)", &Encoder::push_image)
        .function("pushEof", &Encoder::push_eof)
        .property("currentInputQueueLength", &Encoder::get_current_input_queue_length)
        .function("encode", &Encoder::encode)
        .property("state", &Encoder::get_state)
        .function("getPacket(timeout_ms)", &Encoder::get_packet)
        .property("numberOfQueuedPackets", &Encoder::get_number_of_queued_packets);

    enum_<enum en265_encoder_state>("EncoderState", enum_value_type::number)
        .value("IDLE", EN265_STATE_IDLE)
        .value("WAITING_FOR_INPUT", EN265_STATE_WAITING_FOR_INPUT)
        .value("WORKING", EN265_STATE_WORKING)
        .value("OUTPUT_QUEUE_FULL", EN265_STATE_OUTPUT_QUEUE_FULL)
        .value("EOS", EN265_STATE_EOS);
}
