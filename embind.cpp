#include <emscripten/bind.h>
#include <libde265/de265.h>

using namespace emscripten;

struct DecodingResult {
    de265_error error;
    bool more;
};

EMSCRIPTEN_DECLARE_VAL_TYPE(ImagePlane);

class Image {
    private:
        de265_decoder_context* _context;
        const struct de265_image* _image;

    public:
        Image(de265_decoder_context* context, const struct de265_image* image)
            : _context(context), _image(image) {}

        ~Image(){
            de265_release_next_picture(_context);
        }

        int get_width(int channel) {
            return de265_get_image_width(_image, channel);
        }

        int get_height(int channel) {
            return de265_get_image_height(_image, channel);
        }

        de265_chroma get_chroma_format() const {
            return de265_get_chroma_format(_image);
        }

        int get_bits_per_pixel(int channel) {
            return de265_get_bits_per_pixel(_image, channel);
        }

        de265_PTS get_pts() const {
            return de265_get_image_PTS(_image);
        }

        ImagePlane get_image_plane(int channel) {
            int stride;
            auto data = de265_get_image_plane(_image, channel, &stride);
            auto width = this->get_width(channel);
            auto height = this->get_height(channel);
            auto bits_per_pixel = this->get_bits_per_pixel(channel);

            auto result = val::object();
            result.set("width", width);
            result.set("height", height);
            result.set("bytes", val(typed_memory_view(width * height * bits_per_pixel / 8, data)));
            result.set("stride", stride);
            return ImagePlane(result);
        }

        bool get_full_range_flag() const {
            return (bool) de265_get_image_full_range_flag(_image);
        }

        int get_colour_primaries() const {
            return de265_get_image_colour_primaries(_image);
        }

        int get_transfer_characteristics() const {
            return de265_get_image_transfer_characteristics(_image);
        }

        int get_matrix_coefficients() const {
            return de265_get_image_matrix_coefficients(_image);
        }
};

class Decoder {
    private:
        de265_decoder_context* _context;

    public:
        Decoder() {
            _context = de265_new_decoder();
        }

        ~Decoder() {
            de265_free_decoder(_context);
        }

        de265_error start_worker_threads(int threads) {
            return de265_start_worker_threads(_context, threads);
        }

        de265_error push_data(std::string input, de265_PTS pts) {
            return de265_push_data(_context, input.data(), input.length(), pts, nullptr);
        }

        void push_end_of_nal() {
            de265_push_end_of_NAL(_context);
        }

        void push_end_of_frame() {
            de265_push_end_of_frame(_context);
        }

        de265_error push_nal(std::string input, de265_PTS pts) {
            return de265_push_NAL(_context, input.data(), input.length(), pts, nullptr);
        }

        de265_error flush_data() {
            return de265_flush_data(_context);
        }

        int get_number_of_input_bytes_pending() const {
            return de265_get_number_of_input_bytes_pending(_context);
        }

        int get_number_of_nal_units_pending() const {
            return de265_get_number_of_NAL_units_pending(_context);
        }

        DecodingResult decode() {
            int more;
            auto error = de265_decode(_context, &more);
            return DecodingResult {error, more != 0};
        }

        void reset() {
            de265_reset(_context);
        }

        Image* peek_next_picture() {
            auto image = de265_peek_next_picture(_context);
            if (image == nullptr) {
                return nullptr;
            }
            return new Image(_context, image);
        }

        Image* get_next_picture() {
            auto image = de265_get_next_picture(_context);
            if (image == nullptr) {
                return nullptr;
            }
            return new Image(_context, image);
        }

        de265_error get_warning() const {
            return de265_get_warning(_context);
        }

        int get_highest_tid() const {
            return de265_get_highest_TID(_context);
        }

        int get_current_tid() const {
            return de265_get_current_TID(_context);
        }

        void set_limit_tid(int max_tid) {
            de265_set_limit_TID(_context, max_tid);
        }

        void set_framerate_ratio(int percent) {
            de265_set_framerate_ratio(_context, percent);
        }

        int change_framerate(int more_vs_less) {
            return de265_change_framerate(_context, more_vs_less);
        }

