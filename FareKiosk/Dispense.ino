void initDispense() {
#ifdef DEV_MODE
  dispense1.attach(23);
  dispense2.attach(22);
  dispense3.attach(21);
  dispense4.attach(19);
  gate.attach(18);
#else
  dispense1.attach(25);
  dispense2.attach(26);
  dispense3.attach(12);
  dispense4.attach(23);
  gate.attach(15);
#endif
  actionArrayInvoke([&](int i) {
    servos[i].write(0);
  }, (sizeof(servos) / sizeof(servos[0])));
}

void dispense() {
  for (int i = 0; i < 3; i++) {
  }
}

// void actionInvoke(std::function<void(int)> servo) {
//   for (int i = 0; i < numServos; i++) {
//     servo(i)
//   }
// }