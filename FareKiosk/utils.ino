inline void actionArrayInvoke(std::function<void(int)> servo, int size) {
  for (int i = 0; i < size; i++) {
    servo(i);  // Call the function with both parameters
  }
}
