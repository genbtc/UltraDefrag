/* Any copyright is dedicated to the Public Domain.
   http://creativecommons.org/publicdomain/zero/1.0/ */
/* Written by Nils Maier in 2014. */

#ifndef OP_HPP
#define OP_HPP

struct Operation {
  zen::winx zw;
  zen::Volume vol;
  std::unique_ptr<zen::GapEnumeration> ge;
  std::unique_ptr<zen::FileEnumeration> fe;
  Options opts;
  size_t moved;
  uint64_t movedLen;
  uint64_t start;
  uint64_t freq;
  const winx_file_info *last;
  bool replaced;

  Operation()
        : moved(0), movedLen(0), start(0), last(nullptr), replaced(false)
    {
    LARGE_INTEGER li;
    ::QueryPerformanceFrequency(&li);
    freq = li.QuadPart;
  }
  void init();
  void run();
  double seconds() const {
    LARGE_INTEGER li;
    ::QueryPerformanceCounter(&li);
    return (li.QuadPart - start) / (double)freq;
  }
  std::wstring metrics() const;
};

class Exit : public std::exception
{
public:
    explicit Exit(int code) : std::exception("Exiting program"), code_(code) {}
  const int code_;
};

#endif // OP_HPP
