#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <QtGlobal>
#include <QString>

class RingBuffer
{
public:
  RingBuffer(uint size);
  virtual ~RingBuffer();
  void write(uchar b);
  uchar read();
  int available();
  QString readAsString();

private:
  uchar* m_buffer;
  int m_bufferSize;
  int m_writeIndex;
  int m_readIndex;
};

#endif // RINGBUFFER_H
