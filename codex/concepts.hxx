#ifndef CODEX_CONCEPTS
#define CODEX_CONCEPTS

#include "properties.hxx"
#include <concepts>
#include <functional>
#include <ranges>
#include <type_traits>
#include <vector>

template <typename RangeT>
concept EncodingInputConcept = std::ranges::input_range<RangeT> and
    std::convertible_to<std::ranges::range_value_t<RangeT>, uint8_t>;

template <typename RangeT, typename ValueT>
concept EncodingOutputConcept = std::ranges::output_range<RangeT, ValueT> and
    std::convertible_to<ValueT, uint8_t>;

// encoder -> should take vector<uint8_t> and return encoded string
// encoder -> should take uint8_t*, size and return encoded string
// decoder -> should take encoded string and return vector<uint8_t>

template <typename EncoderT>
concept EncoderConcept = requires(const std::vector<uint8_t> &input) {
  EncoderT::alphabet;
  requires std::ranges::input_range<decltype(EncoderT::encode(input))>;

  EncoderT::padding_policy;
  requires std::same_as<std::remove_const_t<decltype(EncoderT::padding_policy)>,
                        Padding>;

  requires EncoderT::padding_policy == Padding::none or requires {
    EncoderT::padding;
    std::convertible_to<decltype(EncoderT::padding), uint8_t>;
  };

  { EncoderT::encode(input) } -> std::convertible_to<std::string_view>;

  {
    EncoderT::encode(input.data(), input.size())
    } -> std::convertible_to<std::string_view>;

  {
    EncoderT::decode(std::string_view{})
    } -> std::convertible_to<std::vector<uint8_t>>;
};

template <typename CodecT, typename EncoderT, typename EncodingInputT,
          typename OutputValueT, typename EncodingOutputT>
concept CodecConcept = EncoderConcept<EncoderT> and
    EncodingInputConcept<EncodingInputT> and
    EncodingOutputConcept<EncodingOutputT, OutputValueT> and
    requires(CodecT codec, EncodingInputT input, EncodingOutputT &output) {
  { codec.encode(input) } -> std::convertible_to<EncodingOutputT>;
  { codec.encode(input, output) } -> std::convertible_to<void>;

  { codec.decode(input) } -> std::convertible_to<OutputValueT>;
  { codec.decode(input, output) } -> std::convertible_to<void>;
};
#endif
