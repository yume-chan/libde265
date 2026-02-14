#include "packet.h"

Packet::Packet(en265_packet *packet)
    : _packet(packet)
{
}

Packet::~Packet()
{
    en265_free_packet(_packet->encoder_context, _packet);
}

int Packet::get_version() const
{
    return _packet->version;
}

Uint8Array Packet::get_data() const
{
    return Uint8Array(val(typed_memory_view(_packet->length, _packet->data)));
}

int Packet::get_frame_number() const
{
    return _packet->frame_number;
}

en265_packet_content_type Packet::get_content_type() const
{
    return _packet->content_type;
}

bool Packet::is_complete_picture() const
{
    return _packet->complete_picture;
}

bool Packet::is_final_slice() const
{
    return _packet->final_slice;
}

bool Packet::is_dependent_slice() const
{
    return _packet->dependent_slice;
}

en265_nal_unit_type Packet::get_nal_unit_type() const
{
    return _packet->nal_unit_type;
}

uint8_t Packet::get_nuh_layer_id() const
{
    return _packet->nuh_layer_id;
}

uint8_t Packet::get_nuh_temporal_id() const
{
    return _packet->nuh_temporal_id;
}

Image Packet::get_input_image() const
{
    return Image(const_cast<struct de265_image *>(_packet->input_image));
}

Image Packet::get_reconstruction() const
{
    return Image(const_cast<struct de265_image *>(_packet->reconstruction));
}

EMSCRIPTEN_BINDINGS(packet)
{
    enum_<enum en265_packet_content_type>("PacketContentType", enum_value_type::number)
        .value("VPS", EN265_PACKET_VPS)
        .value("SPS", EN265_PACKET_SPS)
        .value("PPS", EN265_PACKET_PPS)
        .value("SEI", EN265_PACKET_SEI)
        .value("SLICE", EN265_PACKET_SLICE)
        .value("SKIPPED_IMAGE", EN265_PACKET_SKIPPED_IMAGE);

    enum_<en265_nal_unit_type>("NalUnitType", enum_value_type::number)
        .value("TRAIL_N", EN265_NUT_TRAIL_N)
        .value("TRAIL_R", EN265_NUT_TRAIL_R)
        .value("TSA_N", EN265_NUT_TSA_N)
        .value("TSA_R", EN265_NUT_TSA_R)
        .value("STSA_N", EN265_NUT_STSA_N)
        .value("STSA_R", EN265_NUT_STSA_R)
        .value("RADL_N", EN265_NUT_RADL_N)
        .value("RADL_R", EN265_NUT_RADL_R)
        .value("RASL_N", EN265_NUT_RASL_N)
        .value("RASL_R", EN265_NUT_RASL_R)
        .value("BLA_W_LP", EN265_NUT_BLA_W_LP)
        .value("BLA_W_RADL", EN265_NUT_BLA_W_RADL)
        .value("BLA_N_LP", EN265_NUT_BLA_N_LP)
        .value("IDR_W_RADL", EN265_NUT_IDR_W_RADL)
        .value("IDR_N_LP", EN265_NUT_IDR_N_LP)
        .value("CRA", EN265_NUT_CRA)
        .value("VPS", EN265_NUT_VPS)
        .value("SPS", EN265_NUT_SPS)
        .value("PPS", EN265_NUT_PPS)
        .value("AUD", EN265_NUT_AUD)
        .value("EOS", EN265_NUT_EOS)
        .value("EOB", EN265_NUT_EOB)
        .value("FD", EN265_NUT_FD)
        .value("PREFIX_SEI", EN265_NUT_PREFIX_SEI)
        .value("SUFFIX_SEI", EN265_NUT_SUFFIX_SEI);

    class_<Packet>("Packet")
        .property("version", &Packet::get_version)
        .property("data", &Packet::get_data)
        .property("frameNumber", &Packet::get_frame_number)
        .property("contentType", &Packet::get_content_type)
        .property("isCompletePicture", &Packet::is_complete_picture)
        .property("isFinalSlice", &Packet::is_final_slice)
        .property("isDependentSlice", &Packet::is_dependent_slice)
        .property("nalUnitType", &Packet::get_nal_unit_type)
        .property("nuhLayerId", &Packet::get_nuh_layer_id)
        .property("nuhTemporalId", &Packet::get_nuh_temporal_id)
        .property("inputImage", &Packet::get_input_image)
        .property("reconstruction", &Packet::get_reconstruction);

    register_optional<Packet>();
}
