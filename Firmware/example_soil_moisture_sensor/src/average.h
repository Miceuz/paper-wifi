template <typename T, typename Func>
T Average(const Func f) {
  T sum = 0;
  uint8_t averages = 0;

  for (uint8_t i = 0; i < 10; i++) {
    T m = f();
    if (static_cast<uint16_t>(m) != 0xffff) {
      sum += m;
      averages++;
    }
  }
  if (averages) {
    return sum / averages;
  } else {
    return 0;
  }
}
