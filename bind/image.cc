#include <libde265/en265.h>

#include "image.h"
#include "encoder.h"

using namespace emscripten;

Image::Image(de265_decoder_context *context, struct de265_image *image)
    : _decoder(context), image(image) {}

Image::Image(struct de265_image *image)
    : Image(nullptr, image) {}

Image::~Image()
{
    if (_decoder != nullptr)
    {
        de265_release_next_picture(_decoder);
    }
}

int Image::get_width(int channel) const
{
    return de265_get_image_width(image, channel);
}

int Image::get_height(int channel) const
{
    return de265_get_image_height(image, channel);
}

de265_chroma Image::get_chroma_format() const
{
    return de265_get_chroma_format(image);
}

int Image::get_bits_per_pixel(int channel) const
{
    return de265_get_bits_per_pixel(image, channel);
}

de265_PTS Image::get_pts() const
{
    return de265_get_image_PTS(image);
}

ImagePlane Image::get_plane(int channel) const
{
    int stride;
    auto data = de265_get_image_plane(image, channel, &stride);
    auto width = this->get_width(channel);
    auto height = this->get_height(channel);
    auto bits_per_pixel = this->get_bits_per_pixel(channel);

    return ImagePlane{
        width,
        height,
        Uint8Array(val(typed_memory_view(width * height * bits_per_pixel / 8, data))),
        stride,
    };
}

bool Image::get_full_range_flag() const
{
    return (bool)de265_get_image_full_range_flag(image);
}

int Image::get_colour_primaries() const
{
    return de265_get_image_colour_primaries(image);
}

int Image::get_transfer_characteristics() const
{
    return de265_get_image_transfer_characteristics(image);
}

int Image::get_matrix_coefficients() const
{
    return de265_get_image_matrix_coefficients(image);
}

#ifdef HAS_ENCODER
void Image::alloc_plane(int channel, std::string input)
{
    de265_alloc_image_plane(image, channel, input.data(), input.length(), nullptr);
}

void Image::free_plane(int channel)
{
    de265_free_image_plane(image, channel);
}
#endif

EMSCRIPTEN_BINDINGS(image)
{
    register_type<Uint8Array>("Uint8Array");

    enum_<de265_chroma>("Chroma", enum_value_type::number)
        .value("MONO", de265_chroma_mono)
        .value("420", de265_chroma_420)
        .value("422", de265_chroma_422)
        .value("444", de265_chroma_444);

    value_object<ImagePlane>("ImagePlane")
        .field("width", &ImagePlane::width)
        .field("height", &ImagePlane::height)
        .field("bytes", &ImagePlane::bytes)
        .field("stride", &ImagePlane::stride);

    class_<Image>("Image")
#ifdef HAS_ENCODER
        .constructor(+[](Encoder &encoder, int width, int height, enum de265_chroma chroma, de265_PTS pts)
                     { return encoder.allocate_image(width, height, chroma, pts); })
#endif
        .function("getWidth(channel)", &Image::get_width)
        .function("getHeight(channel)", &Image::get_height)
        .property("chromaFormat", &Image::get_chroma_format)
        .function("getBitsPerPixel(channel)", &Image::get_bits_per_pixel)
        .function("getPlane(channel)", &Image::get_plane)
        .property("pts", &Image::get_pts)
        .property("isFullRange", &Image::get_full_range_flag)
        .property("colorPrimaries", &Image::get_colour_primaries)
        .property("transferCharacteristics", &Image::get_transfer_characteristics)
        .property("matrixCoefficients", &Image::get_matrix_coefficients)
#ifdef HAS_ENCODER
        .function("allocPlane(channel, input)", &Image::alloc_plane)
        .function("freePlane(channel)", &Image::free_plane)
#endif
        ;

    register_optional<Image>();
}
