#include "stream/writable.hpp"

// bool stream::writable::write(const std::string& data) {
//     return _write(std::span<const char>(data.begin(), data.end()));
// }
// bool stream::writable::write(std::span<const char> data) {
//     return _write(data);
// }
// bool stream::writable::write(std::string_view data) {
//     return _write(std::span<const char>(data.begin(), data.end()));
// }