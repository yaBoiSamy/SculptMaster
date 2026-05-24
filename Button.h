#ifndef Button_h
#define Button_h

class Button {
  public:
    Button(int pin);
    bool read();
    static int debounceDelay;

  private:
    int pin;

    bool state;
    bool isBouncing;

    int lastStateChange;
};


#endif