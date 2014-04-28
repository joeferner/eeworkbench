#include "RingBuffer.h"

RingBuffer::RingBuffer(uint size) :
  m_bufferSize(size),
  m_writeIndex(0),
  m_readIndex(-1)
{
  m_buffer = new uchar[size];
}

RingBuffer::~RingBuffer() {
  delete m_buffer;
}

void RingBuffer::write(uchar b) {
  m_buffer[m_writeIndex] = b;
  if(m_readIndex == -1) {
    // if buffer is empty, set the read index to the
    // current write index. because that will be the first
    // slot to be read later.
    m_readIndex = m_writeIndex;
  }
  m_writeIndex = (m_writeIndex + 1) % m_bufferSize;
}

uchar RingBuffer::read() {
  if(m_readIndex == -1) {
    return 0;
  }

  uchar ret = m_buffer[m_readIndex];
  m_readIndex = (m_readIndex + 1) % m_bufferSize;
  if(m_readIndex == m_writeIndex) {
    m_readIndex = -1;
  }
  return ret;
}

int RingBuffer::available() {
  if(m_readIndex == -1) {
    return 0;
  }
  if(m_readIndex > m_writeIndex) {
    return (m_bufferSize - m_readIndex) + m_writeIndex;
  } else {
    return m_writeIndex - m_readIndex;
  }
}

QString RingBuffer::readAsString() {
  QString str = "";
  while(available() > 0) {
    str.append(read());
  }
  return str;
}
