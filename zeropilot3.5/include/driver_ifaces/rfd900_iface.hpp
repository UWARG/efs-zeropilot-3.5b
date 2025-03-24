#pragma once

class IRFD900 { // TODO: Placeholder for now, Replace with actual driver iface by driver people
  protected:
    IRFD900() = default;

  public:
    virtual ~IRFD900() = default;

    virtual void transmit();

    uint8_t *buf_;
};