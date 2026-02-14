#pragma once

#include <libde265/en265.h>

#include "image.h"

class Packet
{
private:
    struct en265_packet *_packet;

public:
    Packet(struct en265_packet *packet);

    ~Packet();

    int get_version() const;

    Uint8Array get_data() const;

    int get_frame_number() const;

    enum en265_packet_content_type get_content_type() const;

    bool is_complete_picture() const;
    bool is_final_slice() const;
    bool is_dependent_slice() const;

    enum en265_nal_unit_type get_nal_unit_type() const;
    uint8_t get_nuh_layer_id() const;
    uint8_t get_nuh_temporal_id() const;

    Image get_input_image() const;
    Image get_reconstruction() const;
};
