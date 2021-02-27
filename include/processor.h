#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  Processor();
  float Utilization();  // See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
 long Total_;
 long Idle_;
};

#endif