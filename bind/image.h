#pragma once

#include <emscripten/bind.h>

#include <libde265/de265.h>

using namespace emscripten;

class Encoder;

// `EMSCRIPTEN_DECLARE_VAL_TYPE(Uint8Array)` don't have a default constructor
// so can't be used in `value_object`
struct Uint8Array : val
{
    Uint8Array() : val() {}
    Uint8Array(val const &other) : val(other) {}
};

struct ImagePlane
{
    int width;
    int height;
    Uint8Array bytes;
    int stride;
};

class Image
{
private:
    de265_decoder_context *_decoder;

public:
    struct de265_image *image;

    Image(de265_decoder_context *context, struct de265_image *image);
    Image(struct de265_image *image);

    ~Image();

    int get_width(int channel) const;
    int get_height(int channel) const;
    de265_chroma get_chroma_format() const;
    int get_bits_per_pixel(int channel) const;
    de265_PTS get_pts() const;
    ImagePlane get_plane(int channel) const;
    bool get_full_range_flag() const;
    int get_colour_primaries() const;
    int get_transfer_characteristics() const;
    int get_matrix_coefficients() const;

#ifdef HAS_ENCODER
    void alloc_plane(int channel, std::string input);
    void free_plane(int channel);
#endif
};
