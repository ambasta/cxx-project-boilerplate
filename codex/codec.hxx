#ifndef CODEX_CODEC
#define CODEX_CODEC

#include "format.hxx"
#include "properties.hxx"
#include <algorithm>
#include <ranges>

// TODO: Replace EncodingT with EncoderConcept
template <typename EncodingT> class Codec {
private:
  template <std::ranges::viewable_range RangeT>
  bool is_valid(RangeT data) const noexcept {
    if (EncodingT::padding != Padding::none) {

      constexpr auto padding_start =
          std::ranges::find_first_of(data, EncodingT::padding_value);

      if (std::any_of(
              std::ranges::subrange(padding_start, std::ranges::end(data)),
              [](const auto &value) {
                return value != EncodingT::padding_value;
              }))
        return false;
      data = std::ranges::subrange(std::ranges::begin(data), padding_start);
    }

    for (const auto &value : data) {
      if (std::ranges::find(EncodingT::alphabet, value) ==
          EncodingT::alphabet.end())
        return false;
    }
    return true;
  }

public:
  template <std::ranges::input_range RangeT, typename U,
            std::ranges::output_range<U> OutRangeT>
  void encode(const RangeT &data, OutRangeT &result) {
    EncodingT::encode(data, result);
  }

  template <
      std::ranges::input_range RangeT,
      std::ranges::output_range<std::ranges::range_value_t<RangeT>> OutRangeT>
  void decode(const RangeT &data, OutRangeT &result) {
    if (not is_valid(data))
      throw std::invalid_argument(
          std::format("Mismatched or corrupted input: ", data));
    EncodingT::decode(data, result);
  }

  template <std::ranges::input_range RangeT,
            std::ranges::output_range<std::ranges::range_value_t<RangeT>>
                OutRangeT = std::vector<uint8_t>>
  OutRangeT decode(RangeT data) const {
    OutRangeT out_range;
    decode(data, out_range);
    return out_range;
  }
};

#endif