        void setParameterBool(enum de265_param param, bool value) {
            de265_set_parameter_bool(_context, param, value);
        }

        bool getParameterBool(enum de265_param param) {
            return (bool) de265_get_parameter_bool(_context, param);
        }
};

EMSCRIPTEN_BINDINGS(de265) {
    function("get_version", &de265_get_version_number);

    function("get_version_major", &de265_get_version_number_major);
    function("get_version_minor", &de265_get_version_number_minor);
    function("get_version_maintenance", &de265_get_version_number_maintenance);

    enum_<de265_error>("Error", enum_value_type::number)
        .value("OK", DE265_OK)
        .value("ERROR_NO_SUCH_FILE", DE265_ERROR_NO_SUCH_FILE)
        .value("ERROR_COEFFICIENT_OUT_OF_IMAGE_BOUNDS", DE265_ERROR_COEFFICIENT_OUT_OF_IMAGE_BOUNDS)
        .value("ERROR_CHECKSUM_MISMATCH", DE265_ERROR_CHECKSUM_MISMATCH)
        .value("ERROR_CTB_OUTSIDE_IMAGE_AREA", DE265_ERROR_CTB_OUTSIDE_IMAGE_AREA)
        .value("ERROR_OUT_OF_MEMORY", DE265_ERROR_OUT_OF_MEMORY)
        .value("ERROR_CODED_PARAMETER_OUT_OF_RANGE", DE265_ERROR_CODED_PARAMETER_OUT_OF_RANGE)
        .value("ERROR_IMAGE_BUFFER_FULL", DE265_ERROR_IMAGE_BUFFER_FULL)
        .value("ERROR_CANNOT_START_THREADPOOL", DE265_ERROR_CANNOT_START_THREADPOOL)
        .value("ERROR_LIBRARY_INITIALIZATION_FAILED", DE265_ERROR_LIBRARY_INITIALIZATION_FAILED)
        .value("ERROR_LIBRARY_NOT_INITIALIZED", DE265_ERROR_LIBRARY_NOT_INITIALIZED)
        .value("ERROR_WAITING_FOR_INPUT_DATA", DE265_ERROR_WAITING_FOR_INPUT_DATA)
        .value("ERROR_CANNOT_PROCESS_SEI", DE265_ERROR_CANNOT_PROCESS_SEI)
        .value("ERROR_PARAMETER_PARSING", DE265_ERROR_PARAMETER_PARSING)
        .value("ERROR_NO_INITIAL_SLICE_HEADER", DE265_ERROR_NO_INITIAL_SLICE_HEADER)
        .value("ERROR_PREMATURE_END_OF_SLICE", DE265_ERROR_PREMATURE_END_OF_SLICE)
        .value("ERROR_UNSPECIFIED_DECODING_ERROR", DE265_ERROR_UNSPECIFIED_DECODING_ERROR)
        .value("ERROR_NOT_IMPLEMENTED_YET", DE265_ERROR_NOT_IMPLEMENTED_YET)
        .value("WARNING_NO_WPP_CANNOT_USE_MULTITHREADING", DE265_WARNING_NO_WPP_CANNOT_USE_MULTITHREADING)
        .value("WARNING_WARNING_BUFFER_FULL", DE265_WARNING_WARNING_BUFFER_FULL)
        .value("WARNING_PREMATURE_END_OF_SLICE_SEGMENT", DE265_WARNING_PREMATURE_END_OF_SLICE_SEGMENT)
        .value("WARNING_INCORRECT_ENTRY_POINT_OFFSET", DE265_WARNING_INCORRECT_ENTRY_POINT_OFFSET)
        .value("WARNING_CTB_OUTSIDE_IMAGE_AREA", DE265_WARNING_CTB_OUTSIDE_IMAGE_AREA)
        .value("WARNING_SPS_HEADER_INVALID", DE265_WARNING_SPS_HEADER_INVALID)
        .value("WARNING_PPS_HEADER_INVALID", DE265_WARNING_PPS_HEADER_INVALID)
        .value("WARNING_SLICEHEADER_INVALID", DE265_WARNING_SLICEHEADER_INVALID)
        .value("WARNING_INCORRECT_MOTION_VECTOR_SCALING", DE265_WARNING_INCORRECT_MOTION_VECTOR_SCALING)
        .value("WARNING_NONEXISTING_PPS_REFERENCED", DE265_WARNING_NONEXISTING_PPS_REFERENCED)
        .value("WARNING_NONEXISTING_SPS_REFERENCED", DE265_WARNING_NONEXISTING_SPS_REFERENCED)
        .value("WARNING_BOTH_PREDFLAGS_ZERO", DE265_WARNING_BOTH_PREDFLAGS_ZERO)
        .value("WARNING_NONEXISTING_REFERENCE_PICTURE_ACCESSED", DE265_WARNING_NONEXISTING_REFERENCE_PICTURE_ACCESSED)
        .value("WARNING_NUMMVP_NOT_EQUAL_TO_NUMMVQ", DE265_WARNING_NUMMVP_NOT_EQUAL_TO_NUMMVQ)
        .value("WARNING_NUMBER_OF_SHORT_TERM_REF_PIC_SETS_OUT_OF_RANGE", DE265_WARNING_NUMBER_OF_SHORT_TERM_REF_PIC_SETS_OUT_OF_RANGE)
        .value("WARNING_SHORT_TERM_REF_PIC_SET_OUT_OF_RANGE", DE265_WARNING_SHORT_TERM_REF_PIC_SET_OUT_OF_RANGE)
        .value("WARNING_FAULTY_REFERENCE_PICTURE_LIST", DE265_WARNING_FAULTY_REFERENCE_PICTURE_LIST)
        .value("WARNING_EOSS_BIT_NOT_SET", DE265_WARNING_EOSS_BIT_NOT_SET)
        .value("WARNING_MAX_NUM_REF_PICS_EXCEEDED", DE265_WARNING_MAX_NUM_REF_PICS_EXCEEDED)
        .value("WARNING_INVALID_CHROMA_FORMAT", DE265_WARNING_INVALID_CHROMA_FORMAT)
        .value("WARNING_SLICE_SEGMENT_ADDRESS_INVALID", DE265_WARNING_SLICE_SEGMENT_ADDRESS_INVALID)
        .value("WARNING_DEPENDENT_SLICE_WITH_ADDRESS_ZERO", DE265_WARNING_DEPENDENT_SLICE_WITH_ADDRESS_ZERO)
        .value("WARNING_NUMBER_OF_THREADS_LIMITED_TO_MAXIMUM", DE265_WARNING_NUMBER_OF_THREADS_LIMITED_TO_MAXIMUM)
        .value("NON_EXISTING_LT_REFERENCE_CANDIDATE_IN_SLICE_HEADER", DE265_NON_EXISTING_LT_REFERENCE_CANDIDATE_IN_SLICE_HEADER)
        .value("WARNING_CANNOT_APPLY_SAO_OUT_OF_MEMORY", DE265_WARNING_CANNOT_APPLY_SAO_OUT_OF_MEMORY)
        .value("WARNING_SPS_MISSING_CANNOT_DECODE_SEI", DE265_WARNING_SPS_MISSING_CANNOT_DECODE_SEI)
        .value("WARNING_COLLOCATED_MOTION_VECTOR_OUTSIDE_IMAGE_AREA", DE265_WARNING_COLLOCATED_MOTION_VECTOR_OUTSIDE_IMAGE_AREA)
        .value("WARNING_PCM_BITDEPTH_TOO_LARGE", DE265_WARNING_PCM_BITDEPTH_TOO_LARGE)
        .value("WARNING_REFERENCE_IMAGE_BIT_DEPTH_DOES_NOT_MATCH", DE265_WARNING_REFERENCE_IMAGE_BIT_DEPTH_DOES_NOT_MATCH)
        .value("WARNING_REFERENCE_IMAGE_SIZE_DOES_NOT_MATCH_SPS", DE265_WARNING_REFERENCE_IMAGE_SIZE_DOES_NOT_MATCH_SPS)
        .value("WARNING_CHROMA_OF_CURRENT_IMAGE_DOES_NOT_MATCH_SPS", DE265_WARNING_CHROMA_OF_CURRENT_IMAGE_DOES_NOT_MATCH_SPS)
        .value("WARNING_BIT_DEPTH_OF_CURRENT_IMAGE_DOES_NOT_MATCH_SPS", DE265_WARNING_BIT_DEPTH_OF_CURRENT_IMAGE_DOES_NOT_MATCH_SPS)
        .value("WARNING_REFERENCE_IMAGE_CHROMA_FORMAT_DOES_NOT_MATCH", DE265_WARNING_REFERENCE_IMAGE_CHROMA_FORMAT_DOES_NOT_MATCH)
        .value("WARNING_INVALID_SLICE_HEADER_INDEX_ACCESS", DE265_WARNING_INVALID_SLICE_HEADER_INDEX_ACCESS)
        ;

    function("getErrorText(error)", +[](de265_error error) {
        return std::string(de265_get_error_text(error));
    });

    function("isOk(error)", +[](de265_error error) {
        return (bool) de265_isOK(error);
    });

    enum_<de265_chroma>("Chroma", enum_value_type::number)
        .value("MONO", de265_chroma_mono)
        .value("420", de265_chroma_420)
        .value("422", de265_chroma_422)
        .value("444", de265_chroma_444)
        ;

    register_type<ImagePlane>("{ width: number, height: number, bytes: Uint8Array; stride: number }");

    class_<Image>("Image")
        .function("getWidth(channel)", &Image::get_width)
        .function("getHeight(channel)", &Image::get_height)
        .property("chromaFormat", &Image::get_chroma_format)
        .function("getBitsPerPixel(channel)", &Image::get_bits_per_pixel)
        .function("getImagePlane(channel)", &Image::get_image_plane)
        .property("pts", &Image::get_pts)
        .property("isFullRange", &Image::get_full_range_flag)
        .property("colorPrimaries", &Image::get_colour_primaries)
        .property("transferCharacteristics", &Image::get_transfer_characteristics)
        .property("matrixCoefficients", &Image::get_matrix_coefficients)
        ;

    value_object<DecodingResult>("DecodingResult")
        .field("error", &DecodingResult::error)
        .field("more", &DecodingResult::more)
        ;

    class_<Decoder>("Decoder")
        .constructor()
        .function("startWorkerThreads(threads)", &Decoder::start_worker_threads)
        .function("pushData(input, pts)", &Decoder::push_data)
        .function("pushEndOfNal", &Decoder::push_end_of_nal)
        .function("pushEndOfFrame", &Decoder::push_end_of_frame)
        .function("pushNal(input, pts)", &Decoder::push_nal)
        .function("flushData", &Decoder::flush_data)
        .property("inputBytesPending", &Decoder::get_number_of_input_bytes_pending)
        .property("nalUnitsPending", &Decoder::get_number_of_nal_units_pending)
        .function("decode", &Decoder::decode)
        .function("reset", &Decoder::reset)
        .function("peekNextPicture", &Decoder::peek_next_picture, return_value_policy::take_ownership())
        .function("getNextPicture", &Decoder::get_next_picture, return_value_policy::take_ownership())
        .property("warning", &Decoder::get_warning)
        .property("highestTid", &Decoder::get_highest_tid)
        .property("currentTid", &Decoder::get_current_tid)
        .function("setLimitTid(maxTid)", &Decoder::set_limit_tid)
        .function("setFrameRatio(percent)", &Decoder::set_framerate_ratio)
        .function("changeFrameRate(moreOrLess)", &Decoder::change_framerate)
        .function("setParameterBool(param, value)", &Decoder::setParameterBool)
        .function("getParameterBool(param)", &Decoder::getParameterBool)
        ;

    enum_<enum de265_param>("Param", enum_value_type::number)
        .value("DISABLE_DEBLOCKING", DE265_DECODER_PARAM_DISABLE_DEBLOCKING)
        .value("DISABLE_SAO", DE265_DECODER_PARAM_DISABLE_SAO)
        ;
}
